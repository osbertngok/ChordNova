//
// Created by Osbert Nephide Ngok on 18/12/2024.
//
#ifndef PITCHITERABLE_H
#define PITCHITERABLE_H
#include <optional>
#include <vector>
#include "pitch.h"
#include "pitchclass.h"

namespace chordnovarw {
  namespace model {
    /**
     * \brief A Container of @ref Pitch.
     */
    class PitchIterable {
    public:
      virtual ~PitchIterable() = default;

      /**
       * \brief Whether current pitch container contains a certain pitch class.
       * @param pitch_class pitch class to check
       * @return true if there exists a pitch in the pitch container
       */
      [[nodiscard]] virtual bool contains_pitch_class(PitchClass pitch_class) const = 0;
      /**
       * \brief Whether current pitch container contains a contain pitch.
       * @param pitch pitch subject to check
       * @return true if the pitch is in the container.
       */
      [[nodiscard]] virtual bool contains_pitch(const Pitch& pitch) const = 0;
      /**
       * \brief Calculate the tension value of the current pitch container.
       */
      [[nodiscard]] virtual double get_tension() const = 0;
      /**
       * \brief Calculate the thickness value of the current pitch container.
       */
      [[nodiscard]] virtual double get_thickness() const = 0;
      /**
       * \brief Calculate the geometrical center of the current pitch container.
       */
      [[nodiscard]] virtual double get_geometrical_center() const = 0;
      /**
       *\brief  Calculate the root pitch class of the current pitch container.
       */
      [[nodiscard]] virtual std::optional<PitchClass> find_root() const = 0;
      /**
       * \brief return a copy of the pitches in vector form. Guarantee not modifying the inner contents.
       */
      [[nodiscard]] virtual std::vector<Pitch> get_pitches() const = 0;
      /**
       * \brief Calculate the pitch classes used in the pitch container, ordered by circle of fifths.
       */
      [[nodiscard]] virtual std::vector<PitchClass> get_pitch_classes_ordered_by_circle_of_fifths() const = 0;
      /**
      * \brief Calculate the minimal span on circle of fifths.
      */
      [[nodiscard]] virtual COFUnit get_span() const;
    };
  }
}


#endif //PITCHITERABLE_H
