#ifndef CHORDNOVARW_SRC_INCLUDE_MODEL_SUBSTITUTION_CONFIG_H_
#define CHORDNOVARW_SRC_INCLUDE_MODEL_SUBSTITUTION_CONFIG_H_

#include "config_enums.h"
#include <string>

namespace chordnovarw::model {

/**
 * \brief Per-parameter substitution tolerance: center value, radius, and computed min/max.
 *
 * For each enabled parameter, the filter checks `min_sub <= value <= max_sub`.
 * The range is derived from the center (reference) value and radius.
 * If `use_percentage` is true, min/max = center * (1 ± radius/100);
 * otherwise min/max = center ± radius.
 *
 * Legacy: `p_reset_value`, `p_radius`, `p_min_sub`, `p_max_sub` etc. in `Chord`.
 */
struct ParamTolerance {
  double center = 0.0;      ///< Reference value (computed or reset).
  double radius = 0.0;      ///< Tolerance radius (absolute or percentage).
  bool use_percentage = false; ///< Whether radius is percentage-based.
  double min_sub = 0.0;     ///< Computed lower bound.
  double max_sub = 0.0;     ///< Computed upper bound.
};

/**
 * \brief Configuration for chord substitution search.
 *
 * Controls which chord(s) to substitute, parameter tolerances,
 * sort ordering, and BothChords sampling behavior.
 *
 * Legacy: Substitution-related fields on `Chord`.
 */
struct SubstitutionConfig {
  SubstituteObj object = SubstituteObj::Postchord; ///< Which chord(s) to substitute.
  bool test_all = false;      ///< BothChords: enumerate all 4095^2 pairs.
  int sample_size = 100000;   ///< BothChords random sample size.

  std::string sort_order;     ///< Sort/filter keys (right-to-left priority).
  std::string reset_list;     ///< Parameters using fixed reset values (not computed from reference).
  std::string percentage_list; ///< Parameters using percentage-based radius.

  // Per-parameter tolerances. Letter code in parentheses.
  ParamTolerance sim_orig;      ///< P — chord-to-original similarity.
  ParamTolerance cardinality;   ///< N — pitch-class cardinality.
  ParamTolerance tension;       ///< T — tension.
  ParamTolerance chroma;        ///< K — Hua chromaticity.
  ParamTolerance common_note;   ///< C — common notes.
  ParamTolerance span;          ///< a — Circle of Fifths span.
  ParamTolerance sspan;         ///< A — super-span.
  ParamTolerance sv;            ///< S — voice-leading distance.
  ParamTolerance q_indicator;   ///< Q — Q indicator.
  ParamTolerance similarity;    ///< X — lateral similarity.
  ParamTolerance chroma_old;    ///< k — gross chromaticity difference.
  ParamTolerance root;          ///< R — root pitch class.

  std::vector<int> rm_priority; ///< Root movement priority (index 0-6). -1 = disabled.
};

} // namespace chordnovarw::model

#endif // CHORDNOVARW_SRC_INCLUDE_MODEL_SUBSTITUTION_CONFIG_H_
