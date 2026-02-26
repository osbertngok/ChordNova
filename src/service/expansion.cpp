#include "service/expansion.h"
#include "utility/combinatorics.h"
#include "model/pitch.h"
#include <stdexcept>

namespace chordnovarw::service {

using namespace chordnovarw::model;
using namespace chordnovarw::utility;

model::OrderedChord expand_single(
    const model::OrderedChord& chord, int target_size, int index) {
  auto pitches = chord.get_pitches();
  const int src_size = static_cast<int>(pitches.size());

  if (target_size < src_size) {
    throw std::invalid_argument("expand_single: target_size < chord size");
  }
  if (target_size == src_size) {
    return chord;
  }

  const auto& cache = ExpansionIndexCache::instance();
  auto mapping = cache.get(src_size, target_size, index);

  std::vector<Pitch> result_pitches;
  result_pitches.reserve(target_size);
  for (int i = 0; i < target_size; ++i) {
    result_pitches.push_back(pitches[mapping[i]]);
  }
  return OrderedChord(std::move(result_pitches));
}

std::vector<model::OrderedChord> expand(
    const model::OrderedChord& chord, int target_size) {
  auto pitches = chord.get_pitches();
  const int src_size = static_cast<int>(pitches.size());

  if (target_size < src_size) {
    throw std::invalid_argument("expand: target_size < chord size");
  }

  const auto& cache = ExpansionIndexCache::instance();
  const int count = cache.count(src_size, target_size);

  std::vector<model::OrderedChord> result;
  result.reserve(count);
  for (int i = 0; i < count; ++i) {
    result.push_back(expand_single(chord, target_size, i));
  }
  return result;
}

} // namespace chordnovarw::service
