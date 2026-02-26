#ifndef CHORDNOVARW_SRC_INCLUDE_MODEL_CHORD_H_
#define CHORDNOVARW_SRC_INCLUDE_MODEL_CHORD_H_

#include <constant.h>
#include <vector>
#include <log4cxx/logger.h>
#include "pitchclass.h"

namespace chordnovarw {
namespace model {
/**
 * \brief A set of distinct @ref PitchClass .
 */
class PitchClassSet {
 private:
  std::vector<PitchClass> _pitch_classes;
};

/**
  *
  * The weight of tension of an interval is the opposite of consonance of an interval, proposed by 赵晓生.
  * See also: https://www.bilibili.com/video/BV1HF411F7ji
  * and: https://www.bilibili.com/video/BV1Ja411s7mA
  *
  * \li Unison: 0
  * \li Minor Second: 11
  * \li Major Second: 8
  * \li Minor Third: 6
  * \li Major Third: 5
  * \li Perfect Forth: 3
  * \li Tritone: 7
  * \li Perfect Fifth: 3
  * \li Minor Sixth: 5
  * \li Major Sixth: 6
  * \li Minor Seventh 8
  * \li Major Seventh 11
 */
const double ZXS_TENSION_WEIGHT_VECTOR[ET_SIZE] = {0.0, 11.0, 8.0, 6.0, 5.0, 3.0, 7.0, 3.0, 5.0, 6.0, 8.0, 11.0};
const int restriction[ET_SIZE] = {0, 53, 53, 51, 50, 51, 52, 39, 51, 50, 51, 52};
const int overall_scale[ET_SIZE] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};


}
}

#endif //CHORDNOVARW_SRC_INCLUDE_MODEL_CHORD_H_
