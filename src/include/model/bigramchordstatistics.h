#ifndef CHORDNOVARW_SRC_INCLUDE_MODEL_BIGRAMCHORDSTATISTICS_H_
#define CHORDNOVARW_SRC_INCLUDE_MODEL_BIGRAMCHORDSTATISTICS_H_

#include <string>
#include <vector>
#include "chordstatistics.h"
#include "orderedchord.h"

namespace chordnovarw{
  namespace model {

    /**
     * \brief Overflow state for Circle of Fifths wrapping.
     *
     * When computing \ref BigramChordStatistics::chroma_old "chroma_old",
     * the average Circle of Fifths position may jump by more than 6 units
     * from the previous chord, in which case a ±12 wrap is applied.
     */
    enum OverflowState {
      NoOverflow, ///< No wrapping was applied.
      Single,     ///< Overflow adjusted after note-name generation.
      Total       ///< Full ±12 wrapping was applied to chroma_old.
    };

    /**
     * \brief Statistics describing the relationship between two consecutive chords (a bigram).
     *
     * Each instance holds both per-chord descriptors (notes, pitch-class set,
     * span, naming) and bigram-specific metrics that measure how the two chords
     * relate (voice-leading distance, harmonic distance on the Circle of Fifths,
     * similarity, etc.).
     *
     * Legacy variable names from the original ChordNova codebase are noted in
     * parentheses where applicable (e.g. \e kk, \e k, \e Q).
     */
    class BigramChordStatistics {
    public:
      // ── Circle of Fifths metrics ──────────────────────────────────

      /**
       * \brief Average Circle of Fifths position of the chord's unique pitch classes (\e kk).
       *
       * Computed as the arithmetic mean of the Circle of Fifths positions of
       * all unique pitch classes, truncated to two decimal places. If the
       * difference from \ref prev_chroma_old exceeds ±6, a ±12 wrap is applied
       * and \ref overflow_state is set to \ref Total.
       */
      const double chroma_old;

      /**
       * \brief The previous chord's \ref chroma_old value.
       *
       * Stored before computing the current chord's chroma_old so that
       * overflow wrapping can be detected.
       */
      const double prev_chroma_old;

      /**
       * \brief Harmonic distance between two consecutive chords on the Circle of Fifths (\e k).
       *
       * Derived from the pairwise distances of pitch classes exclusive to each
       * chord, scaled through an arctan function into the range (−100, +100).
       * Negative values indicate flat-ward movement; positive values indicate
       * sharp-ward movement.
       */
      const double chroma;

      /**
       * \brief Combined harmonic-complexity / voice-leading indicator (\e Q).
       *
       * \f$ Q = \frac{k \cdot (t_1 + t_2)}{2 \cdot \max(n_1, n_2)} \f$
       *
       * where \e k is \ref chroma, \e t is tension, and \e n is note count.
       * Higher absolute values indicate more demanding progressions.
       */
      const double Q_indicator;

      // ── Voice-leading metrics ─────────────────────────────────────

      /**
       * \brief Number of pitches (exact MIDI values) shared by both chords (\e c).
       */
      const int common_note;

      /**
       * \brief Total voice-leading distance in semitones (\e sv, Σvec).
       *
       * Sum of absolute values of the voice-movement vector \ref vec.
       * When the two chords differ in size, the smaller chord is expanded
       * using the combination that minimises this sum.
       */
      const int sv;

      // ── Circle of Fifths span metrics ────────────────────────────

      /**
       * \brief Span of this chord on the Circle of Fifths (\e s).
       *
       * The minimum range of Circle of Fifths positions across all
       * enharmonic rotations. Range: 0–12.
       */
      const int span;

      /**
       * \brief Super-span: span of the union of both chords on the Circle of Fifths (\e ss).
       *
       * Measures how spread out the combined pitch-class material is.
       * Range: 0–12.
       */
      const int sspan;

      // ── Similarity metrics ────────────────────────────────────────

