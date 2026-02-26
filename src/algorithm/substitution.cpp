#include "algorithm/substitution.h"
#include "service/voiceleading.h"
#include "algorithm/sorting.h"
#include "model/chordstatistics.h"
#include "model/pitch.h"
#include "constant.h"
#include <cmath>
#include <random>
#include <algorithm>
#include <numeric>
#include <bitset>
#include <set>

namespace chordnovarw::algorithm {

using namespace chordnovarw::model;
using namespace chordnovarw::service;

// ── Utility functions ─────────────────────────────────────────────

OrderedChord id_to_chord(int id) {
  std::vector<Pitch> pitches;
  int note = 72;
  int copy = id;
  while (copy != 0) {
    if (copy % 2 == 1)
      pitches.push_back(Pitch(static_cast<uint8_t>(note)));
    ++note;
    copy /= 2;
  }
  return OrderedChord(std::move(pitches));
}

OrderedChord reduce_to_octave6(const OrderedChord& chord) {
  std::bitset<ET_SIZE> pc_bits;
  for (const auto& p : chord.get_pitches())
    pc_bits.set(p.get_number() % ET_SIZE);

  std::vector<Pitch> pitches;
  for (int i = 0; i < ET_SIZE; ++i) {
    if (pc_bits.test(i))
      pitches.push_back(Pitch(static_cast<uint8_t>(72 + i)));
  }
  return OrderedChord(std::move(pitches));
}

int compute_substitution_similarity(int sv, bool same_root) {
  double temp = 1.0 - static_cast<double>(sv) / 36.0;
  if (temp < 0.0) temp = 0.0;
  if (same_root) temp = std::sqrt(temp);
  return static_cast<int>(std::round(100.0 * temp));
}

void compute_tolerance_range(ParamTolerance& tol) {
  if (tol.use_percentage) {
    tol.min_sub = tol.center * (1.0 - tol.radius / 100.0);
    tol.max_sub = tol.center * (1.0 + tol.radius / 100.0);
  } else {
    tol.min_sub = tol.center - tol.radius;
    tol.max_sub = tol.center + tol.radius;
  }
}

// ── Internal helpers ──────────────────────────────────────────────

namespace {

bool contains_char(const std::string& s, char c) {
  return s.find(c) != std::string::npos;
}

ParamTolerance& get_tolerance(SubstitutionConfig& config, char var) {
  switch (var) {
    case 'P': return config.sim_orig;
    case 'N': return config.cardinality;
    case 'T': return config.tension;
    case 'K': return config.chroma;
    case 'C': return config.common_note;
    case 'a': return config.span;
    case 'A': return config.sspan;
    case 'S': return config.sv;
    case 'Q': return config.q_indicator;
    case 'X': return config.similarity;
    case 'k': return config.chroma_old;
    case 'R': return config.root;
    default:  return config.sim_orig;
  }
}

bool in_range(const ParamTolerance& tol, double value) {
  return value >= tol.min_sub && value <= tol.max_sub;
}

void compute_param_centers(
    SubstitutionConfig& config,
    const OrderedChord& ante,
    const OrderedChord& post) {

  auto ante_stats = calculate_statistics(ante);
  auto post_stats = calculate_statistics(post);

  auto vl = find_voice_leading_substitution(ante, post);

  auto bigram = calculate_bigram_statistics(
      ante, post, ante_stats, post_stats,
      vl.vec, vl.sv, 6, 0.0, {});

  int sim_orig_val = compute_substitution_similarity(
      vl.sv, ante_stats.root == post_stats.root);

  auto set_center = [&](char var, double computed) {
    auto& tol = get_tolerance(config, var);
    if (!contains_char(config.reset_list, var))
      tol.center = computed;
    tol.use_percentage = contains_char(config.percentage_list, var);
    compute_tolerance_range(tol);
  };

  set_center('P', sim_orig_val);
  set_center('N', static_cast<double>(post_stats.num_of_unique_pitch_classes));
  set_center('T', post_stats.tension);
  set_center('K', bigram.chroma);
  set_center('C', bigram.common_note);
  set_center('a', bigram.span);
  set_center('A', bigram.sspan);
  set_center('S', bigram.sv);
  set_center('Q', bigram.Q_indicator);
  set_center('X', bigram.similarity);
  set_center('k', bigram.chroma_old - bigram.prev_chroma_old);
  set_center('R', post_stats.root ? static_cast<double>(static_cast<int>(*post_stats.root)) : 0.0);
}

bool valid_sub(
    SubstitutionConfig& config,
    const OrderedChord& reference,
    const OrderedChord& candidate,
    int sim_orig_val,
    const OrderedChordStatistics& cand_stats,
    SubstituteObj object) {

  const auto& sort_order = config.sort_order;

  if (contains_char(sort_order, 'P') && !in_range(config.sim_orig, sim_orig_val))
    return false;
  if (contains_char(sort_order, 'N') &&
      !in_range(config.cardinality, static_cast<double>(cand_stats.num_of_unique_pitch_classes)))
    return false;
  if (contains_char(sort_order, 'T') && !in_range(config.tension, cand_stats.tension))
    return false;
  if (contains_char(sort_order, 'R') && cand_stats.root &&
      !in_range(config.root, static_cast<double>(static_cast<int>(*cand_stats.root))))
    return false;

  auto ref_stats = calculate_statistics(reference);
  auto vl = find_voice_leading_substitution(reference, candidate);

  auto bigram = calculate_bigram_statistics(
      reference, candidate, ref_stats, cand_stats,
      vl.vec, vl.sv, 6, 0.0, {});

  double chroma_val = bigram.chroma;
  double q_val = bigram.Q_indicator;
  double chroma_old_diff = bigram.chroma_old - bigram.prev_chroma_old;
  if (object == SubstituteObj::Antechord) {
    chroma_val = -chroma_val;
    q_val = -q_val;
  }

  if (contains_char(sort_order, 'K') && !in_range(config.chroma, chroma_val))
    return false;
  if (contains_char(sort_order, 'C') && !in_range(config.common_note, bigram.common_note))
    return false;
  if (contains_char(sort_order, 'a') && !in_range(config.span, bigram.span))
    return false;
  if (contains_char(sort_order, 'A') && !in_range(config.sspan, bigram.sspan))
    return false;
  if (contains_char(sort_order, 'S') && !in_range(config.sv, bigram.sv))
    return false;
  if (contains_char(sort_order, 'Q') && !in_range(config.q_indicator, q_val))
    return false;
  if (contains_char(sort_order, 'X') && !in_range(config.similarity, bigram.similarity))
    return false;
  if (contains_char(sort_order, 'k') && !in_range(config.chroma_old, chroma_old_diff))
    return false;

  if (contains_char(sort_order, 'V') && !config.rm_priority.empty()) {
    int rm = bigram.root_movement;
    if (rm >= 0 && rm < static_cast<int>(config.rm_priority.size()) &&
        config.rm_priority[rm] == -1)
      return false;
  }

  return true;
}

void sort_substitution_entries(
    std::vector<SubstitutionEntry>& entries,
    const std::string& sort_order) {

  std::vector<CandidateEntry> candidates;
  candidates.reserve(entries.size());
  for (auto& e : entries)
    candidates.push_back(CandidateEntry{e.chord, e.stats});

  sort_candidates(candidates, sort_order);

  std::vector<SubstitutionEntry> sorted;
  sorted.reserve(entries.size());
  for (size_t i = 0; i < candidates.size(); ++i) {
    sorted.push_back(SubstitutionEntry{
        std::move(candidates[i].chord),
        std::move(candidates[i].stats),
        candidates[i].stats.sim_orig
    });
  }
  entries = std::move(sorted);
}

} // anonymous namespace

// ── Public API ────────────────────────────────────────────────────

SubstitutionResult substitute(
    const OrderedChord& ante,
    const OrderedChord& post,
    SubstitutionConfig& config,
    SubstitutionProgressCallback progress) {

  SubstitutionResult result;

  auto reduced_ante = reduce_to_octave6(ante);
  auto reduced_post = reduce_to_octave6(post);

  compute_param_centers(config, reduced_ante, reduced_post);

  const int total_ids = (1 << ET_SIZE) - 1;  // 4095

  if (config.object == SubstituteObj::Postchord) {
    auto ante_stats = calculate_statistics(reduced_ante);

    for (int id = 1; id <= total_ids; ++id) {
      auto candidate = id_to_chord(id);

      // Skip the original
      if (candidate.get_pitches().size() == reduced_post.get_pitches().size()) {
        bool same = true;
        auto cp = candidate.get_pitches();
        auto rp = reduced_post.get_pitches();
        for (size_t j = 0; j < cp.size() && same; ++j)
          if (cp[j].get_number() != rp[j].get_number()) same = false;
        if (same) continue;
      }

      auto cand_stats = calculate_statistics(candidate);

      auto vl = find_voice_leading_substitution(reduced_post, candidate);
      bool same_root = (cand_stats.root && ante_stats.root &&
                        *cand_stats.root == *ante_stats.root);
      int sim_orig_val = compute_substitution_similarity(vl.sv, same_root);

      if (valid_sub(config, reduced_ante, candidate, sim_orig_val,
                    cand_stats, config.object)) {
        auto ref_stats = calculate_statistics(reduced_ante);
        auto vl2 = find_voice_leading_substitution(reduced_ante, candidate);
        auto bigram = calculate_bigram_statistics(
            reduced_ante, candidate, ref_stats, cand_stats,
            vl2.vec, vl2.sv, 6, 0.0, {});

        result.entries.push_back(SubstitutionEntry{
            std::move(candidate), std::move(bigram), sim_orig_val});
      }

      result.total_evaluated++;
      if (progress && (id % 100 == 0))
        progress(id, total_ids);
    }

    sort_substitution_entries(result.entries, config.sort_order);

  } else if (config.object == SubstituteObj::Antechord) {
    auto post_stats = calculate_statistics(reduced_post);

    for (int id = 1; id <= total_ids; ++id) {
      auto candidate = id_to_chord(id);

      if (candidate.get_pitches().size() == reduced_ante.get_pitches().size()) {
        bool same = true;
        auto cp = candidate.get_pitches();
        auto rp = reduced_ante.get_pitches();
        for (size_t j = 0; j < cp.size() && same; ++j)
          if (cp[j].get_number() != rp[j].get_number()) same = false;
        if (same) continue;
      }

      auto cand_stats = calculate_statistics(candidate);

      auto vl = find_voice_leading_substitution(reduced_ante, candidate);
      bool same_root = (cand_stats.root && post_stats.root &&
                        *cand_stats.root == *post_stats.root);
      int sim_orig_val = compute_substitution_similarity(vl.sv, same_root);

      if (valid_sub(config, reduced_post, candidate, sim_orig_val,
                    cand_stats, config.object)) {
        auto ref_stats = calculate_statistics(reduced_post);
        auto vl2 = find_voice_leading_substitution(candidate, reduced_post);
        auto bigram = calculate_bigram_statistics(
            candidate, reduced_post, cand_stats, ref_stats,
            vl2.vec, vl2.sv, 6, 0.0, {});

        result.entries.push_back(SubstitutionEntry{
            std::move(candidate), std::move(bigram), sim_orig_val});
      }

      result.total_evaluated++;
      if (progress && (id % 100 == 0))
        progress(id, total_ids);
    }

    sort_substitution_entries(result.entries, config.sort_order);

  } else {
    // BothChords mode
    long long total = config.test_all
        ? static_cast<long long>(total_ids) * total_ids
        : config.sample_size;

    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(1, total_ids);

    for (long long i = 0; i < total; ++i) {
      int ante_id, post_id;
      if (config.test_all) {
        ante_id = static_cast<int>(i / total_ids) + 1;
        post_id = static_cast<int>(i % total_ids) + 1;
      } else {
        ante_id = dist(rng);
        post_id = dist(rng);
      }

      auto ante_cand = id_to_chord(ante_id);
      auto post_cand = id_to_chord(post_id);

      // Skip original pair
      {
        auto ap = ante_cand.get_pitches();
        auto rp_a = reduced_ante.get_pitches();
        auto pp = post_cand.get_pitches();
        auto rp_p = reduced_post.get_pitches();
        if (ap.size() == rp_a.size() && pp.size() == rp_p.size()) {
          bool a_eq = true, p_eq = true;
          for (size_t j = 0; j < ap.size() && a_eq; ++j)
            if (ap[j].get_number() != rp_a[j].get_number()) a_eq = false;
          for (size_t j = 0; j < pp.size() && p_eq; ++j)
            if (pp[j].get_number() != rp_p[j].get_number()) p_eq = false;
          if (a_eq && p_eq) continue;
        }
      }

      auto ante_cand_stats = calculate_statistics(ante_cand);
      auto post_cand_stats = calculate_statistics(post_cand);

      auto vl_post = find_voice_leading_substitution(reduced_post, post_cand);
      auto reduced_post_stats = calculate_statistics(reduced_post);
      bool same_root_post = (post_cand_stats.root && reduced_post_stats.root &&
          *post_cand_stats.root == *reduced_post_stats.root);
      int sim_orig_post = compute_substitution_similarity(vl_post.sv, same_root_post);

      auto vl_ante = find_voice_leading_substitution(reduced_ante, ante_cand);
      auto reduced_ante_stats = calculate_statistics(reduced_ante);
      bool same_root_ante = (ante_cand_stats.root && reduced_ante_stats.root &&
          *ante_cand_stats.root == *reduced_ante_stats.root);
      int sim_orig_ante = compute_substitution_similarity(vl_ante.sv, same_root_ante);

      if (valid_sub(config, ante_cand, post_cand, sim_orig_post,
                    post_cand_stats, SubstituteObj::Postchord)) {
        auto vl2 = find_voice_leading_substitution(ante_cand, post_cand);
        auto bigram_post = calculate_bigram_statistics(
            ante_cand, post_cand, ante_cand_stats, post_cand_stats,
            vl2.vec, vl2.sv, 6, 0.0, {});

        auto vl3 = find_voice_leading_substitution(reduced_ante, ante_cand);
        auto bigram_ante = calculate_bigram_statistics(
            reduced_ante, ante_cand, reduced_ante_stats, ante_cand_stats,
            vl3.vec, vl3.sv, 6, 0.0, {});

        result.pairs.push_back(SubstitutionPair{
            SubstitutionEntry{std::move(ante_cand), std::move(bigram_ante), sim_orig_ante},
            SubstitutionEntry{std::move(post_cand), std::move(bigram_post), sim_orig_post}
        });
      }

      result.total_evaluated++;
      if (progress && (i % 1000 == 0))
        progress(i, total);
    }
  }

  return result;
}

} // namespace chordnovarw::algorithm
