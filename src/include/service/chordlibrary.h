#ifndef CHORDNOVARW_SRC_INCLUDE_SERVICE_CHORDLIBRARY_H_
#define CHORDNOVARW_SRC_INCLUDE_SERVICE_CHORDLIBRARY_H_

#include "model/chord.h"
#include "model/chordstatistics.h"
#include <map>
#include <string>

namespace chordnovarw::service {
using namespace chordnovarw::model;
/**
 * Cache of the calculated characteristics of the chord
 */
class ChordLibrary {
 public:
  static ChordLibrary &getInstance() {
    static ChordLibrary instance;
    return instance;
  }
 private:
  std::map<OrderedChord, OrderedChordStatistics> _data;


  ChordLibrary() {}

 public:
  ChordLibrary(ChordLibrary const &) = delete;
  ChordLibrary& operator=(ChordLibrary const &) = delete;

  const OrderedChordStatistics& get_chord_data(const OrderedChord& chord);

};

}


#endif //CHORDNOVARW_SRC_INCLUDE_SERVICE_CHORDLIBRARY_H_
