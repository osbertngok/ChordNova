#ifndef CHORDNOVARW_SRC_INCLUDE_SERVICE_VOICELEADING_H_
#define CHORDNOVARW_SRC_INCLUDE_SERVICE_VOICELEADING_H_

#include "model/orderedchord.h"
#include <vector>

namespace chordnovarw::service {

/**
 * \brief Result of a voice-leading vector computation.
 *
 * Legacy: `vec` and `sv` fields in `ChordData`.
 */
struct VoiceLeadingResult {
  /**
   * \brief Signed semitone movement per voice, from old chord to new chord.
   *
   * Positive = ascending, negative = descending.
   * Length = max(old_size, new_size) after expansion.
   */
  std::vector<int> vec;

  /**
   * \brief Sum of absolute voice-leading distances (Σ|vec[i]|).
   */
  int sv = 0;
};

/**
 * \brief Finds the optimal voice-leading vector between two chords.
 *
 * When the two chords have different sizes, the smaller one is expanded
 * (using the expansion index cache) to match, trying all possible expansions
 * and selecting the one that minimizes total voice-leading distance (sv).
 *
 * This is a pure function with no side effects.
 *
 * Legacy: `Chord::_find_vec()` in `chord.cpp`.
 *
 * \param from  The previous (antecedent) chord.
 * \param to    The current (consequent) chord.
 * \return VoiceLeadingResult with the optimal vec and minimum sv.
 */
[[nodiscard]] VoiceLeadingResult find_voice_leading(
    const model::OrderedChord& from,
    const model::OrderedChord& to);

/**
 * \brief Finds voice-leading with substitution mode (octave-inversion search).
 *
 * Tries all inversions (octave rotations) of the target chord and picks the
 * one with the smallest sv, provided all individual voice movements are <= 6 semitones.
 *
 * Legacy: The `in_substitution` branch of `Chord::find_vec()` in `chord.cpp`.
 *
 * \param from  The previous chord.
 * \param to    The current chord.
 * \return VoiceLeadingResult with the optimal vec and sv across all inversions.
 */
[[nodiscard]] VoiceLeadingResult find_voice_leading_substitution(
    const model::OrderedChord& from,
    const model::OrderedChord& to);

} // namespace chordnovarw::service

#endif // CHORDNOVARW_SRC_INCLUDE_SERVICE_VOICELEADING_H_
