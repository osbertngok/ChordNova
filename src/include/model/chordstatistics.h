#ifndef CHORDNOVARW_SRC_INCLUDE_MODEL_CHORDSTATISTICS_H_
#define CHORDNOVARW_SRC_INCLUDE_MODEL_CHORDSTATISTICS_H_
#include "orderedchord.h"
#include <optional>
#include <vector>

namespace chordnovarw {
  namespace model {
    /**
     * \brief The calculated properties of a chord.
     */
    class OrderedChordStatistics {
    public:
      /**
       * \brief size of notes.
       *
       * n; originally s.
       */
      const size_t num_of_pitches;
      /**
       * \brief size of pitch_class_set
       *
       * m; originally t.
       */
      const size_t num_of_unique_pitch_classes;
      /**
       * \brief tension
       *
       * t
       */
      const double tension;
      /**
       * \brief thickness
       *
       * h
       */
      const double thickness;
      /**
       * \brief root note.
       *
       * r
       */
      const std::optional<PitchClass> root;
      /**
       * \brief geometrical center ratio among min/max pitch span.
       *
       * Originally 0-100; In RW version, the range is 0-1.
       * g
       */
      const double geometrical_center;
      /**
       * \brief Scale-degree position of each note relative to root.
       *
       * For each note, (midi_note - root) % 12 mapped through note_pos lookup.
       * e.g. C major root-position -> [1, 3, 5]
       */
      const std::vector<int> alignment;
      /**
       * \brief Intervals in the normal form of the pitch-class set.
       *
       * Consecutive differences of the Forte normal form vector.
       */
      const std::vector<int> self_diff;
      /**
       * \brief Interval-class frequency vector (length 6).
       *
       * Histogram of interval classes 1-6 among all pairs of unique pitch classes.
       */
      const std::vector<int> count_vec;

      OrderedChordStatistics(
          size_t num_of_pitches,
          size_t num_of_unique_pitches,
          double tension,
          double thickness,
          std::optional<PitchClass> root,
          double geometrical_center,
          std::vector<int> alignment,
          std::vector<int> self_diff,
          std::vector<int> count_vec
      );
    };

    [[nodiscard]] OrderedChordStatistics calculate_statistics(const OrderedChord &chord);
  }
}

#endif //CHORDNOVARW_SRC_INCLUDE_MODEL_CHORDSTATISTICS_H_
