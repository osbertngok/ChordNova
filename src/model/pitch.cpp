#include "exception.h"
#include "model/pitchclass.h"
#include "model/octave.h"
#include "model/pitch.h"
#include "logger.h"
#include "constant.h"

using namespace std;
using namespace chordnovaexception;
using namespace chordnovarw::model;

LOGGER("chordnovarw::model");

Pitch::Pitch(const Pitch &pitch) : _note(pitch.get_number()) {  // NOLINT: uint8_t copy
}

Pitch::Pitch(PitchClass pitch_class, Octave octave) {
  _note = static_cast<uint8_t>(static_cast<int>(pitch_class) + static_cast<int>(octave) * 12 + 12);
}

Pitch::Pitch(const string &str) {
  // FIXME: this parser function smells really bad; need re-write
  auto pitch_class = optional<PitchClass>{};
  auto octave = optional<Octave>{};
  const size_t STAGE_PITCH_CLASS = 0;
  const size_t STAGE_OCTAVE = 1;

  size_t stage = STAGE_PITCH_CLASS;

  size_t endpos = str.length();
  size_t pointer = 0;
  while (pointer < endpos) {
    if (pointer == 0) {
      pointer++;
      continue;
    } else if (pointer == 1) {
      if (str[pointer] == '#' || str[pointer] == '-') {
        // do nothing
      } else {
        pitch_class = to_pitch_class(str.substr(0, 1));
        stage = STAGE_OCTAVE;
      }
      pointer++;
    } else if (pointer == 2) {
      if (stage == STAGE_PITCH_CLASS) {
        pitch_class = to_pitch_class(str.substr(0, 2));
        stage = STAGE_OCTAVE;
      }
      pointer++;
      break;
    } else {
      throw ChordNovaGenericException(string("Unexpected construction string for Pitch ") + str);
    }
  }

  if (stage == STAGE_PITCH_CLASS) {
    pitch_class = to_pitch_class(str.substr(0, pointer));
    octave = Octave::OMinus1;
  } else if (stage == STAGE_OCTAVE) {
    string s_octave = str.substr(pointer - 1, 1);
    try {
      octave = static_cast<Octave>(stoi(s_octave));
    } catch (const exception &e) {
      throw ChordNovaGenericException(string("Cannot convert str ") + s_octave + " to octave. Error: " + e.what());
    }
  }
  if (pitch_class.has_value() && octave.has_value()) {
    _note = static_cast<uint8_t>(static_cast<int>(pitch_class.value()) + static_cast<int>(octave.value()) * 12 + 12);
  } else {
    throw ChordNovaGenericException("Cannot initialize pitch from string " + str);
  }

}

Octave Pitch::get_octave() const {
  return Octave(_note / 12 - 1);
}

PitchClass Pitch::get_pitch_class() const {
  return PitchClass(_note % 12);
}

uint8_t Pitch::get_number() const {
  return _note;
}

bool Pitch::operator<(const Pitch &pitch) const {
  return _note < pitch._note;
}

bool Pitch::operator>(const Pitch &pitch) const {
  return _note > pitch._note;
}

int Pitch::operator-(const Pitch &pitch) const {
  return _note - pitch._note;
}

bool Pitch::operator==(const Pitch &pitch) const {
  return _note == pitch._note;
}

Pitch::Pitch(uint8_t midi_num) : _note(midi_num) {
}

Chroma Pitch::get_chroma() const {
  return Chroma(ET_SIZE / 2 - ((ET_SIZE / 2 - 1) * (_note % ET_SIZE) + ET_SIZE / 2) % ET_SIZE);
}

std::string Pitch::to_string() const {
  return get_pitch_class().to_string() + std::to_string(static_cast<int>(get_octave()));
}

Chroma::Chroma(int chroma) : _chroma(chroma) {
}

int Chroma::get_chroma() const {
  return _chroma;
}
bool Chroma::operator<(const Chroma &chroma) const {
  return _chroma < chroma._chroma;
}

bool Chroma::operator>(const Chroma &chroma) const {
  return _chroma > chroma._chroma;
}

bool Chroma::operator==(const Chroma &chroma) const {
  return _chroma == chroma._chroma;
}