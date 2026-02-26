#ifndef CHORDNOVARW_SRC_INCLUDE_ALGORITHM_SORTING_H_
#define CHORDNOVARW_SRC_INCLUDE_ALGORITHM_SORTING_H_

#include "model/bigramchordstatistics.h"
#include <string>
#include <vector>

namespace chordnovarw::algorithm {

/**
 * \brief Result entry: a candidate chord with its bigram statistics.
 */
struct CandidateEntry {
  model::OrderedChord chord;
  model::BigramChordStatistics stats;
};

/**
 * \brief Sort candidate entries according to a sort_order string.
 *
 * The sort_order string is read right-to-left. Each character selects a
 * sort key; a '+' suffix means ascending order (default is descending).
 *
 * Key codes (matching legacy ChordNova):
 *   P = sim_orig, N = pitch class count, T = tension, K = chroma,
 *   C = common_note, a = span, A = sspan, m = note count,
 *   h = thickness, g = geometrical center, S = sv,
 *   Q = Q_indicator, X = similarity, k = chroma_old,
 *   R = root, V = root_movement
 *
 * Legacy: `Chord::sort_results()` in `chord.cpp`.
 */
void sort_candidates(std::vector<CandidateEntry>& candidates,
                     const std::string& sort_order);

} // namespace chordnovarw::algorithm

#endif // CHORDNOVARW_SRC_INCLUDE_ALGORITHM_SORTING_H_
