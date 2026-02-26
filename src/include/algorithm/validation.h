#ifndef CHORDNOVARW_SRC_INCLUDE_ALGORITHM_VALIDATION_H_
#define CHORDNOVARW_SRC_INCLUDE_ALGORITHM_VALIDATION_H_

#include "model/orderedchord.h"
#include "model/chordstatistics.h"
#include "model/bigramchordstatistics.h"
#include "model/config.h"
#include "service/voiceleading.h"
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace chordnovarw::algorithm {

/**
 * \brief Context passed to each validator in the pipeline.
 *
 * Carries the previous chord's statistics, the generated chord set
 * (for uniqueness checking), configuration, and voice-leading results.
 */
struct ValidationContext {
  const model::ProgressionConfig& config;
  const model::OrderedChord& prev_chord;
  const model::OrderedChordStatistics& prev_stats;

  /// Voice-leading result (computed during validation)
  service::VoiceLeadingResult vl_result;

  /// Computed statistics for the candidate chord (lazily populated)
  std::optional<model::OrderedChordStatistics> candidate_stats;

  /// Set IDs for uniqueness checking (RemoveDupType mode)
  std::vector<int>& rec_ids;

  /// Vec IDs for movement vector uniqueness
  std::vector<long long>& vec_ids;

  /// Previous single_chroma values (for span/chroma computations)
  const std::vector<int>& prev_single_chroma;

  /// Previous chroma_old value
  double prev_chroma_old;

  /// Record of all chords so far (for similarity lookback)
  const std::vector<model::OrderedChord>& record;
};

/**
 * \brief A single validation function.
 *
 * Returns true if the candidate passes this validation stage.
 * May modify the context (e.g., computing voice-leading lazily).
 */
using Validator = std::function<bool(ValidationContext&, model::OrderedChord&)>;

// ── Individual Validators ───────────────────────────────────────────

/**
 * \brief Stage 1: Notes must be in ascending (non-strict) order.
 *
 * Legacy: First check in `Chord::valid()`.
 */
[[nodiscard]] bool validate_monotonicity(
    ValidationContext& ctx, model::OrderedChord& chord);

/**
 * \brief Stage 2: Notes must be within [lowest, highest] range.
 *
 * Legacy: Range check in `Chord::valid()`.
 */
[[nodiscard]] bool validate_range(
    ValidationContext& ctx, model::OrderedChord& chord);

/**
 * \brief Stage 3: Voice alignment validation (Interval or List mode).
 *
 * Legacy: `Chord::valid_alignment()`.
 */
[[nodiscard]] bool validate_alignment(
    ValidationContext& ctx, model::OrderedChord& chord);

/**
 * \brief Stage 4: Exclusion of forbidden notes/roots/intervals.
 *
 * Legacy: `Chord::valid_exclusion()`.
 */
[[nodiscard]] bool validate_exclusion(
    ValidationContext& ctx, model::OrderedChord& chord);

/**
 * \brief Stage 5: Pedal note inclusion check.
 *
 * Legacy: `Chord::include_pedal()`.
 */
[[nodiscard]] bool validate_pedal(
    ValidationContext& ctx, model::OrderedChord& chord);

/**
 * \brief Stage 6: Note count (m) and pitch class count (n) constraints.
 *
 * Legacy: m_min/m_max and n_min/n_max checks in `Chord::valid()`.
 */
[[nodiscard]] bool validate_cardinality(
    ValidationContext& ctx, model::OrderedChord& chord);

/**
 * \brief Stage 7: Single-chord statistics constraints (thickness, root, g_center).
 *
 * Legacy: h_min/max, r_min/max, g_min/max checks in `Chord::valid()`.
 */
[[nodiscard]] bool validate_single_chord_stats(
    ValidationContext& ctx, model::OrderedChord& chord);

/**
 * \brief Stage 8: All pitch classes must be in the overall scale.
 *
 * Legacy: Scale intersection check in `Chord::valid()`.
 */
[[nodiscard]] bool validate_scale_membership(
    ValidationContext& ctx, model::OrderedChord& chord);

/**
 * \brief Stage 9: Bass note must be in bass_avail, chord must be in library.
 *
 * Legacy: bass_avail and chord_library checks in `Chord::valid()`.
 */
[[nodiscard]] bool validate_bass_and_library(
    ValidationContext& ctx, model::OrderedChord& chord);

/**
 * \brief Stage 10: RemoveDupType uniqueness check.
 *
 * Legacy: rec_id uniqueness check in `Chord::valid()`.
 */
[[nodiscard]] bool validate_uniqueness(
    ValidationContext& ctx, model::OrderedChord& chord);

/**
 * \brief Stage 11: Voice-leading vector constraints.
 *
 * Computes the voice-leading vector and validates per-voice movement range,
 * direction constraints, common notes, sv range.
 *
 * Legacy: `find_vec()` + `valid_vec()` + c/sv checks in `Chord::valid()`.
 */
[[nodiscard]] bool validate_voice_leading(
    ValidationContext& ctx, model::OrderedChord& chord);

/**
 * \brief Stage 12: Similarity constraint.
 *
 * Legacy: `valid_sim()` in `Chord`.
 */
[[nodiscard]] bool validate_similarity(
    ValidationContext& ctx, model::OrderedChord& chord);

/**
 * \brief Stage 13: Circle of Fifths span and super-span constraints.
 *
 * Legacy: s_min/max and ss_min/max checks in `Chord::valid()`.
 */
[[nodiscard]] bool validate_span(
    ValidationContext& ctx, model::OrderedChord& chord);

/**
 * \brief Stage 14: Q indicator constraint.
 *
 * Legacy: q_min/max check in `Chord::valid()`.
 */
[[nodiscard]] bool validate_q_indicator(
    ValidationContext& ctx, model::OrderedChord& chord);

/**
 * \brief Stage 15: Movement vector uniqueness (no duplicate vec patterns).
 *
 * Legacy: vec_id uniqueness in `Chord::valid()`.
 */
[[nodiscard]] bool validate_vec_uniqueness(
    ValidationContext& ctx, model::OrderedChord& chord);

// ── Validation Pipeline ─────────────────────────────────────────────

/**
 * \brief Chains validators with short-circuit evaluation.
 *
 * Runs each validator in order. Returns false on first failure.
 */
class ChordValidationPipeline {
public:
  /**
   * \brief Constructs the default validation pipeline with all stages.
   */
  ChordValidationPipeline();

  /**
   * \brief Runs the validation pipeline on a candidate chord.
   *
   * \param ctx   Validation context with config, previous chord, etc.
   * \param chord The candidate chord to validate (may be modified: dedup, stats).
   * \return true if the chord passes all validation stages.
   */
  [[nodiscard]] bool validate(ValidationContext& ctx, model::OrderedChord& chord) const;

private:
  std::vector<Validator> _validators;
};

} // namespace chordnovarw::algorithm

#endif // CHORDNOVARW_SRC_INCLUDE_ALGORITHM_VALIDATION_H_
