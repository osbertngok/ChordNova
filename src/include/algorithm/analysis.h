#ifndef CHORDNOVARW_SRC_INCLUDE_ALGORITHM_ANALYSIS_H_
#define CHORDNOVARW_SRC_INCLUDE_ALGORITHM_ANALYSIS_H_

#include "model/orderedchord.h"
#include "model/chordstatistics.h"
#include "model/bigramchordstatistics.h"
#include "service/voiceleading.h"

namespace chordnovarw::algorithm {

/**
 * \brief Result of analysing a two-chord progression (ante -> post).
 *
 * Contains single-chord statistics for each chord, the voice-leading
 * result, and the full bigram statistics describing their relationship.
 *
 * Legacy: Output of `Chord::analyse()` in `analyser.cpp`.
 */
struct AnalysisResult {
  model::OrderedChordStatistics ante_stats;
  model::OrderedChordStatistics post_stats;
  service::VoiceLeadingResult vl_result;
  model::BigramChordStatistics bigram_stats;
};

/**
 * \brief Analyse the relationship between two consecutive chords.
 *
 * Computes single-chord statistics for each chord, finds the optimal
 * voice-leading vector, and derives all bigram metrics (chroma, Q, sv,
 * span, similarity, etc.).
 *
 * This is a pure function with no side effects.
 *
 * Legacy: `Chord::analyse()` in `analyser.cpp`.
 *
 * \param ante The antecedent (previous) chord.
 * \param post The consequent (next) chord.
 * \param prev_chroma_old The chroma_old from the chord before `ante` (0.0 if none).
 * \param prev_single_chroma The single_chroma vector from the chord before `ante` (empty if none).
 * \return AnalysisResult with all computed statistics.
 */
[[nodiscard]] AnalysisResult analyse(
    const model::OrderedChord& ante,
    const model::OrderedChord& post,
    double prev_chroma_old = 0.0,
    const std::vector<int>& prev_single_chroma = {});

} // namespace chordnovarw::algorithm

#endif // CHORDNOVARW_SRC_INCLUDE_ALGORITHM_ANALYSIS_H_
