#ifndef CHORDNOVARW_SRC_INCLUDE_MODEL_OCTAVE_H_
#define CHORDNOVARW_SRC_INCLUDE_MODEL_OCTAVE_H_

namespace chordnovarw::model {
  /**
   * \brief Octave of the pitch.
   */
  enum class Octave : int {
  OMinus1 = -1,
  O0 = 0,
  O1 = 1,
  O2 = 2,
  O3 = 3,
  O4 = 4,
  O5 = 5,
  O6 = 6,
  O7 = 7
};
}

#endif //CHORDNOVARW_SRC_INCLUDE_MODEL_OCTAVE_H_
