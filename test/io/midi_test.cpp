#include <gtest/gtest.h>
#include "io/midi.h"
#include "model/orderedchord.h"
#include "model/pitch.h"
#include <fstream>
#include <filesystem>
#include <cstdint>

using namespace chordnovarw::io;
using namespace chordnovarw::model;

namespace {

// Helper: read entire file into a byte vector
std::vector<uint8_t> read_file_bytes(const std::string& path) {
  std::ifstream in(path, std::ios::binary);
  return {std::istreambuf_iterator<char>(in),
          std::istreambuf_iterator<char>()};
}

// Helper: read big-endian 16-bit from buffer
uint16_t read_be16(const uint8_t* p) {
  return static_cast<uint16_t>((p[0] << 8) | p[1]);
}

// Helper: read big-endian 32-bit from buffer
uint32_t read_be32(const uint8_t* p) {
  return (static_cast<uint32_t>(p[0]) << 24) |
         (static_cast<uint32_t>(p[1]) << 16) |
         (static_cast<uint32_t>(p[2]) << 8) |
          static_cast<uint32_t>(p[3]);
}

// Helper: create a C major triad (C4 E4 G4)
OrderedChord make_c_major() {
  return OrderedChord({Pitch(60), Pitch(64), Pitch(67)});
}

// Helper: create an F major triad (F4 A4 C5)
OrderedChord make_f_major() {
  return OrderedChord({Pitch(65), Pitch(69), Pitch(72)});
}

std::string temp_midi_path() {
  auto tmp = std::filesystem::temp_directory_path() / "chord_nova_test.mid";
  return tmp.string();
}

} // anonymous namespace

class MidiWriteTest : public ::testing::Test {
protected:
  std::string path;

  void SetUp() override {
    path = temp_midi_path();
  }

  void TearDown() override {
    std::filesystem::remove(path);
  }
};

TEST_F(MidiWriteTest, WriteMidiCreatesFile) {
  std::vector<OrderedChord> chords = {make_c_major(), make_f_major()};
  ASSERT_TRUE(write_midi(path, chords));
  EXPECT_TRUE(std::filesystem::exists(path));
  EXPECT_GT(std::filesystem::file_size(path), 0u);
}

TEST_F(MidiWriteTest, WriteMidiHeaderValid) {
  std::vector<OrderedChord> chords = {make_c_major()};
  ASSERT_TRUE(write_midi(path, chords));

  auto data = read_file_bytes(path);
  ASSERT_GE(data.size(), 14u);  // MThd(4) + len(4) + format(2) + tracks(2) + tpq(2)

  // MThd chunk
  EXPECT_EQ(data[0], 'M');
  EXPECT_EQ(data[1], 'T');
  EXPECT_EQ(data[2], 'h');
  EXPECT_EQ(data[3], 'd');

  // Header length = 6
  EXPECT_EQ(read_be32(&data[4]), 6u);

  // Format 0
  EXPECT_EQ(read_be16(&data[8]), 0u);

  // 1 track
  EXPECT_EQ(read_be16(&data[10]), 1u);

  // Default ticks per quarter = 480
  EXPECT_EQ(read_be16(&data[12]), 480u);
}

TEST_F(MidiWriteTest, WriteMidiTrackChunkPresent) {
  std::vector<OrderedChord> chords = {make_c_major()};
  ASSERT_TRUE(write_midi(path, chords));

  auto data = read_file_bytes(path);
  ASSERT_GE(data.size(), 22u);  // Header(14) + MTrk(4) + len(4)

  // MTrk at offset 14
  EXPECT_EQ(data[14], 'M');
  EXPECT_EQ(data[15], 'T');
  EXPECT_EQ(data[16], 'r');
  EXPECT_EQ(data[17], 'k');

  // Track length should be > 0
  uint32_t track_len = read_be32(&data[18]);
  EXPECT_GT(track_len, 0u);

  // Total file size should match header + track header + track data
  EXPECT_EQ(data.size(), 14u + 8u + track_len);
}

TEST_F(MidiWriteTest, WriteMidiContainsNoteEvents) {
  std::vector<OrderedChord> chords = {make_c_major()};
  ASSERT_TRUE(write_midi(path, chords));

  auto data = read_file_bytes(path);

  // Search for Note On events (0x90 followed by pitch)
  bool found_note_on_60 = false;
  bool found_note_on_64 = false;
  bool found_note_on_67 = false;
  for (size_t i = 22; i + 2 < data.size(); ++i) {
    if (data[i] == 0x90) {
      if (data[i + 1] == 60) found_note_on_60 = true;
      if (data[i + 1] == 64) found_note_on_64 = true;
      if (data[i + 1] == 67) found_note_on_67 = true;
    }
  }
  EXPECT_TRUE(found_note_on_60) << "Missing Note On for C4 (60)";
  EXPECT_TRUE(found_note_on_64) << "Missing Note On for E4 (64)";
  EXPECT_TRUE(found_note_on_67) << "Missing Note On for G4 (67)";

  // Search for Note Off events (0x80)
  bool found_note_off_60 = false;
  bool found_note_off_64 = false;
  bool found_note_off_67 = false;
  for (size_t i = 22; i + 2 < data.size(); ++i) {
    if (data[i] == 0x80) {
      if (data[i + 1] == 60) found_note_off_60 = true;
      if (data[i + 1] == 64) found_note_off_64 = true;
      if (data[i + 1] == 67) found_note_off_67 = true;
    }
  }
  EXPECT_TRUE(found_note_off_60) << "Missing Note Off for C4 (60)";
  EXPECT_TRUE(found_note_off_64) << "Missing Note Off for E4 (64)";
  EXPECT_TRUE(found_note_off_67) << "Missing Note Off for G4 (67)";
}

