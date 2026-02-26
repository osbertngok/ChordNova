#include <gtest/gtest.h>
#include "io/noteparser.h"

using namespace chordnovarw::io;

// ── nametonum() tests ────────────────────────────────────────────

TEST(NoteParserTest, NameToNumMidiNumber) {
  EXPECT_EQ(nametonum("60"), 60);
  EXPECT_EQ(nametonum("0"), 0);
  EXPECT_EQ(nametonum("127"), 127);
  EXPECT_EQ(nametonum("128"), -1);  // Out of range
}

TEST(NoteParserTest, NameToNumLetterOctave) {
  EXPECT_EQ(nametonum("C4"), 60);
  EXPECT_EQ(nametonum("A4"), 69);
  EXPECT_EQ(nametonum("B3"), 59);
  EXPECT_EQ(nametonum("C0"), 12);
}

TEST(NoteParserTest, NameToNumSharp) {
  EXPECT_EQ(nametonum("C#4"), 61);
  EXPECT_EQ(nametonum("F#4"), 66);
  EXPECT_EQ(nametonum("D#4"), 63);
}

TEST(NoteParserTest, NameToNumFlat) {
  EXPECT_EQ(nametonum("Bb3"), 58);
  EXPECT_EQ(nametonum("Eb4"), 63);
  EXPECT_EQ(nametonum("Ab4"), 68);
}

TEST(NoteParserTest, NameToNumAccidentalFirst) {
  EXPECT_EQ(nametonum("bB3"), 58);
  EXPECT_EQ(nametonum("#C4"), 61);
  EXPECT_EQ(nametonum("#F4"), 66);
}

TEST(NoteParserTest, NameToNumNoOctave) {
  // No octave defaults to 4
  EXPECT_EQ(nametonum("C"), 60);
  EXPECT_EQ(nametonum("E"), 64);
  EXPECT_EQ(nametonum("G"), 67);
}

TEST(NoteParserTest, NameToNumLowercase) {
  EXPECT_EQ(nametonum("c4"), 60);
  EXPECT_EQ(nametonum("e4"), 64);
}

TEST(NoteParserTest, NameToNumInvalid) {
  EXPECT_EQ(nametonum(""), -1);
  EXPECT_EQ(nametonum("X"), -1);
  EXPECT_EQ(nametonum("C-2"), -1);  // C at octave -2 would be negative
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
