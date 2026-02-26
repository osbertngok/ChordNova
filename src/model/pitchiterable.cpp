//
// Created by Osbert Nephide Ngok on 18/12/2024.
//
#include "model/pitchiterable.h"
#include "model/pitchclass.h"

using namespace std;

namespace chordnovarw {
  namespace model {
    /**
   * Find the minimal span on Circle of Fifths in COF distance.
   * Only the following pitch classes are supported, so that the calculation is faster (using modulo 12)
   *
   *
   * \internal
   * For example, [C] is 0, [C G] is 1, [C E G] => [C G (D) (A) E] = 4
   *
   * The algorithm used here is:
   * 1) list all Pitch Classes in Circle of Fifth, e.g. [C G E] or [E C G]
   * 2) Calculate COF distance for consecutive pairs. Notice the sum is definitely 12: 1, 3, 8
   * 3) We understand for sure that the minimal span must be less than 12 - 1, 12 - 3, 12 - 8, respectively,
   *    Because there is no pitch class in use between C / E, E / G, G / C pairs.
   * 4) Then the minimal span of COF is the min value of the values above, which is min(12 - 1, 12 - 3, 12 - 8).
   * 5) In which case, we are actually calculating 12 - max(1, 3, 8) = 12 - max( ∀ adjacent pair, COF_distance(pair[1] - pair[0]))
   */
    COFUnit PitchIterable::get_span() const {
      auto max_adjacent_pair_cof_distance = COFUnit(0);
      // Step 1:
      const vector<PitchClass> pitch_classes_ordered_by_circle_of_fifth = get_pitch_classes_ordered_by_circle_of_fifths();
      const size_t num_of_pitch_classes = pitch_classes_ordered_by_circle_of_fifth.size();
      for (int i = 0; i < num_of_pitch_classes; ++i) {
        COFUnit adjacent_pair_cof_distance = get_circle_of_fifth_distance(pitch_classes_ordered_by_circle_of_fifth[(i + 1) % num_of_pitch_classes], pitch_classes_ordered_by_circle_of_fifth[i]);
        if (adjacent_pair_cof_distance > max_adjacent_pair_cof_distance) {
          max_adjacent_pair_cof_distance = adjacent_pair_cof_distance;
        }
      }
      return COFUnit(12) - max_adjacent_pair_cof_distance;


    }
  }
}