TEST_F(MidiWriteTest, WriteMidiEndsWithEOT) {
  std::vector<OrderedChord> chords = {make_c_major()};
  ASSERT_TRUE(write_midi(path, chords));

  auto data = read_file_bytes(path);
  size_t n = data.size();
  ASSERT_GE(n, 4u);

  // End of Track: 00 FF 2F 00
  EXPECT_EQ(data[n - 4], 0x00);
  EXPECT_EQ(data[n - 3], 0xFF);
  EXPECT_EQ(data[n - 2], 0x2F);
  EXPECT_EQ(data[n - 1], 0x00);
}

TEST_F(MidiWriteTest, WriteMidiCustomConfig) {
  std::vector<OrderedChord> chords = {make_c_major()};
  MidiConfig config;
  config.ticks_per_quarter = 240;
  config.tempo_bpm = 120;
  ASSERT_TRUE(write_midi(path, chords, config));

  auto data = read_file_bytes(path);
  ASSERT_GE(data.size(), 14u);

  // ticks_per_quarter = 240
  EXPECT_EQ(read_be16(&data[12]), 240u);
}

TEST_F(MidiWriteTest, WriteMidiMultipleChords) {
  std::vector<OrderedChord> chords = {make_c_major(), make_f_major()};
  ASSERT_TRUE(write_midi(path, chords));

  auto data = read_file_bytes(path);

  // Count Note On events (0x90)
  int note_on_count = 0;
  for (size_t i = 22; i + 2 < data.size(); ++i) {
    if (data[i] == 0x90) note_on_count++;
  }
  // 3 notes per chord * 2 chords = 6
  EXPECT_EQ(note_on_count, 6);
}

TEST_F(MidiWriteTest, WriteMidiInvalidPathFails) {
  std::vector<OrderedChord> chords = {make_c_major()};
  EXPECT_FALSE(write_midi("/nonexistent/dir/file.mid", chords));
}

// ── write_midi_single() tests ───────────────────────────────────

TEST_F(MidiWriteTest, WriteMidiSingleNoInterlace) {
  OrderedChord initial = make_c_major();
  std::vector<OrderedChord> candidates = {make_f_major()};
  MidiConfig config;
  config.interlace = false;
  ASSERT_TRUE(write_midi_single(path, initial, candidates, config));

  auto data = read_file_bytes(path);

  // Count Note On events: initial(3) + 1 candidate(3) = 6
  int note_on_count = 0;
  for (size_t i = 22; i + 2 < data.size(); ++i) {
    if (data[i] == 0x90) note_on_count++;
  }
  EXPECT_EQ(note_on_count, 6);
}

TEST_F(MidiWriteTest, WriteMidiSingleInterlace) {
  OrderedChord initial = make_c_major();
  std::vector<OrderedChord> candidates = {make_f_major(), make_c_major()};
  MidiConfig config;
  config.interlace = true;
  ASSERT_TRUE(write_midi_single(path, initial, candidates, config));

  auto data = read_file_bytes(path);

  // Count Note On events: 2 candidates * (initial(3) + candidate(3)) = 12
  int note_on_count = 0;
  for (size_t i = 22; i + 2 < data.size(); ++i) {
    if (data[i] == 0x90) note_on_count++;
  }
  EXPECT_EQ(note_on_count, 12);
}

TEST_F(MidiWriteTest, WriteMidiTempoMetaEvent) {
  std::vector<OrderedChord> chords = {make_c_major()};
  MidiConfig config;
  config.tempo_bpm = 120;
  ASSERT_TRUE(write_midi(path, chords, config));

  auto data = read_file_bytes(path);

  // Search for tempo meta event: FF 51 03
  bool found_tempo = false;
  for (size_t i = 22; i + 5 < data.size(); ++i) {
    if (data[i] == 0xFF && data[i + 1] == 0x51 && data[i + 2] == 0x03) {
      // 120 BPM = 500000 us/beat = 0x07A120
      uint32_t us_per_beat = (static_cast<uint32_t>(data[i + 3]) << 16) |
                             (static_cast<uint32_t>(data[i + 4]) << 8) |
                              static_cast<uint32_t>(data[i + 5]);
      EXPECT_EQ(us_per_beat, 500000u);
      found_tempo = true;
      break;
    }
  }
  EXPECT_TRUE(found_tempo) << "Tempo meta event not found";
}
