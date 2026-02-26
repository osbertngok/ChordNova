#include "model/chord.h"
#include "model/chordstatistics.h"
#include "service/chordlibrary.h"

namespace chordnovarw::service {
const OrderedChordStatistics& ChordLibrary::get_chord_data(const OrderedChord& chord) {
  auto it = _data.find(chord);
  if (it == _data.end()) {
    it = _data.emplace(chord, calculate_statistics(chord)).first;
  }
  return it->second;
}
}

