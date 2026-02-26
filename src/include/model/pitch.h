#ifndef CHORDNOVARW_SRC_INCLUDE_MODEL_PITCH_H_
#define CHORDNOVARW_SRC_INCLUDE_MODEL_PITCH_H_
#include "circleoffifths.h"
#include "pitchclass.h"
#include "octave.h"
#include <string>

namespace chordnovarw::model {
 /**
  * \brief Pitch of a note.
  */
 class Pitch {
 public:
  explicit Pitch(char midi_num);
  Pitch(const Pitch &pitch);
  Pitch(PitchClass pitch_class, Octave octave);
  explicit Pitch(const std::string &str);
  [[nodiscard]] PitchClass get_pitch_class() const;
  [[nodiscard]] Octave get_octave() const;
  [[nodiscard]] char get_number() const;
  [[nodiscard]] Chroma get_chroma() const;

  [[nodiscard]] std::string to_string() const;

  bool operator<(const Pitch &pitch) const;
  bool operator>(const Pitch &pitch) const;
  int operator-(const Pitch &pitch) const;
  bool operator==(const Pitch &pitch) const;
 private:
  char _note;
};
}

#endif //CHORDNOVARW_SRC_INCLUDE_MODEL_PITCH_H_
