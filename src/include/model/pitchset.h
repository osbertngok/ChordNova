//
// Created by Osbert Nephide Ngok on 18/12/2024.
//

#ifndef PITCHSET_H
#define PITCHSET_H
#include <set>
#include "pitchiterable.h"

namespace chordnovarw {
  namespace model {
    class OrderedChord;
    /**
     * \brief A set of pitches. All the pitches are different.
     */
    class PitchSet : public PitchIterable {
    public:
      explicit PitchSet(const std::string &str);
      explicit PitchSet(const OrderedChord &chord);
      [[nodiscard]] bool contains_pitch_class(PitchClass pitch_class) const override;
      [[nodiscard]] bool contains_pitch(const Pitch& pitch) const override;
      [[nodiscard]] double get_tension() const override;
      [[nodiscard]] double get_thickness() const override;
      [[nodiscard]] double get_geometrical_center() const override;
      [[nodiscard]] std::optional<PitchClass> find_root() const override;
      [[nodiscard]] std::vector<Pitch> get_pitches() const override;
      [[nodiscard]] std::vector<PitchClass> get_pitch_classes_ordered_by_circle_of_fifths() const override;

    private:
      std::set<Pitch> _pitches;

    };
  }
}
#endif //PITCHSET_H
