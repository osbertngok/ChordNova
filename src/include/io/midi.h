#ifndef CHORDNOVARW_SRC_INCLUDE_IO_MIDI_H_
#define CHORDNOVARW_SRC_INCLUDE_IO_MIDI_H_

#include "model/orderedchord.h"
#include <string>
#include <vector>

namespace chordnovarw::io {

/**
 * \brief Configuration for MIDI output.
 */
struct MidiConfig {
  int ticks_per_quarter = 480;   ///< Ticks per quarter note.
  int tempo_bpm = 60;            ///< Beats per minute.
  int note_on_velocity = 80;     ///< Note On velocity (0-127).
  int note_off_velocity = 64;    ///< Note Off velocity (0-127).
  int beat_duration = 1;         ///< Duration per chord in beats.
  bool interlace = false;        ///< Interleave original chord with each candidate.
};

/**
 * \brief Write a sequence of chords as a Standard MIDI File (Format 0).
 *
 * Writes a single-track MIDI file with each chord played as a block chord
 * lasting `config.beat_duration` beats.
 *
 * Legacy: `Chord::to_midi()` in `chord.cpp`.
 *
 * \param path    Output file path.
 * \param chords  Sequence of chords to write.
 * \param config  MIDI output configuration.
 * \return true if the file was written successfully.
 */
[[nodiscard]] bool write_midi(
    const std::string& path,
    const std::vector<model::OrderedChord>& chords,
    const MidiConfig& config = MidiConfig{});

/**
 * \brief Write single-mode results: initial chord followed by all candidates.
 *
 * If interlace is true, writes: initial, cand1, initial, cand2, ...
 * Otherwise writes: initial, cand1, cand2, ...
 *
 * \param path       Output file path.
 * \param initial    The initial/reference chord.
 * \param candidates All candidate chords.
 * \param config     MIDI output configuration.
 * \return true if the file was written successfully.
 */
[[nodiscard]] bool write_midi_single(
    const std::string& path,
    const model::OrderedChord& initial,
    const std::vector<model::OrderedChord>& candidates,
    const MidiConfig& config = MidiConfig{});

} // namespace chordnovarw::io

#endif // CHORDNOVARW_SRC_INCLUDE_IO_MIDI_H_
