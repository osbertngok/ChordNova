#include "algorithm/progression.h"
#include "service/expansion.h"
#include "service/voiceleading.h"
#include "algorithm/validation.h"
#include "model/chordstatistics.h"
#include "model/bigramchordstatistics.h"
#include "utility/combinatorics.h"
#include "utility/mixedradix.h"
#include "model/pitch.h"
#include "constant.h"
#include <cmath>
#include <set>

namespace chordnovarw::algorithm {

using namespace chordnovarw::model;
using namespace chordnovarw::service;
using namespace chordnovarw::utility;

ProgressionResult generate_single(
    const OrderedChord& initial_chord,
    const ProgressionConfig& config,
    const std::vector<int>& prev_single_chroma,
    double prev_chroma_old,
    const std::vector<OrderedChord>& record,
    ProgressCallback progress) {

  ProgressionResult result;

  const auto initial_stats = calculate_statistics(initial_chord);
  const int m_notes_size = static_cast<int>(initial_chord.get_num_of_pitches());
  const int m_max = config.range.m_max;
  const int vl_max = config.voice_leading.vl_max;
  const int vl_min = config.voice_leading.vl_min;

  // Mutable state for uniqueness tracking across all candidates
  std::vector<int> rec_ids;
  std::vector<long long> vec_ids;

  // Compute total iteration count for progress reporting
  const auto& cache = ExpansionIndexCache::instance();
  const int num_expansions = cache.count(m_notes_size, m_max);
  MixedRadixRange mutation_range(vl_max, m_max, vl_min);
  const long long mutations_per_expansion = mutation_range.total_count();
  const long long total_iterations = num_expansions * mutations_per_expansion;
  long long iteration_count = 0;

  for (int exp_idx = 0; exp_idx < num_expansions; ++exp_idx) {
    auto expansion = expand_single(initial_chord, m_max, exp_idx);
    auto exp_pitches = expansion.get_pitches();

    for (const auto& mutation_vec : MixedRadixRange(vl_max, m_max, vl_min)) {
      ++iteration_count;
      ++result.total_evaluated;

      // Apply mutation to expanded chord
      std::vector<Pitch> new_pitches;
      new_pitches.reserve(m_max);
      bool out_of_range = false;
      for (int i = 0; i < m_max; ++i) {
        int new_midi = exp_pitches[i].get_number() + mutation_vec[i];
        if (new_midi < 0 || new_midi > 127) { out_of_range = true; break; }
        new_pitches.push_back(Pitch(static_cast<char>(new_midi)));
      }
      if (out_of_range) {
        if (progress && (iteration_count % 10000 == 0))
          progress(iteration_count, total_iterations);
        continue;
      }

      OrderedChord candidate(std::move(new_pitches));

      // Build validation context
      ValidationContext ctx{
          config, initial_chord, initial_stats,
          VoiceLeadingResult{}, std::nullopt,
          rec_ids, vec_ids,
          prev_single_chroma, prev_chroma_old,
          record
      };

      // Run validation pipeline
      static ChordValidationPipeline pipeline;
      if (pipeline.validate(ctx, candidate)) {
        // Compute full bigram statistics for this candidate
        auto cand_stats = ctx.candidate_stats.has_value()
            ? *ctx.candidate_stats
            : calculate_statistics(candidate);

        auto bigram_stats = calculate_bigram_statistics(
            initial_chord, candidate,
            initial_stats, cand_stats,
            ctx.vl_result.vec, ctx.vl_result.sv,
            vl_max,
            prev_chroma_old,
            prev_single_chroma
        );

        result.candidates.push_back(
            CandidateEntry{std::move(candidate), std::move(bigram_stats)});
      }

      if (progress && (iteration_count % 10000 == 0)) {
        progress(iteration_count, total_iterations);
      }
    }
  }

  // Sort candidates according to config
  if (!config.sort.sort_order.empty()) {
    sort_candidates(result.candidates, config.sort.sort_order);
  }

  return result;
}

} // namespace chordnovarw::algorithm
