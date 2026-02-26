#include "io/noteparser.h"
#include "model/pitch.h"
#include <algorithm>
#include <charconv>
#include <cctype>
#include <cmath>
#include <sstream>
#include <vector>

namespace chordnovarw::io {

using namespace chordnovarw::model;

std::optional<uint8_t> nametonum(const std::string& token) {
  if (token.empty()) return std::nullopt;

  // Check if it's a pure MIDI number
  if (std::isdigit(static_cast<unsigned char>(token[0]))) {
    int val = 0;
    auto [ptr, ec] = std::from_chars(token.data(), token.data() + token.size(), val);
    if (ec != std::errc{} || ptr != token.data() + token.size())
      return std::nullopt;
    if (val < 0 || val > 127) return std::nullopt;
    return static_cast<uint8_t>(val);
  }

  // Path 1: "Letter[accidental][octave]" — e.g. C#4, Bb3, G
  {
    int val = 0;
    size_t pos = 0;
    char ch = std::toupper(static_cast<unsigned char>(token[0]));
    switch (ch) {
      case 'C': val = 12; break;
      case 'D': val = 14; break;
      case 'E': val = 16; break;
      case 'F': val = 17; break;
      case 'G': val = 19; break;
      case 'A': val = 21; break;
      case 'B': val = 23; break;
      default: val = -1;
    }
    if (val >= 0) {
      ++pos;
      if (pos < token.size()) {
        if (token[pos] == '#') { val += 1; ++pos; }
        else if (token[pos] == 'b') { val -= 1; ++pos; }
      }
      int octave = 4;  // default octave
      bool has_octave = false;
      if (pos < token.size()) {
        if (token[pos] == '-' && pos + 1 < token.size() &&
            std::isdigit(static_cast<unsigned char>(token[pos + 1]))) {
          octave = -(token[pos + 1] - '0');
          pos += 2;
          has_octave = true;
        } else if (std::isdigit(static_cast<unsigned char>(token[pos]))) {
          octave = token[pos] - '0';
          ++pos;
          has_octave = true;
        }
      }
      // Check that there's nothing unexpected remaining
      if (pos == token.size() || (!has_octave && pos == token.size())) {
        int result = val + 12 * octave;
        if (result >= 0 && result <= 127) return static_cast<uint8_t>(result);
      }
    }
  }

  // Path 2: "[accidental]Letter[octave]" — e.g. bB4, #C3
  {
    int val = 0;
    size_t pos = 0;
    if (token[0] == '#') { val += 1; ++pos; }
    else if (token[0] == 'b') { val -= 1; ++pos; }
    else return std::nullopt;

    if (pos >= token.size()) return std::nullopt;
    char ch = std::toupper(static_cast<unsigned char>(token[pos]));
    switch (ch) {
      case 'C': val += 12; break;
      case 'D': val += 14; break;
      case 'E': val += 16; break;
      case 'F': val += 17; break;
      case 'G': val += 19; break;
      case 'A': val += 21; break;
      case 'B': val += 23; break;
      default: return std::nullopt;
    }
    ++pos;
    int octave = 4;
    if (pos < token.size()) {
      if (token[pos] == '-' && pos + 1 < token.size() &&
          std::isdigit(static_cast<unsigned char>(token[pos + 1]))) {
        octave = -(token[pos + 1] - '0');
        pos += 2;
      } else if (std::isdigit(static_cast<unsigned char>(token[pos]))) {
        octave = token[pos] - '0';
        ++pos;
      }
    }
    if (pos == token.size()) {
      int result = val + 12 * octave;
      if (result >= 0 && result <= 127) return static_cast<uint8_t>(result);
    }
  }

  return std::nullopt;
}

std::optional<OrderedChord> parse_notes(const std::string& input) {
  if (input.empty() || input.size() >= 500) return std::nullopt;

  // Tokenize
  std::istringstream iss(input);
  std::vector<std::string> tokens;
  std::string tok;
  while (iss >> tok) tokens.push_back(tok);
  if (tokens.empty()) return std::nullopt;

  // Parse each token
  std::vector<int> midi_notes;
  bool no_octave = true;
  for (const auto& t : tokens) {
    auto note = nametonum(t);
    if (!note) return std::nullopt;
    midi_notes.push_back(*note);

    // Check if any token has an octave number at the end
    if (!t.empty() && std::isdigit(static_cast<unsigned char>(t.back())))
      no_octave = false;
    // Also check for MIDI number input
    if (!t.empty() && std::isdigit(static_cast<unsigned char>(t[0])))
      no_octave = false;
  }

  if (no_octave) {
    // Auto-assign octaves: fold notes down to maintain ascending order,
    // then center in valid MIDI range
    int sz = static_cast<int>(midi_notes.size());
    for (int i = sz - 1; i > 0; --i) {
      if (midi_notes[i - 1] > midi_notes[i]) {
        int octave_diff = (midi_notes[i - 1] - midi_notes[i]) / 12;
        midi_notes[i - 1] -= (octave_diff + 1) * 12;
      }
    }
    int octave_h = (127 - midi_notes[sz - 1]) / 12;
    int octave_l = static_cast<int>(std::floor(midi_notes[0] / 12.0));
    if (octave_h + octave_l < 0) return std::nullopt;
    int octave_shift = (octave_h - octave_l) / 2;
    for (int& note : midi_notes) note += octave_shift * 12;
  } else {
    // Sort and remove exact duplicates
    std::sort(midi_notes.begin(), midi_notes.end());
    midi_notes.erase(
        std::unique(midi_notes.begin(), midi_notes.end()),
        midi_notes.end());
  }

  // Convert to Pitch vector
  std::vector<Pitch> pitches;
  pitches.reserve(midi_notes.size());
  for (int m : midi_notes) {
    if (m < 0 || m > 127) return std::nullopt;
    pitches.push_back(Pitch(static_cast<uint8_t>(m)));
  }

  return OrderedChord(std::move(pitches));
}

} // namespace chordnovarw::io
