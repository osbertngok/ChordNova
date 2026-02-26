#ifndef CHORDNOVARW_SRC_INCLUDE_ALGORITHM_PROGRESSION_H_
#define CHORDNOVARW_SRC_INCLUDE_ALGORITHM_PROGRESSION_H_

#include "model/orderedchord.h"
#include "model/chordstatistics.h"
#include "model/bigramchordstatistics.h"
#include "model/config.h"
#include "algorithm/sorting.h"
#include <functional>
#include <vector>

namespace chordnovarw::algorithm {

/**
 * \brief Result of a single-step progression generation.
 */
struct ProgressionResult {
  std::vector<CandidateEntry> candidates;
  int total_evaluated = 0;  ///< Total mutations evaluated.
};

/**
 * \brief Progress callback: (current_count, total_count).
 */
using ProgressCallback = std::function<void(long long, long long)>;

/**
 * \brief Generates all valid next chords from an initial chord.
 *
 * This is the core generation engine. It:
 * 1. Expands the initial chord to m_max voices (all C(m_max-1, m-1) expansions)
 * 2. For each expansion, iterates all mutation vectors in [-vl_max, vl_max]
 * 3. Validates each candidate through the validation pipeline
 * 4. Collects all passing candidates with their bigram statistics
 *
 * Legacy: `Chord::get_progression()` + `Chord::set_new_chords()`.
 *
 * \param initial_chord  The starting chord.
 * \param config         Progression configuration (constraints, sorting).
 * \param prev_single_chroma Circle of Fifths positions from previous step (empty for first).
 * \param prev_chroma_old    Previous chroma_old value (0.0 for first).
 * \param record         All chords generated so far (for similarity lookback).
 * \param progress       Optional progress callback.
 * \return ProgressionResult with all valid candidates.
 */
[[nodiscard]] ProgressionResult generate_single(
    const model::OrderedChord& initial_chord,
    const model::ProgressionConfig& config,
    const std::vector<int>& prev_single_chroma = {},
    double prev_chroma_old = 0.0,
    const std::vector<model::OrderedChord>& record = {},
    ProgressCallback progress = nullptr);

} // namespace chordnovarw::algorithm

#endif // CHORDNOVARW_SRC_INCLUDE_ALGORITHM_PROGRESSION_H_
