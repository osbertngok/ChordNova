#ifndef CHORDNOVARW_SRC_INCLUDE_MODEL_CONFIG_H_
#define CHORDNOVARW_SRC_INCLUDE_MODEL_CONFIG_H_

#include "config_enums.h"
#include <string>
#include <vector>

namespace chordnovarw::model {

/**
 * \brief Interval constraint data for exclusion rules.
 *
 * Legacy: `intervalData` in `chord.h`.
 */
struct IntervalConstraint {
  int interval;    ///< The interval in semitones (0–11).
  int octave_min;  ///< Minimum octave distance for this interval.
  int octave_max;  ///< Maximum octave distance for this interval.
  int num_min;     ///< Minimum count of this interval to trigger exclusion.
  int num_max;     ///< Maximum count of this interval to trigger exclusion.
};

/**
 * \brief Voice-leading movement constraints.
 *
 * Legacy: `vl_min`, `vl_max`, `vl_setting`, `steady_*`, `ascending_*`, `descending_*` in `Chord`.
 */
struct VoiceLeadingConstraints {
  int vl_min = 0;             ///< Minimum absolute voice movement (dead zone below this).
  int vl_max = 4;             ///< Maximum absolute voice movement per voice.
  VLSetting vl_setting = VLSetting::Default;  ///< How to validate direction counts.
  double steady_min = 0.0;    ///< Min steady voice count (or percentage if Percentage mode).
  double steady_max = 100.0;  ///< Max steady voice count.
  double ascending_min = 0.0; ///< Min ascending voice count.
  double ascending_max = 100.0; ///< Max ascending voice count.
  double descending_min = 0.0;  ///< Min descending voice count.
  double descending_max = 100.0; ///< Max descending voice count.
};

/**
 * \brief Pitch range constraints.
 *
 * Legacy: `lowest`, `highest`, `m_min`, `m_max`, `n_min`, `n_max`,
 *         `h_min`, `h_max`, `r_min`, `r_max`, `g_min`, `g_max` in `Chord`.
 */
struct RangeConstraints {
  int lowest = 0;     ///< Lowest allowed MIDI note number.
  int highest = 127;  ///< Highest allowed MIDI note number.
  int m_min = 1;      ///< Minimum number of notes (voices).
  int m_max = 15;     ///< Maximum number of notes (voices).
  int n_min = 1;      ///< Minimum number of unique pitch classes.
  int n_max = 12;     ///< Maximum number of unique pitch classes.
  double h_min = 0.0; ///< Minimum thickness.
  double h_max = 50.0; ///< Maximum thickness.
  int r_min = 0;      ///< Minimum root pitch class (0–11).
  int r_max = 11;     ///< Maximum root pitch class (0–11).
  int g_min = 0;      ///< Minimum geometrical center (0–100 in legacy).
  int g_max = 100;    ///< Maximum geometrical center.
};

/**
 * \brief Harmonic/bigram statistic range constraints.
 *
 * Legacy: `k_min`, `k_max`, `kk_min`, `kk_max`, `t_min`, `t_max`,
 *         `c_min`, `c_max`, `sv_min`, `sv_max`, `s_min`, `s_max`,
 *         `ss_min`, `ss_max`, `q_min`, `q_max`, `x_min`, `x_max` in `Chord`.
 */
struct HarmonicConstraints {
  double k_min = 0.0;    ///< Min chroma (harmonic distance percentile).
  double k_max = 100.0;  ///< Max chroma percentile.
  double kk_min = 0.0;   ///< Min chroma_old percentile.
  double kk_max = 100.0; ///< Max chroma_old percentile.
  double t_min = 0.0;    ///< Min tension percentile.
  double t_max = 100.0;  ///< Max tension percentile.
  int c_min = 0;         ///< Min common notes.
  int c_max = 15;        ///< Max common notes.
  int sv_min = 0;        ///< Min sum of voice-leading distances.
  int sv_max = 100;      ///< Max sum of voice-leading distances.
  int s_min = 0;         ///< Min Circle of Fifths span.
  int s_max = 12;        ///< Max span.
  int ss_min = 0;        ///< Min super-span.
  int ss_max = 12;       ///< Max super-span.
  double q_min = -500.0; ///< Min Q indicator.
  double q_max = 500.0;  ///< Max Q indicator.
  int x_min = 0;         ///< Min similarity (0–100).
  int x_max = 100;       ///< Max similarity.
};

/**
 * \brief Alignment/interval spacing constraints.
 *
 * Legacy: `align_mode`, `i_min`, `i_max`, `i_high`, `i_low`,
 *         `alignment_list` in `Chord`.
 */
struct AlignmentConfig {
  AlignMode align_mode = AlignMode::Unlimited; ///< Alignment validation mode.
  int i_min = 0;    ///< Min interval between middle voices (Interval mode).
  int i_max = 24;   ///< Max interval between middle voices.
  int i_low = 0;    ///< Min interval for lowest pair.
  int i_high = 24;  ///< Max interval for highest pair.
  std::vector<std::vector<int>> alignment_list; ///< Whitelist (List mode).
};

/**
 * \brief Note/root/interval exclusion rules.
 *
 * Legacy: `exclusion_notes`, `exclusion_roots`, `exclusion_intervals`,
 *         `enable_ex` in `Chord`.
 */
struct ExclusionConfig {
  bool enabled = false;  ///< Whether exclusion checking is active.
  std::vector<int> exclusion_notes;    ///< MIDI note numbers to exclude.
  std::vector<int> exclusion_roots;    ///< Root pitch classes to exclude.
  std::vector<IntervalConstraint> exclusion_intervals; ///< Interval patterns to exclude.
};

/**
 * \brief Pedal note constraints.
 *
 * Legacy: `enable_pedal`, `pedal_notes`, `pedal_notes_set`,
 *         `in_bass`, `realign`, `period`, `connect_pedal` in `Chord`.
 */
struct PedalConfig {
  bool enabled = false;        ///< Whether pedal note enforcement is active.
  std::vector<int> pedal_notes;     ///< MIDI note numbers of pedal notes.
  std::vector<int> pedal_notes_set; ///< Pitch classes of pedal notes.
  bool in_bass = false;        ///< Pedal notes must be lowest notes.
  bool realign = false;        ///< Force pedal octave change at period boundary.
  int period = 1;              ///< Pedal re-alignment period (in chords).
  bool connect_pedal = false;  ///< Connect pedal notes across progressions.
};

/**
 * \brief Deduplication settings.
 *
 * Legacy: `unique_mode` in `Chord`.
 */
struct UniquenessConfig {
  UniqueMode unique_mode = UniqueMode::Disabled; ///< Deduplication mode.
};

/**
 * \brief Overall scale constraint (pitch class filter).
 *
 * Legacy: `overall_scale` in `chord.cpp`.
 */
struct ScaleConfig {
  std::vector<int> overall_scale = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}; ///< Allowed pitch classes.
};

