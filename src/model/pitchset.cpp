//
// Created by Osbert Nephide Ngok on 18/12/2024.
//

#include "model/pitchset.h"
#include "model/orderedchord.h"
#include "model/chord.h"
#include <constant.h>
#include "utility.h"

using namespace std;

namespace chordnovarw {
  namespace model {
    PitchSet::PitchSet(const string &str) {
    vector<string> split_str = split(str, ' ');
    vector<Pitch> vec_pitch;

    transform(split_str.begin(),
              split_str.end(),
              back_inserter(vec_pitch),
              [](const string &pitch_name) { return Pitch(pitch_name); });
    _pitches.clear();
    for (const auto &pitch : vec_pitch) {
      _pitches.insert(pitch);
    }
  }

  PitchSet::PitchSet(const OrderedChord &chord) {
    _pitches.clear();
    for (const auto &pitch : chord.get_pitches()) {
      _pitches.insert(pitch);
    }
  }

  // Reference: https://www.ux1.eiu.edu/~pdhesterman/old/analysis/chord_roots.html
  // Notes should be sorted in ascending order.
  optional<PitchClass> PitchSet::find_root() const {
    int interval_rank[ET_SIZE] = {11, 8, 6, 5, 3, 0, 10, 1, 2, 4, 7, 9};
    // int intervals[11] = {5, 7, 8, 4, 9, 3, 2, 10, 1, 11, 6};
    // 'interval_rank[interval]' is the position of 'interval' in the array above.
    // Odd numbers represent that the lower note is the root, even numbers the opposite; -1 is a random value.
    vector<Pitch> pitch_vector;
    std::copy(_pitches.begin(), _pitches.end(), std::back_inserter(pitch_vector));
    Pitch root = *pitch_vector.rbegin();
    const size_t t_size = pitch_vector.size();
    int best_interval = 6;
    for (int i = 0; i < t_size; ++i)
      for (int j = i + 1; j < t_size; ++j) {
        int interval = (pitch_vector[j] - pitch_vector[i]) % ET_SIZE;
        int rank = interval_rank[interval];
        if (rank / 2 < interval_rank[best_interval] / 2) {
          root = pitch_vector[(rank % 2) ? i : j];
          best_interval = interval;
        }
      }
    return root.get_pitch_class();
  }

  bool PitchSet::contains_pitch_class(PitchClass pitch_class) const {
    return any_of(_pitches.cbegin(),
                  _pitches.cend(),
                  [&pitch_class = static_cast<const PitchClass &>(pitch_class)](const Pitch &pitch) {
                    return pitch.get_pitch_class() == pitch_class;
                  });
  }

  bool PitchSet::contains_pitch(const Pitch &pitch_) const {
    return any_of(_pitches.cbegin(),
                  _pitches.cend(),
                  [&pitch_ = static_cast<const Pitch &>(pitch_)](const Pitch &pitch) {
                    return pitch_ == pitch;
                  });
  }

  vector<Pitch> PitchSet::get_pitches() const {
    vector<Pitch> ret;
    // Copying vector by insert function
    ret.insert(ret.begin(), _pitches.cbegin(), _pitches.cend());
    return ret;
  }

  /**
   * \brief Calculate the tension value
   *
   * Traverse all intervals between the notes
   * For each interval, calculate the tension score
   * for second note in the note pair that are high enough.
   *
   * Per https://www.bilibili.com/video/BV1Ja411s7mA, the 5 problems above are partially handled:
   *
   * \li Tension value is not normalized by note count: NOT addressed. Its value is proportional to \f$n^2\f$.
   * \li The weight vector is arbitrary: PARTIALLY addressed. Weight vector used here is proposed by 赵晓生, based on consonance of an interval.
   * \li The arrangement of the chord should impact tension score: NOT addressed in this algo. This algo however does consider penalty when notes are in lower register.
   * \li The prime form of an asymmetrical forte arrangement should have different score of the inverse form: NOT addressed in this algo. The weight vector is symmetrical.
   * \li Timbre also impacts tension value. NOT addressed.
   *
   * @return tension value
   */
  double PitchSet::get_tension() const {
    const vector<Pitch> pitch_vec = get_pitches();
    double tension = 0.0;
    const size_t t_size = pitch_vec.size();
    for (int i = 0; i < t_size; ++i)
      for (int j = i + 1; j < t_size; ++j) {
        int diff = pitch_vec[j] - pitch_vec[i];
        // If it is many octaves apart, the tension is smaller.
        double temp = ZXS_TENSION_WEIGHT_VECTOR[diff % ET_SIZE] / (static_cast<double>(diff / ET_SIZE) + 1.0);
        if (pitch_vec[j].get_number() < restriction[diff % ET_SIZE])
          temp = temp * restriction[diff % ET_SIZE] / pitch_vec[j].get_number();
        tension += temp;
      }
    tension /= 10.0;
    return tension;
  }

  double PitchSet::get_thickness() const {
    double thickness = 0.0;
    const vector<Pitch> pitch_vec = get_pitches();
    const size_t t_size = pitch_vec.size();
    for (int i = 0; i < t_size; ++i)
      for (int j = i + 1; j < t_size; ++j)
        if ((pitch_vec[j] - pitch_vec[i]) % ET_SIZE == 0)
          thickness += (double) ET_SIZE / (double) (pitch_vec[j] - pitch_vec[i]);
    return thickness;
  }

  double PitchSet::get_geometrical_center() const {
    int sum_of_pitch = 0;
    const vector<Pitch> pitch_vec = get_pitches();
    const size_t t_size = pitch_vec.size();
    optional<Pitch> min_pitch = nullopt;
    optional<Pitch> max_pitch = nullopt;
    for (const auto &pitch : pitch_vec) {
      if (!min_pitch.has_value() || pitch < min_pitch) {
        min_pitch = pitch;
      }
      if (!max_pitch.has_value() || pitch > max_pitch) {
        max_pitch = pitch;
      }
      sum_of_pitch += pitch.get_number();
    }
    if (min_pitch == max_pitch) {
      return 0.5;
    } else {
      return ((double) sum_of_pitch / (double) t_size - min_pitch->get_number())
          / (max_pitch.value() - min_pitch.value());
    }
  }


  vector<PitchClass> PitchSet::get_pitch_classes_ordered_by_circle_of_fifths() const {
    set<PitchClass> pitch_class_set = {};
    for (const auto& pitch: _pitches) {
      pitch_class_set.insert(pitch.get_pitch_class());
    }
    std::vector<PitchClass> orderedVector(pitch_class_set.begin(), pitch_class_set.end());
    std::sort(orderedVector.begin(), orderedVector.end(),
              [](const PitchClass& a, const PitchClass& b) {
                  return a.get_chroma() < b.get_chroma();
              });
    return orderedVector;
  }
  }
}
