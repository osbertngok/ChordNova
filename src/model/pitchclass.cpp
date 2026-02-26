#include "constant.h"
#include "model/pitchclass.h"

#include <string>
#include "exception.h"

namespace chordnovarw {
  namespace model {

    const PitchClass PitchClass::C = PitchClassEnum::C;
    const PitchClass PitchClass::Cs = PitchClassEnum::Cs;
    const PitchClass PitchClass::Db = PitchClassEnum::Db;
    const PitchClass PitchClass::D = PitchClassEnum::D;
    const PitchClass PitchClass::Ds = PitchClassEnum::Ds;
    const PitchClass PitchClass::Eb = PitchClassEnum::Eb;
    const PitchClass PitchClass::E = PitchClassEnum::E;
    const PitchClass PitchClass::F = PitchClassEnum::F;
    const PitchClass PitchClass::Fs = PitchClassEnum::Fs;
    const PitchClass PitchClass::Gb = PitchClassEnum::Gb;
    const PitchClass PitchClass::G = PitchClassEnum::G;
    const PitchClass PitchClass::Gs = PitchClassEnum::Gs;
    const PitchClass PitchClass::Ab = PitchClassEnum::Ab;
    const PitchClass PitchClass::A = PitchClassEnum::A;
    const PitchClass PitchClass::As = PitchClassEnum::As;
    const PitchClass PitchClass::Bb = PitchClassEnum::Bb;
    const PitchClass PitchClass::B = PitchClassEnum::B;

    const std::unordered_map<std::string, PitchClass> PitchClass::table = {
      {"A-", PitchClass::Ab},
      {"A", PitchClass::A},
      {"A#", PitchClass::As},
      {"B-", PitchClass::Bb},
      {"B", PitchClass::B},
      {"C", PitchClass::C},
      {"Cs", PitchClass::Cs},
      {"D-", PitchClass::Db},
      {"D", PitchClass::D},
      {"D#", PitchClass::Ds},
      {"E-", PitchClass::Eb},
      {"E", PitchClass::E},
      {"F", PitchClass::F},
      {"F#", PitchClass::Fs},
      {"G-", PitchClass::Gb},
      {"G", PitchClass::G},
      {"G#", PitchClass::Gs},
    };


  PitchClass to_pitch_class(const std::string &str) {
      auto it = PitchClass::table.find(str);
      if (it != PitchClass::table.end()) {
          return it->second;
        } else {
            throw chordnovaexception::ChordNovaGenericException(std::string("Cannot find pitch class \"") + str + "\"");
          }
    }

    PitchClass::PitchClass(char pc) {
      _value = pc;
    }

    PitchClass::PitchClass(PitchClassEnum pce) {
      _value = (char) pce;
    }

    PitchClass::operator int() const {
      return (int) _value;
    }

    bool PitchClass::operator==(const PitchClass &pitch_class) const {
      return _value == pitch_class._value;
    }

    char PitchClass::value() const {
      return _value;
    }

    Semitone::Semitone(int value) : _value(value) {
    }

    int Semitone::value() const {
      return _value;
    }

    bool PitchClass::operator<(const PitchClass &pitch_class) const {
        return _value < pitch_class._value;
    }

    bool PitchClass::operator>(const PitchClass &pitch_class) const {
        return _value > pitch_class._value;
    }

    bool Semitone::operator==(const Semitone &semitone) const {
      return _value == semitone._value;
    }

    COFUnit::COFUnit(int value) : _value(value) {
    }

    int COFUnit::value() const {
      return _value;
    }

    bool COFUnit::operator==(const COFUnit &cofunit) const {
      return _value == cofunit._value;
    }

    bool COFUnit::operator<(const COFUnit &cofunit) const {
        return _value < cofunit._value;
    }

    bool COFUnit::operator>(const COFUnit &cofunit) const {
        return _value > cofunit._value;
    }

    COFUnit COFUnit::operator-(const COFUnit &cofunit) const {
        return COFUnit(_value - cofunit._value);
    }

    Semitone get_interval(const PitchClass &from_pitch_class, const PitchClass &to_pitch_class) {
      return Semitone((ET_SIZE + to_pitch_class.value() - from_pitch_class.value()) % ET_SIZE);
    }

    Chroma PitchClass::get_chroma() const {
      return Chroma(ET_SIZE / 2 - ((ET_SIZE / 2 - 1) * (_value % ET_SIZE) + ET_SIZE / 2) % ET_SIZE);
    }

    std::string PitchClass::to_string() const {
      int chroma = get_chroma().get_chroma();
      const char table[7] = {'F', 'C', 'G', 'D', 'A', 'E', 'B'};
      std::string result(1, table[(chroma + 36) % 7]);
      int accidental = (chroma + 36) / 7 - 5;
      switch (accidental) {
        case -2: result += "bb"; break;
        case -1: result += "b"; break;
        case 1: result += "#"; break;
        case 2: result += "x"; break;
        default: break;
      }
      return result;
    }

    /**
     *
     * Distance of two pitch classes on circle of fifths.
     * It does not consider tonality, and treats all enharmonically equivalent pitches the same.
     * @param from_pitch_class starting pitch class
     * @param to_pitch_class ending pitch class
     * @return Distance of two pitch classes on circle of fifths
     *
     * \internal
     * because FIFTH_IN_SEMITONE = 7, and 7 * 7 ≡ 1 (mode 1), so 7 * 7 * d ≡ d (mod 12).
     * We just need to know the difference d of semitones between these two pitch class,
     * and the COF distance can be expressed as 7 * d (mod 12).
     *
     * The original implementation,
     * chord.single_chroma.push_back(6 - (5 * (chord.notes[i] % ET_SIZE) + 6) % ET_SIZE);
     * is using similar logics (using (-5) instead of 7, but essentially the same in the context of modulo 12)
     *
     * Pitch Class                     Gb  Db  Ab  Eb  Bb  F   C   G   D   A   E   B
     * Old Impl.                       -6  -5  -4  -3  -2  -1  0   1   2   3   4   5
     * New Impl. (in COF Unit)          6   7   8   9  10  11  0   1   2   3   4   5
     */
    COFUnit get_circle_of_fifth_distance(const PitchClass &from_pitch_class, const PitchClass &to_pitch_class) {
      return COFUnit(7 * get_interval(from_pitch_class, to_pitch_class).value() % ET_SIZE);
    }
  }
}
