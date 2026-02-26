#ifndef CHORDNOVARW_SRC_INCLUDE_MODEL_CONFIG_ENUMS_H_
#define CHORDNOVARW_SRC_INCLUDE_MODEL_CONFIG_ENUMS_H_

namespace chordnovarw::model {

/**
 * \brief Output format mode.
 *
 * Legacy: `OutputMode` in `chord.h`.
 */
enum class OutputMode {
  Both,      ///< Output both text and MIDI files.
  MidiOnly,  ///< Output MIDI file only.
  TextOnly   ///< Output text file only.
};

/**
 * \brief Deduplication mode for generated chords.
 *
 * Legacy: `UniqueMode` in `chord.h`.
 */
enum class UniqueMode {
  Disabled,       ///< No deduplication.
  RemoveDup,      ///< Remove exact pitch duplicates.
  RemoveDupType   ///< Remove chords with identical pitch-class set type.
};

/**
 * \brief Voice alignment validation mode.
 *
 * Legacy: `AlignMode` in `chord.h`.
 */
enum class AlignMode {
  Interval,   ///< Validate by interval range constraints.
  List,       ///< Validate against a whitelist of allowed alignments.
  Unlimited   ///< No alignment constraints.
};

/**
 * \brief Voice-leading direction constraint mode.
 *
 * Legacy: `VLSetting` in `chord.h`.
 */
enum class VLSetting {
  Percentage,  ///< Direction counts as percentage of total voices.
  Number,      ///< Direction counts as absolute numbers.
  Default      ///< Default: reject parallel motion only.
};

/**
 * \brief Which chord(s) to substitute in chord substitution mode.
 *
 * Legacy: `SubstituteObj` in `chord.h`.
 */
enum class SubstituteObj {
  Postchord,    ///< Substitute the post-chord only.
  Antechord,    ///< Substitute the ante-chord only.
  BothChords    ///< Substitute both chords.
};

} // namespace chordnovarw::model

#endif // CHORDNOVARW_SRC_INCLUDE_MODEL_CONFIG_ENUMS_H_
