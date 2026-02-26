#ifndef CHORDNOVARW_SRC_INCLUDE_IO_NOTEPARSER_H_
#define CHORDNOVARW_SRC_INCLUDE_IO_NOTEPARSER_H_

#include "model/orderedchord.h"
#include <optional>
#include <string>

namespace chordnovarw::io {

/**
 * \brief Parse a space-separated string of note names or MIDI numbers into an OrderedChord.
 *
 * Supports three input formats:
 * 1. MIDI numbers: "60 64 67"
 * 2. Named notes with octave: "C4 E4 G4", "B3 D#4 F#4 A4"
 * 3. Named notes without octave: "C E G" — auto-assigns octaves to keep ascending order
 *
 * For named notes, supports both "Letter[accidental][octave]" and "[accidental]Letter[octave]"
 * formats. Accidentals: '#' for sharp, 'b' for flat. When octave is omitted,
 * notes are placed in a mid-range octave while maintaining ascending order.
 *
 * Legacy: `Chord::set_notes_from_text()` + `nametonum()` in `chord.cpp` / `functions.cpp`.
 *
 * \param input Space-separated string of note tokens.
 * \return OrderedChord if parsing succeeded, std::nullopt on error.
 */
[[nodiscard]] std::optional<model::OrderedChord> parse_notes(const std::string& input);

/**
 * \brief Convert a single note token to a MIDI number.
 *
 * Supports MIDI number strings ("60"), named notes ("C4", "D#3", "Bb2"),
 * and accidental-first format ("#C4", "bB3").
 *
 * \param token A single note token string.
 * \return MIDI note number 0-127, or -1 on failure.
 */
[[nodiscard]] int nametonum(const std::string& token);

} // namespace chordnovarw::io

#endif // CHORDNOVARW_SRC_INCLUDE_IO_NOTEPARSER_H_
