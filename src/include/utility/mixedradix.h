#ifndef CHORDNOVARW_SRC_INCLUDE_UTILITY_MIXEDRADIX_H_
#define CHORDNOVARW_SRC_INCLUDE_UTILITY_MIXEDRADIX_H_

#include <cstddef>
#include <iterator>
#include <vector>

namespace chordnovarw::utility {

/**
 * \brief Mixed-radix counter that generates all mutation vectors in
 *        [-vl_max, vl_max] per voice, with optional dead zone (-vl_min, vl_min).
 *
 * Replaces `Chord::next()` from legacy `chord.cpp`. Iterates through all
 * possible voice movement vectors for a chord of a given width (number of voices).
 *
 * When `vl_min == 0`, each voice has `2*vl_max + 1` choices.
 * When `vl_min > 0`, values in the open interval `(-vl_min, vl_min)` are
 * skipped (i.e., each voice has `2*(vl_max - vl_min + 1)` choices).
 *
 * Iteration order: least-significant-digit first (voice 0 changes fastest),
 * matching the legacy `Chord::next()` behavior.
 *
 * Usage:
 * \code
 *   for (const auto& vec : MixedRadixRange(4, 3, 0)) {
 *     // vec is a vector<int> of width 3, each element in [-4, 4]
 *   }
 * \endcode
 */
class MixedRadixIterator {
public:
  using iterator_category = std::input_iterator_tag;
  using value_type = std::vector<int>;
  using difference_type = std::ptrdiff_t;
  using pointer = const value_type*;
  using reference = const value_type&;

  /**
   * \brief Construct an iterator at position 0 (all values = -vl_max or -vl_min for dead zone).
   */
  MixedRadixIterator(int vl_max, int width, int vl_min)
      : _vl_max(vl_max), _vl_min(vl_min), _width(width), _done(false) {
    _vec.resize(width, start_value());
  }

  /**
   * \brief Construct a sentinel (end) iterator.
   */
  MixedRadixIterator() : _vl_max(0), _vl_min(0), _width(0), _done(true) {}

  reference operator*() const { return _vec; }
  pointer operator->() const { return &_vec; }

  MixedRadixIterator& operator++() {
    advance();
    return *this;
  }

  MixedRadixIterator operator++(int) {
    auto copy = *this;
    advance();
    return copy;
  }

  bool operator==(const MixedRadixIterator& other) const {
    if (_done && other._done) return true;
    if (_done != other._done) return false;
    return _vec == other._vec;
  }

  bool operator!=(const MixedRadixIterator& other) const {
    return !(*this == other);
  }

private:
  int _vl_max;
  int _vl_min;
  int _width;
  bool _done;
  std::vector<int> _vec;

  [[nodiscard]] int start_value() const {
    return -_vl_max;
  }

  /**
   * \brief Returns the next valid value after \p val, or std::nullopt if overflow.
   */
  [[nodiscard]] int next_value(int val) const {
    if (_vl_min != 0 && val == -_vl_min) {
      // Skip dead zone: jump from -vl_min to vl_min
      return _vl_min;
    }
    return val + 1;
  }

  void advance() {
    int index = 0;
    while (index < _width) {
      if (_vec[index] == _vl_max) {
        _vec[index] = start_value();
        ++index;
      } else {
        _vec[index] = next_value(_vec[index]);
        return;
      }
    }
    _done = true; // All digits overflowed
  }
};

/**
 * \brief Range object for mixed-radix iteration.
 *
 * \param vl_max  Maximum absolute voice movement per voice.
 * \param width   Number of voices (dimensionality).
 * \param vl_min  Dead zone size (0 = no dead zone). Values in (-vl_min, vl_min)
 *                exclusive are skipped.
 */
class MixedRadixRange {
public:
  MixedRadixRange(int vl_max, int width, int vl_min = 0)
      : _vl_max(vl_max), _width(width), _vl_min(vl_min) {}

  [[nodiscard]] MixedRadixIterator begin() const {
    return MixedRadixIterator(_vl_max, _width, _vl_min);
  }

  [[nodiscard]] MixedRadixIterator end() const {
    return MixedRadixIterator(); // sentinel
  }

  /**
   * \brief Total number of vectors this range will produce.
   */
  [[nodiscard]] long long total_count() const {
    int choice;
    if (_vl_min == 0) {
      choice = 2 * _vl_max + 1;
    } else {
      choice = 2 * (_vl_max - _vl_min + 1);
    }
    long long count = 1;
    for (int i = 0; i < _width; ++i)
      count *= choice;
    return count;
  }

private:
  int _vl_max;
  int _width;
  int _vl_min;
};

} // namespace chordnovarw::utility

#endif // CHORDNOVARW_SRC_INCLUDE_UTILITY_MIXEDRADIX_H_
