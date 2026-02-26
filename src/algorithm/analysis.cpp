#include "algorithm/analysis.h"
#include <cmath>

namespace chordnovarw::algorithm {

using namespace chordnovarw::model;
using namespace chordnovarw::service;

AnalysisResult analyse(
    const OrderedChord& ante,
    const OrderedChord& post,
    double prev_chroma_old,
    const std::vector<int>& prev_single_chroma) {

  auto ante_stats = calculate_statistics(ante);
  auto post_stats = calculate_statistics(post);

  auto vl = find_voice_leading(ante, post);

  // In analyser mode, vl_max is derived from the actual max abs vec component
  int vl_max = 0;
  for (int v : vl.vec) {
    int abs_v = std::abs(v);
    if (abs_v > vl_max) vl_max = abs_v;
  }
  if (vl_max == 0) vl_max = 1;  // avoid division by zero

  auto bigram = calculate_bigram_statistics(
      ante, post, ante_stats, post_stats,
      vl.vec, vl.sv, vl_max,
      prev_chroma_old, prev_single_chroma);

  return AnalysisResult{
      std::move(ante_stats),
      std::move(post_stats),
      std::move(vl),
      std::move(bigram)
  };
}

} // namespace chordnovarw::algorithm
