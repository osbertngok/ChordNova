#ifndef CHORDNOVARW_SRC_INCLUDE_MODEL_PITCHCLASS_H_
#define CHORDNOVARW_SRC_INCLUDE_MODEL_PITCHCLASS_H_
#include <unordered_map>
#include <string>
#include "circleoffifths.h"

namespace chordnovarw::model {
  /**
   * \brief The musical Semitone.
   */
  class Semitone {
  public:
    explicit Semitone(int value);

    bool operator==(const Semitone &semitone) const;

    [[nodiscard]] int value() const;

  private:
    int _value;
  };

  /**
   * \brief Unit in Circle of Fifths.
   *
   * It is a new type for int for easier arithmetic of pitch / pitch class and interval etc..
   */
  class COFUnit {
  public:
    explicit COFUnit(int value);

    bool operator<(const COFUnit &cofunit) const;

    bool operator>(const COFUnit &cofunit) const;

    bool operator==(const COFUnit &cofunit) const;

    COFUnit operator-(const COFUnit &cofunit) const;

    [[nodiscard]] int value() const;

  private:
    int _value;
  };

  enum class PitchClassEnum : char {
    C = 0,
    Cs = 1,
    Db = 1,
    D = 2,
    Ds = 3,
    Eb = 3,
    E = 4,
    F = 5,
    Fs = 6,
    Gb = 7,
    G = 7,
    Gs = 8,
    Ab = 9,
    A = 9,
    As = 10,
    Bb = 10,
    B = 11
  };

  /**
   * \brief The musical Pitch Class.
   *
   * Current implementation does not consider spelling difference.
   */
  class PitchClass {
  public:
    explicit PitchClass(char pc);

    /**
     * \brief Convert PitchClassEnum to PitchClass
     * @param pce the pitch class enum.
     *
     * \internal
     * PitchClass and PitchClassEnum is the same thing, similar to string and char*, so it is ok to use implicit conversion
     * We need to enhance PitchClassEnum so that we can use member functions.
     */
    explicit(false) PitchClass(PitchClassEnum pce); // NOLINT(*-explicit-constructor)

    explicit operator int() const;

    bool operator<(const PitchClass &pitch_class) const;

    bool operator>(const PitchClass &pitch_class) const;

    bool operator==(const PitchClass &pitch_class) const;

    [[nodiscard]] char value() const;

    [[nodiscard]] Chroma get_chroma() const;

    [[nodiscard]] std::string to_string() const;

    static const PitchClass C;
    static const PitchClass Cs;
    static const PitchClass Db;
    static const PitchClass D;
    static const PitchClass Ds;
    static const PitchClass Eb;
    static const PitchClass E;
    static const PitchClass F;
    static const PitchClass Fs;
    static const PitchClass Gb;
    static const PitchClass G;
    static const PitchClass Gs;
    static const PitchClass Ab;
    static const PitchClass A;
    static const PitchClass As;
    static const PitchClass Bb;
    static const PitchClass B;

    static const std::unordered_map<std::string, PitchClass> table;

  private:
    char _value;
  };

  PitchClass to_pitch_class(const std::string &str);

  Semitone get_interval(const PitchClass &from_pitch_class, const PitchClass &to_pitch_class);

  COFUnit get_circle_of_fifth_distance(const PitchClass &from_pitch_class, const PitchClass &to_pitch_class);
}

#endif //CHORDNOVARW_SRC_INCLUDE_MODEL_PITCHCLASS_H_