/**
 * \brief Extended similarity checking configuration.
 *
 * Legacy: `enable_sim`, `sim_period`, `sim_min`, `sim_max` in `Chord`.
 */
struct SimilarityConfig {
  bool enabled = false;             ///< Whether extended similarity checks are active.
  std::vector<int> sim_period;      ///< Lookback periods for similarity checking.
  std::vector<int> sim_min;         ///< Min similarity for each period.
  std::vector<int> sim_max;         ///< Max similarity for each period.
};

/**
 * \brief Root movement constraint.
 *
 * Legacy: `enable_rm`, `rm_priority` in `Chord`.
 */
struct RootMovementConfig {
  bool enabled = false;             ///< Whether root movement filtering is active.
  std::vector<int> rm_priority;     ///< Priority for each root movement (0–6). -1 = disabled.
};

/**
 * \brief Bass note availability constraints.
 *
 * Legacy: `bass_avail` in `Chord`.
 */
struct BassConfig {
  std::vector<int> bass_avail = {1, 3, 5, 7, 9, 11, 13}; ///< Allowed bass scale degrees.
};

/**
 * \brief Chord library (database) configuration.
 *
 * Legacy: `chord_library` in `functions.cpp`, `database_filename` in `Chord`.
 */
struct ChordLibraryConfig {
  std::vector<int> chord_library; ///< Set IDs of all allowed chord types (all transpositions).
};

/**
 * \brief Sort order configuration.
 *
 * Legacy: `sort_order` in `Chord`.
 */
struct SortConfig {
  std::string sort_order; ///< Sort string read right-to-left; '+' suffix = ascending.
};

/**
 * \brief Aggregated progression generation configuration.
 *
 * Collects all constraint and configuration structs needed to run
 * the progression generator. All sub-structs have sensible defaults.
 *
 * Legacy: The ~80 public fields of `Chord`.
 */
struct ProgressionConfig {
  VoiceLeadingConstraints voice_leading;
  RangeConstraints range;
  HarmonicConstraints harmonic;
  AlignmentConfig alignment;
  ExclusionConfig exclusion;
  PedalConfig pedal;
  UniquenessConfig uniqueness;
  ScaleConfig scale;
  SimilarityConfig similarity;
  RootMovementConfig root_movement;
  BassConfig bass;
  ChordLibraryConfig chord_library;
  SortConfig sort;
  bool continual = false;     ///< True for multi-step continual mode.
  int loop_count = 1;         ///< Number of progressions to generate in continual mode.
  OutputMode output_mode = OutputMode::Both; ///< Output format.
};

} // namespace chordnovarw::model

#endif // CHORDNOVARW_SRC_INCLUDE_MODEL_CONFIG_H_
