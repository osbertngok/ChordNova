#ifndef CHORDNOVARW_SRC_INCLUDE_UTILITY_MIDI_ENCODING_H_
#define CHORDNOVARW_SRC_INCLUDE_UTILITY_MIDI_ENCODING_H_

#include <cstdint>
#include <vector>

namespace chordnovarw::utility {

/**
 * \brief Swap bytes of an integer for big-endian MIDI format.
 *
 * Legacy: `swapInt()` in `functions.cpp`.
 *
 * \param value The integer to swap.
 * \param len   Number of bytes (1-4).
 * \return The byte-swapped value.
 */
[[nodiscard]] constexpr int swap_int(int value, int len = 4) noexcept {
  switch (len) {
    case 1: return value;
    case 2: return ((value & 0x00FF) << 8) | ((value & 0xFF00) >> 8);
    case 3: return ((value & 0x0000FF) << 16) | (value & 0x00FF00) |
                   ((value & 0xFF0000) >> 16);
    case 4: return (static_cast<int>(
                   (static_cast<unsigned>(value) & 0x000000FFu) << 24) |
                   ((value & 0x0000FF00) << 8) |
                   ((value & 0x00FF0000) >> 8) |
                   static_cast<int>(
                   (static_cast<unsigned>(value) & 0xFF000000u) >> 24));
    default: return value;
  }
}

/**
 * \brief Encode an integer as a MIDI Variable-Length Quantity (VLQ).
 *
 * Returns the VLQ bytes as a vector. Each byte uses 7 data bits;
 * bit 7 is set on all bytes except the last.
 *
 * Legacy: `to_VLQ()` in `functions.cpp`.
 *
 * \param value Non-negative integer to encode.
 * \return Vector of VLQ bytes (1-4 bytes).
 */
[[nodiscard]] inline std::vector<uint8_t> to_vlq(int value) {
  if (value == 0) return {0};

  std::vector<uint8_t> bytes;
  while (value > 0) {
    bytes.push_back(static_cast<uint8_t>(value & 0x7F));
    value >>= 7;
  }
  // Set continuation bit on all bytes except the last (which is bytes[0])
  for (size_t i = 1; i < bytes.size(); ++i)
    bytes[i] |= 0x80;
  // Reverse to get MSB first
  std::reverse(bytes.begin(), bytes.end());
  return bytes;
}

} // namespace chordnovarw::utility

#endif // CHORDNOVARW_SRC_INCLUDE_UTILITY_MIDI_ENCODING_H_
