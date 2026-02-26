#ifndef CHORDNOVARW_SRC_INCLUDE_ALGORITHM_SUBSTITUTION_H_
#define CHORDNOVARW_SRC_INCLUDE_ALGORITHM_SUBSTITUTION_H_

#include "model/orderedchord.h"
#include "model/bigramchordstatistics.h"
#include "model/substitution_config.h"
#include <functional>
#include <string>
#include <vector>

namespace chordnovarw::algorithm {

/**
 * \brief A single substitution result entry.
 */
struct SubstitutionEntry {
  model::OrderedChord chord;
  model::BigramChordStatistics stats;
  int sim_orig = 100;  ///< Similarity to the original chord (0-100).
};

/**
 * \brief Paired substitution entry for BothChords mode.
 */
struct SubstitutionPair {
  SubstitutionEntry ante;
  SubstitutionEntry post;
};

/**
 * \brief Result of a substitution search.
 */
struct SubstitutionResult {
  /// Postchord/Antechord mode: single-chord substitutes.
  std::vector<SubstitutionEntry> entries;
  /// BothChords mode: paired substitutes.
  std::vector<SubstitutionPair> pairs;
  int total_evaluated = 0;  ///< Total candidates evaluated.
};

/**
 * \brief Progress callback: (current, total).
 */
using SubstitutionProgressCallback = std::function<void(long long, long long)>;

/**
 * \brief Convert a 12-bit pitch-class bitmask to MIDI notes at octave 6 (base 72).
 *
 * Bit k set -> MIDI note 72+k is present.
 *
 * Legacy: `id_to_notes()` in `functions.cpp`.
 *
 * \param id Bitmask (1-4095).
 * \return OrderedChord with pitches at MIDI 72-83.
 */
[[nodiscard]] model::OrderedChord id_to_chord(int id);

/**
 * \brief Reduce a chord to pitch classes mapped to MIDI 72-83 range.
 *
 * Each pitch is reduced mod 12, then offset to 72. Duplicates removed,
 * sorted ascending.
 *
 * Legacy: pitch-class reduction in `set_param_center()`.
 *
 * \param chord The chord to reduce.
 * \return Reduced chord at octave 6.
 */
[[nodiscard]] model::OrderedChord reduce_to_octave6(const model::OrderedChord& chord);

/**
 * \brief Compute substitution similarity between original and candidate.
 *
 * Legacy: `set_similarity()` with `in_substitution=true`.
 *
 * \param sv Total voice-leading distance.
 * \param same_root Whether the two chords share the same root.
 * \return Similarity percentage (0-100).
 */
[[nodiscard]] int compute_substitution_similarity(int sv, bool same_root);

/**
 * \brief Compute the parameter tolerance min/max from center and radius.
 *
 * Legacy: `set_param_range()` in `analyser.cpp`.
 */
void compute_tolerance_range(model::ParamTolerance& tol);

/**
 * \brief Perform a chord substitution search.
 *
 * Iterates all 4095 non-empty pitch-class subsets (or 4095^2 pairs in
 * BothChords mode), validates each against the tolerance windows in
 * config, and returns ranked results.
 *
 * Legacy: `Chord::substitute()` in `analyser.cpp`.
 *
 * \param ante The antecedent chord.
 * \param post The consequent chord.
 * \param config Substitution configuration with tolerance ranges.
 * \param progress Optional progress callback.
 * \return SubstitutionResult with sorted, filtered candidates.
 */
[[nodiscard]] SubstitutionResult substitute(
    const model::OrderedChord& ante,
    const model::OrderedChord& post,
    model::SubstitutionConfig& config,
    SubstitutionProgressCallback progress = nullptr);

} // namespace chordnovarw::algorithm

#endif // CHORDNOVARW_SRC_INCLUDE_ALGORITHM_SUBSTITUTION_H_
