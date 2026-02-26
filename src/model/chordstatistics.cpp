#include "model/chord.h"
#include "model/chordstatistics.h"
#include "constant.h"
#include "utility.h"
#include <algorithm>
#include <bitset>

using namespace std;
using namespace chordnovarw::model;

namespace {
  const int note_pos[chordnovarw::ET_SIZE] = {1, 9, 9, 3, 3, 11, 11, 5, 13, 13, 7, 7};
}

namespace chordnovarw {
  namespace model {
    OrderedChordStatistics calculate_statistics(const OrderedChord &chord) {
      const auto root = chord.find_root();
      const auto pitches = chord.get_pitches();

      // alignment
      vector<int> alignment;
      if (root.has_value()) {
        for (const auto &pitch : pitches) {
          int diff = (pitch.get_number() - root->value()) % ET_SIZE;
          if (diff < 0) diff += ET_SIZE;
          alignment.push_back(note_pos[diff]);
        }
      }

      // pitch class set (sorted, unique) — bitset for O(1) dedup
      bitset<ET_SIZE> pc_bits;
      for (const auto &pitch : pitches) {
        pc_bits.set(pitch.get_pitch_class().value());
      }
      vector<int> pc_vec;
      for (int i = 0; i < ET_SIZE; ++i) {
        if (pc_bits.test(i)) pc_vec.push_back(i);
      }

      // self_diff
      const vector<int> normal = normal_form(pc_vec);
      vector<int> self_diff;
      for (size_t i = 1; i < normal.size(); ++i) {
        self_diff.push_back(normal[i] - normal[i - 1]);
      }

      // count_vec
      vector<int> count_vec(6, 0);
      for (size_t i = 0; i < pc_vec.size(); ++i) {
        for (size_t j = i + 1; j < pc_vec.size(); ++j) {
          int interval = pc_vec[j] - pc_vec[i];
          int ic = std::min(interval, ET_SIZE - interval);
          if (ic >= 1 && ic <= 6) {
            ++count_vec[ic - 1];
          }
        }
      }

      return {
        chord.get_num_of_pitches(),
        chord.get_num_of_unique_pitches(),
        chord.get_tension(),
        chord.get_thickness(),
        root,
        chord.get_geometrical_center(),
        alignment,
        self_diff,
        count_vec
      };
    }
  }
}

OrderedChordStatistics::OrderedChordStatistics(
    size_t num_of_pitches,
    size_t num_of_unique_pitches,
    double tension,
    double thickness,
    optional<PitchClass> root,
    double geometrical_center,
    vector<int> alignment,
    vector<int> self_diff,
    vector<int> count_vec
) : num_of_pitches(num_of_pitches),
    num_of_unique_pitch_classes(num_of_unique_pitches),
    tension(tension),
    thickness(thickness),
    root(root),
    geometrical_center(geometrical_center),
    alignment(std::move(alignment)),
    self_diff(std::move(self_diff)),
    count_vec(std::move(count_vec)) {
}
