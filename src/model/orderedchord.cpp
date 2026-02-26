//
// Created by Osbert Nephide Ngok on 18/12/2024.
//
#include "model/orderedchord.h"
#include "model/pitchset.h"
#include "exception.h"
#include "utility.h"

using namespace std;

namespace chordnovarw {
  namespace model {
    OrderedChord::OrderedChord(vector<Pitch> pitches) : _pitches(std::move(pitches)) {
    }

    OrderedChord::OrderedChord(const string &str) {
    vector<string> split_str = split(str, ' ');
    _pitches.clear();
    try {
      transform(split_str.begin(),
                split_str.end(),
                back_inserter(_pitches),
                [](const string &pitch_name) { return Pitch(pitch_name); });
    } catch (const exception &e) {
      throw chordnovaexception::ChordNovaGenericException(string("Cannot construct Chord from ") + str + " ; Error: " + e.what());
    }

  }

  bool OrderedChord::contains_pitch_class(PitchClass pitch_class) const {
    return any_of(_pitches.cbegin(),
                  _pitches.cend(),
                  [&pitch_class = static_cast<const PitchClass &>(pitch_class)](const Pitch &pitch) {
                    return pitch.get_pitch_class() == pitch_class;
                  });
  }

  bool OrderedChord::contains_pitch(const Pitch &pitch_) const {
    return any_of(_pitches.cbegin(),
                  _pitches.cend(),
                  [&pitch_ = static_cast<const Pitch &>(pitch_)](const Pitch &pitch) {
                    return pitch_ == pitch;
                  });
  }

  vector<Pitch> OrderedChord::get_pitches() const {
    vector<Pitch> ret;
    // Copying vector by insert function
    ret.insert(ret.begin(), _pitches.cbegin(), _pitches.cend());
    return ret;
  }

  bool OrderedChord::operator<(const OrderedChord &chord) const {
    if (_pitches.size() < chord._pitches.size()) {
      return true;
    }
    if (_pitches.size() > chord._pitches.size()) {
      return false;
    }
    for (auto i = 0; i < _pitches.size(); ++i) {
      if (_pitches[i] < chord._pitches[i]) {
        return true;
      }
      if (_pitches[i] > chord._pitches[i]) {
        return false;
      }
    }
    return false;
  }

  size_t OrderedChord::get_num_of_pitches() const {
    return _pitches.size();
  }

  size_t OrderedChord::get_num_of_unique_pitches() const {
    set<Pitch> pitches;
    for (const auto &pitch : _pitches) {
      pitches.insert(pitch);
    }
    return pitches.size();
  }

  PitchSet OrderedChord::to_set() const {
    const auto pitch_set = PitchSet(*this);
    return pitch_set; // Don't attempt to move explicitly; let RVO do its job after c++14!
  }

  double OrderedChord::get_tension() const {
    return to_set().get_tension();
  }

  double OrderedChord::get_thickness() const {
    return to_set().get_thickness();
  }

  optional<PitchClass> OrderedChord::find_root() const {
    return to_set().find_root();
  }
  double OrderedChord::get_geometrical_center() const {
    return to_set().get_geometrical_center();
  }

vector<PitchClass> OrderedChord::get_pitch_classes_ordered_by_circle_of_fifths() const {
  // pitch class however might not be a set
  set<PitchClass> pitch_class_set = {};
  for (const auto& pitch: _pitches) {
    pitch_class_set.insert(pitch.get_pitch_class());
  }
  // Create a vector from the set
  std::vector<PitchClass> orderedVector(pitch_class_set.begin(), pitch_class_set.end());

  // Sort the vector based on get_chroma()
  std::sort(orderedVector.begin(), orderedVector.end(),
            [](const PitchClass& a, const PitchClass& b) {
                return a.get_chroma() < b.get_chroma();
            });

  return orderedVector;

}
  }
}
