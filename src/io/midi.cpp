#include "io/midi.h"
#include "utility/midi_encoding.h"
#include <fstream>

namespace chordnovarw::io {

using namespace chordnovarw::utility;

namespace {

// Write big-endian 4-byte integer
void write_be32(std::ofstream& out, int value) {
  int be = swap_int(value, 4);
  out.write(reinterpret_cast<const char*>(&be), 4);
}

// Write big-endian 2-byte integer
void write_be16(std::ofstream& out, int value) {
  int be = swap_int(value, 2);
  out.write(reinterpret_cast<const char*>(&be), 2);
}

// Write MIDI file header (MThd)
void write_header(std::ofstream& out, int format, int num_tracks, int ticks_per_quarter) {
  out.write("MThd", 4);
  write_be32(out, 6);  // header length
  write_be16(out, format);
  write_be16(out, num_tracks);
  write_be16(out, ticks_per_quarter);
}

// Write track header (MTrk + length placeholder, returns position for length fixup)
std::streampos write_track_header(std::ofstream& out) {
  out.write("MTrk", 4);
  auto len_pos = out.tellp();
  write_be32(out, 0);  // placeholder for track length
  return len_pos;
}

// Fix up track length at previously saved position
void fixup_track_length(std::ofstream& out, std::streampos len_pos) {
  auto end_pos = out.tellp();
  int track_len = static_cast<int>(end_pos - len_pos - 4);
  out.seekp(len_pos);
  write_be32(out, track_len);
  out.seekp(end_pos);
}

// Write track preamble meta events (copyright, instrument, tempo, time sig, key sig, program change)
void write_track_preamble(std::ofstream& out, int tempo_bpm) {
  // Copyright text
  const char* copyright = "(c) 2020 Wenge Chen, Ji-woon Sim.";
  int copy_len = 33;
  out.write("\x00\xff\x02", 3);
  out.put(static_cast<char>(copy_len));
  out.write(copyright, copy_len);

  // Instrument name: "Piano"
  out.write("\x00\xff\x04\x05Piano", 9);

  // Tempo: microseconds per beat
  int us_per_beat = 60000000 / tempo_bpm;
  out.write("\x00\xff\x51\x03", 4);
  out.put(static_cast<char>((us_per_beat >> 16) & 0xFF));
  out.put(static_cast<char>((us_per_beat >> 8) & 0xFF));
  out.put(static_cast<char>(us_per_beat & 0xFF));

  // Time signature: 4/4
  out.write("\x00\xff\x58\x04\x04\x02\x18\x08", 8);

  // Key signature: C major
  out.write("\x00\xff\x59\x02\x00\x00", 6);

  // Program change: channel 0, patch 0 (piano)
  out.write("\x00\xc0\x00", 3);
}

// Write a single chord as MIDI events
void write_chord_events(std::ofstream& out, const model::OrderedChord& chord,
                        int beat, int ticks_per_quarter,
                        int note_on_vel, int note_off_vel) {
  auto pitches = chord.get_pitches();
  int size = static_cast<int>(pitches.size());

  // Note On events (all at delta=0)
  for (int i = 0; i < size; ++i) {
    out.put('\x00');     // delta time = 0
    out.put('\x90');     // Note On, channel 0
    out.put(static_cast<char>(pitches[i].get_number()));
    out.put(static_cast<char>(note_on_vel));
  }

  // Note Off events
  int ticks = beat * ticks_per_quarter;
  for (int i = 0; i < size; ++i) {
    if (i == 0) {
      auto vlq = to_vlq(ticks);
      out.write(reinterpret_cast<const char*>(vlq.data()),
                static_cast<std::streamsize>(vlq.size()));
    } else {
      out.put('\x00');   // delta time = 0 for subsequent notes
    }
    out.put('\x80');     // Note Off, channel 0
    out.put(static_cast<char>(pitches[i].get_number()));
    out.put(static_cast<char>(note_off_vel));
  }
}

// Write end-of-track meta event
void write_end_of_track(std::ofstream& out) {
  out.write("\x00\xff\x2f\x00", 4);
}

} // anonymous namespace

bool write_midi(
    const std::string& path,
    const std::vector<model::OrderedChord>& chords,
    const MidiConfig& config) {

  std::ofstream out(path, std::ios::binary);
  if (!out.is_open()) return false;

  write_header(out, 0, 1, config.ticks_per_quarter);

  auto len_pos = write_track_header(out);
  write_track_preamble(out, config.tempo_bpm);

  for (const auto& chord : chords) {
    write_chord_events(out, chord, config.beat_duration,
                       config.ticks_per_quarter,
                       config.note_on_velocity, config.note_off_velocity);
  }

  write_end_of_track(out);
  fixup_track_length(out, len_pos);

  out.close();
  return true;
}

bool write_midi_single(
    const std::string& path,
    const model::OrderedChord& initial,
    const std::vector<model::OrderedChord>& candidates,
    const MidiConfig& config) {

  std::ofstream out(path, std::ios::binary);
  if (!out.is_open()) return false;

  write_header(out, 0, 1, config.ticks_per_quarter);

  auto len_pos = write_track_header(out);
  write_track_preamble(out, config.tempo_bpm);

  if (config.interlace) {
    for (const auto& cand : candidates) {
      write_chord_events(out, initial, config.beat_duration,
                         config.ticks_per_quarter,
                         config.note_on_velocity, config.note_off_velocity);
      write_chord_events(out, cand, config.beat_duration,
                         config.ticks_per_quarter,
                         config.note_on_velocity, config.note_off_velocity);
    }
  } else {
    write_chord_events(out, initial, config.beat_duration,
                       config.ticks_per_quarter,
                       config.note_on_velocity, config.note_off_velocity);
    for (const auto& cand : candidates) {
      write_chord_events(out, cand, config.beat_duration,
                         config.ticks_per_quarter,
                         config.note_on_velocity, config.note_off_velocity);
    }
  }

  write_end_of_track(out);
  fixup_track_length(out, len_pos);

  out.close();
  return true;
}

} // namespace chordnovarw::io
