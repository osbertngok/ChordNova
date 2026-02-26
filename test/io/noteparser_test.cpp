#include <gtest/gtest.h>
#include "io/noteparser.h"

using namespace chordnovarw::io;

// ── nametonum() tests ────────────────────────────────────────────

TEST(NoteParserTest, NameToNumMidiNumber) {
  EXPECT_EQ(nametonum("60"), std::optional<uint8_t>(60));
  EXPECT_EQ(nametonum("0"), std::optional<uint8_t>(0));
  EXPECT_EQ(nametonum("127"), std::optional<uint8_t>(127));
  EXPECT_EQ(nametonum("128"), std::nullopt);  // Out of range
}

TEST(NoteParserTest, NameToNumLetterOctave) {
  EXPECT_EQ(nametonum("C4"), std::optional<uint8_t>(60));
  EXPECT_EQ(nametonum("A4"), std::optional<uint8_t>(69));
  EXPECT_EQ(nametonum("B3"), std::optional<uint8_t>(59));
  EXPECT_EQ(nametonum("C0"), std::optional<uint8_t>(12));
}

TEST(NoteParserTest, NameToNumSharp) {
  EXPECT_EQ(nametonum("C#4"), std::optional<uint8_t>(61));
  EXPECT_EQ(nametonum("F#4"), std::optional<uint8_t>(66));
  EXPECT_EQ(nametonum("D#4"), std::optional<uint8_t>(63));
}

TEST(NoteParserTest, NameToNumFlat) {
  EXPECT_EQ(nametonum("Bb3"), std::optional<uint8_t>(58));
  EXPECT_EQ(nametonum("Eb4"), std::optional<uint8_t>(63));
  EXPECT_EQ(nametonum("Ab4"), std::optional<uint8_t>(68));
}

TEST(NoteParserTest, NameToNumAccidentalFirst) {
  EXPECT_EQ(nametonum("bB3"), std::optional<uint8_t>(58));
  EXPECT_EQ(nametonum("#C4"), std::optional<uint8_t>(61));
  EXPECT_EQ(nametonum("#F4"), std::optional<uint8_t>(66));
}

TEST(NoteParserTest, NameToNumNoOctave) {
  // No octave defaults to 4
  EXPECT_EQ(nametonum("C"), std::optional<uint8_t>(60));
  EXPECT_EQ(nametonum("E"), std::optional<uint8_t>(64));
  EXPECT_EQ(nametonum("G"), std::optional<uint8_t>(67));
}

TEST(NoteParserTest, NameToNumLowercase) {
  EXPECT_EQ(nametonum("c4"), std::optional<uint8_t>(60));
  EXPECT_EQ(nametonum("e4"), std::optional<uint8_t>(64));
}

TEST(NoteParserTest, NameToNumInvalid) {
  EXPECT_EQ(nametonum(""), std::nullopt);
  EXPECT_EQ(nametonum("X"), std::nullopt);
  EXPECT_EQ(nametonum("C-2"), std::nullopt);  // C at octave -2 would be negative
}

// ── parse_notes() tests ──────────────────────────────────────────

// Port of legacy maintest/parse_notes.cpp test 1
TEST(NoteParserTest, ParseCMajorTriad) {
  auto result = parse_notes("C4 E4 G4");
  ASSERT_TRUE(result.has_value());
  auto pitches = result->get_pitches();
  ASSERT_EQ(pitches.size(), 3u);
  EXPECT_EQ(pitches[0].get_number(), 60);
  EXPECT_EQ(pitches[1].get_number(), 64);
  EXPECT_EQ(pitches[2].get_number(), 67);
}

// Port of legacy maintest/parse_notes.cpp test 2
TEST(NoteParserTest, ParseBDom7) {
  auto result = parse_notes("B3 D#4 F#4 A4");
  ASSERT_TRUE(result.has_value());
  auto pitches = result->get_pitches();
  ASSERT_EQ(pitches.size(), 4u);
  EXPECT_EQ(pitches[0].get_number(), 59);
  EXPECT_EQ(pitches[1].get_number(), 63);
  EXPECT_EQ(pitches[2].get_number(), 66);
  EXPECT_EQ(pitches[3].get_number(), 69);
}

TEST(NoteParserTest, ParseMidiNumbers) {
  auto result = parse_notes("60 64 67");
  ASSERT_TRUE(result.has_value());
  auto pitches = result->get_pitches();
  ASSERT_EQ(pitches.size(), 3u);
  EXPECT_EQ(pitches[0].get_number(), 60);
  EXPECT_EQ(pitches[1].get_number(), 64);
  EXPECT_EQ(pitches[2].get_number(), 67);
}

TEST(NoteParserTest, ParseNoOctaveMode) {
  auto result = parse_notes("C E G");
  ASSERT_TRUE(result.has_value());
  auto pitches = result->get_pitches();
  ASSERT_EQ(pitches.size(), 3u);
  // Should be ascending and in valid MIDI range
  EXPECT_LT(pitches[0].get_number(), pitches[1].get_number());
  EXPECT_LT(pitches[1].get_number(), pitches[2].get_number());
  EXPECT_GE(pitches[0].get_number(), 0);
  EXPECT_LE(pitches[2].get_number(), 127);
}

TEST(NoteParserTest, ParseEmpty) {
  EXPECT_FALSE(parse_notes("").has_value());
}

TEST(NoteParserTest, ParseInvalid) {
  EXPECT_FALSE(parse_notes("XYZ").has_value());
}

TEST(NoteParserTest, ParseUnsortedSorts) {
  auto result = parse_notes("G4 C4 E4");
  ASSERT_TRUE(result.has_value());
  auto pitches = result->get_pitches();
  ASSERT_EQ(pitches.size(), 3u);
  EXPECT_EQ(pitches[0].get_number(), 60);
  EXPECT_EQ(pitches[1].get_number(), 64);
  EXPECT_EQ(pitches[2].get_number(), 67);
}

TEST(NoteParserTest, ParseDuplicatesRemoved) {
  auto result = parse_notes("C4 C4 E4 G4");
  ASSERT_TRUE(result.has_value());
  auto pitches = result->get_pitches();
  ASSERT_EQ(pitches.size(), 3u);
}
