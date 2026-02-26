#include "service/voiceleading.h"
#include "service/expansion.h"
#include "utility/combinatorics.h"
#include "model/pitch.h"
#include "constant.h"
#include <cmath>
#include <climits>

namespace chordnovarw::service {

using namespace chordnovarw::model;
using namespace chordnovarw::utility;

VoiceLeadingResult find_voice_leading(
    const model::OrderedChord& from,
    const model::OrderedChord& to) {

  auto from_pitches = from.get_pitches();
  auto to_pitches = to.get_pitches();
  const int from_size = static_cast<int>(from_pitches.size());
  const int to_size = static_cast<int>(to_pitches.size());

  VoiceLeadingResult result;

  if (to_size > from_size) {
    // New chord is larger: expand 'from' to match, try all expansions
    const auto& cache = ExpansionIndexCache::instance();
    const int len = cache.count(from_size, to_size);
    int min_diff = INT_MAX;
    int min_index = 0;

    for (int i = 0; i < len; ++i) {
      auto expansion = expand_single(from, to_size, i);
      auto exp_pitches = expansion.get_pitches();
      int diff = 0;
      for (int j = 0; j < to_size; ++j)
        diff += std::abs(to_pitches[j].get_number() - exp_pitches[j].get_number());
      if (diff < min_diff) {
        min_diff = diff;
        min_index = i;
      }
    }

    auto best_expansion = expand_single(from, to_size, min_index);
    auto best_pitches = best_expansion.get_pitches();
    result.vec.reserve(to_size);
    for (int i = 0; i < to_size; ++i)
      result.vec.push_back(to_pitches[i].get_number() - best_pitches[i].get_number());
    result.sv = min_diff;

  } else {
    // 'from' is larger or equal: expand 'to' to match
    const auto& cache = ExpansionIndexCache::instance();
    const int len = cache.count(to_size, from_size);
    int min_diff = INT_MAX;
    int min_index = 0;

    for (int i = 0; i < len; ++i) {
      auto expansion = expand_single(to, from_size, i);
      auto exp_pitches = expansion.get_pitches();
      int diff = 0;
      for (int j = 0; j < from_size; ++j)
        diff += std::abs(exp_pitches[j].get_number() - from_pitches[j].get_number());
      if (diff < min_diff) {
        min_diff = diff;
        min_index = i;
      }
    }

    auto best_expansion = expand_single(to, from_size, min_index);
    auto best_pitches = best_expansion.get_pitches();
    result.vec.reserve(from_size);
    for (int i = 0; i < from_size; ++i)
      result.vec.push_back(best_pitches[i].get_number() - from_pitches[i].get_number());
    result.sv = min_diff;
  }

  return result;
}

VoiceLeadingResult find_voice_leading_substitution(
    const model::OrderedChord& from,
    const model::OrderedChord& to) {

  auto to_pitches = to.get_pitches();
  const int size = static_cast<int>(to_pitches.size());

  int min_sv = INT_MAX;
  std::vector<int> min_vec;

  // Try all inversions: i in [0, 2*size]
  // i=0 flips the whole chord down an octave; i=2*size flips up an octave
  for (int i = 0; i <= 2 * size; ++i) {
    std::vector<Pitch> inv_pitches;
    bool out_of_range = false;
    for (int j = 0; j < size; ++j) {
      int src_idx = (j + i) % size;
      int octave_shift = ((j + i) / size - 1) * chordnovarw::ET_SIZE;
      int midi = to_pitches[src_idx].get_number() + octave_shift;
      if (midi < 0 || midi > 127) {
        out_of_range = true;
        break;
      }
      inv_pitches.push_back(Pitch(static_cast<char>(midi)));
    }
    if (out_of_range) continue;

    OrderedChord inversion(std::move(inv_pitches));
    auto vl = find_voice_leading(from, inversion);

    // Check all movements are within 6 semitones
    bool valid = true;
    for (int v : vl.vec) {
      if (std::abs(v) > 6) {
        valid = false;
        break;
      }
    }
    if (valid && vl.sv < min_sv) {
      min_sv = vl.sv;
      min_vec = vl.vec;
    }
  }

  return VoiceLeadingResult{min_vec, min_sv};
}

} // namespace chordnovarw::service
