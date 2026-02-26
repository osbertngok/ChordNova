#ifndef CHORDNOVARW_SRC_INCLUDE_SERVICE_EXPANSION_H_
#define CHORDNOVARW_SRC_INCLUDE_SERVICE_EXPANSION_H_

#include "model/orderedchord.h"
#include <vector>

namespace chordnovarw::service {

/**
 * \brief Expands an OrderedChord to a target voice count by duplicating existing notes.
 *
 * Given a chord with N distinct notes and a target size M >= N, returns all
 * C(M-1, N-1) ways to expand the chord by doubling its notes. Each expansion
 * preserves the original pitch classes and is sorted in ascending pitch order.
 *
 * Legacy: `Chord::expand()` in `chord.cpp`.
 *
 * \param chord       The source chord to expand.
 * \param target_size The desired number of voices (must be >= chord size).
 * \return Vector of all possible expansions, each an OrderedChord of size target_size.
 */
[[nodiscard]] std::vector<model::OrderedChord> expand(
    const model::OrderedChord& chord, int target_size);

/**
 * \brief Returns a single expansion of a chord using a specific combination index.
 *
 * Legacy: `Chord::expand(expansion, target_size, index)` in `chord.cpp`.
 *
 * \param chord       The source chord to expand.
 * \param target_size The desired number of voices.
 * \param index       Which combination to use (0-based).
 * \return The expanded OrderedChord.
 */
[[nodiscard]] model::OrderedChord expand_single(
    const model::OrderedChord& chord, int target_size, int index);

} // namespace chordnovarw::service

#endif // CHORDNOVARW_SRC_INCLUDE_SERVICE_EXPANSION_H_