      /**
       * \brief Voice-leading similarity between the two chords (\e x).
       *
       * Percentage (0–100) derived from the voice-leading distance \ref sv
       * relative to a theoretical maximum. Boosted (square-rooted) when both
       * chords share the same root.
       */
      const int similarity;

      /**
       * \brief Original / baseline similarity value (\e p), default 100%.
       */
      const int sim_orig;

      // ── Voice-movement direction counts ───────────────────────────

      /** \brief Number of voices that remain on the same pitch (vec[i] == 0). */
      const int steady_count;
      /** \brief Number of voices that move upward (vec[i] > 0). */
      const int ascending_count;
      /** \brief Number of voices that move downward (vec[i] < 0). */
      const int descending_count;

      // ── Root movement ─────────────────────────────────────────────

      /**
       * \brief Shortest chromatic distance between the roots of the two chords.
       *
       * Always the shorter way around the chromatic circle. Range: 0–6
       * (0 = same root, 6 = tritone).
       */
      const int root_movement;

      // ── Naming / display fields ───────────────────────────────────

      /** \brief Human-readable root name (e.g. "C", "F#", "Bb"). */
      const std::string root_name;
      /** \brief Whether to suppress octave numbers in display output. */
      const bool hide_octave;
      /** \brief Note names of all pitches in the chord (e.g. "C E G"). */
      const std::string name;
      /** \brief Note names with octave numbers (e.g. "C4 E4 G4"). */
      const std::string name_with_octave;

      // ── Overflow tracking ─────────────────────────────────────────

      /** \brief Type of Circle of Fifths overflow adjustment applied. */
      const OverflowState overflow_state;
      /**
       * \brief The ±12 adjustment applied to \ref single_chroma values
       *        when overflow wrapping occurs.
       */
      const int overflow_amount;

      // ── Per-note vectors ──────────────────────────────────────────

      /** \brief MIDI note numbers, sorted low → high. */
      const std::vector<int> notes;
      /**
       * \brief Unique pitch classes (0–11), sorted low → high.
       *
       * Not necessarily zero-based (e.g. {4, 7, 11} for an E-minor triad).
       */
      const std::vector<int> pitch_class_set;
      /** \brief Circle of Fifths position for each note, in ascending order. */
      const std::vector<int> single_chroma;
      /**
       * \brief Voice-leading vector (\e v).
       *
       * Each element is the signed semitone interval a voice moves
       * from the previous chord to this one (positive = up, negative = down).
       */
      const std::vector<int> vec;
      /** \brief Self-difference vector (\e d): intervals within the chord. */
      const std::vector<int> self_diff;
      /** \brief Interval-class frequency vector (\e vec). */
      const std::vector<int> count_vec;
      /**
       * \brief Voice alignment mapping.
       *
       * e.g. [3, 1, 7] means the chord is voiced as
       * "3rd scale degree, root, 7th scale degree".
       */
      const std::vector<int> alignment;

      BigramChordStatistics(
          double chroma_old, double prev_chroma_old, double chroma,
          double Q_indicator, int common_note, int sv, int span, int sspan,
          int similarity, int sim_orig, int steady_count, int ascending_count,
          int descending_count, int root_movement,
          std::string root_name, bool hide_octave,
          std::string name, std::string name_with_octave,
          OverflowState overflow_state, int overflow_amount,
          std::vector<int> notes, std::vector<int> pitch_class_set,
          std::vector<int> single_chroma, std::vector<int> vec,
          std::vector<int> self_diff, std::vector<int> count_vec,
          std::vector<int> alignment
      );
    };

    [[nodiscard]] BigramChordStatistics calculate_bigram_statistics(
        const OrderedChord &prev_chord,
        const OrderedChord &curr_chord,
        const OrderedChordStatistics &prev_stats,
        const OrderedChordStatistics &curr_stats,
        const std::vector<int> &vec,
        int sv,
        int vl_max,
        double prev_chroma_old,
        const std::vector<int> &prev_single_chroma
    );
  }
}

#endif //CHORDNOVARW_SRC_INCLUDE_MODEL_BIGRAMCHORDSTATISTICS_H_
