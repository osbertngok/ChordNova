//
// Created by Osbert Nephide Ngok on 18/12/2024.
//

#ifndef ORDEREDCHORD_H
#define ORDEREDCHORD_H
#include "chord.h"
#include "pitchiterable.h"

namespace chordnovarw {
  namespace model {
    class PitchSet;
    /**
     * \brief An ordered array of pitches. There might be (adjacent) equivalent pitches.
     */
    class OrderedChord : public PitchIterable {
    public:
      /**
       * \brief Construct an Ordered Chord by pitch names, delimited by space.
       * @param str string of the list of pitch names
       */
      explicit OrderedChord(const std::string &str);
      /**
       * \brief Construct an Ordered Chord directly from a vector of pitches.
       * @param pitches the pitches to use (order is preserved)
       */
      explicit OrderedChord(std::vector<Pitch> pitches);
      [[nodiscard]] bool contains_pitch_class(PitchClass pitch_class) const override;
      [[nodiscard]] bool contains_pitch(const Pitch& pitch) const override;
      [[nodiscard]] size_t get_num_of_pitches() const;
      [[nodiscard]] size_t get_num_of_unique_pitches() const;
      [[nodiscard]] double get_tension() const override;
      [[nodiscard]] double get_thickness() const override;
      [[nodiscard]] double get_geometrical_center() const override;
      [[nodiscard]] std::optional<PitchClass> find_root() const override;
      [[nodiscard]] std::vector<Pitch> get_pitches() const override;
      [[nodiscard]] std::vector<PitchClass> get_pitch_classes_ordered_by_circle_of_fifths() const override;
      [[nodiscard]] PitchSet to_set() const;

      bool operator<(const OrderedChord &chord) const;



    private:
      std::vector<Pitch> _pitches;
    };
  }
}
#endif //ORDEREDCHORD_H
