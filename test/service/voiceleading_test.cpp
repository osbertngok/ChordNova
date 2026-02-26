#include "gtest/gtest.h"
#include "service/voiceleading.h"
#include "model/orderedchord.h"

using namespace chordnovarw::service;
using namespace chordnovarw::model;

// ── Ported from legacy maintest/find_vec.cpp ────────────────────────
// Note: Revamp Pitch parser uses '-' for flats (e.g., "E-4" not "Eb4").

TEST(VoiceLeading, find_vec_test_1) {
  /**
   * C4 E4 G4 → C4 E4 G4 A4
   * Expansion: C4 E4 G4 → C4 E4 G4 G4
   * Vec: [0, 0, 0, 2]
   */
  OrderedChord chord1("C4 E4 G4");
  OrderedChord chord2("C4 E4 G4 A4");
  auto result = find_voice_leading(chord1, chord2);
  EXPECT_EQ(result.vec.size(), 4);
  EXPECT_EQ(result.vec[0], 0);
  EXPECT_EQ(result.vec[1], 0);
  EXPECT_EQ(result.vec[2], 0);
  EXPECT_EQ(result.vec[3], 2);
}

TEST(VoiceLeading, find_vec_test_2a) {
  // G2 F3 C4 Eb4 Bb4 C5 D5 → Eb4 D5 F5 C6
  // Using '-' notation for flats in revamp Pitch parser
  OrderedChord chord1("G2 F3 C4 E-4 B-4 C5 D5");
  OrderedChord chord2("E-4 D5 F5 C6");
  auto result = find_voice_leading(chord1, chord2);
  std::vector<int> expected = {20, 10, 3, 0, 4, 5, 10};
  EXPECT_EQ(result.vec, expected);
}

TEST(VoiceLeading, find_vec_test_2b) {
  // Eb3 G3 Bb3 Bb4 D5 Eb5 F5 → D2 F#3 C4 Eb4
  OrderedChord chord1("E-3 G3 B-3 B-4 D5 E-5 F5");
  OrderedChord chord2("D2 F#3 C4 E-4");
  auto result = find_voice_leading(chord1, chord2);
  std::vector<int> expected = {-13, -1, 2, -7, -11, -12, -14};
  EXPECT_EQ(result.vec, expected);
}

// ── Additional edge cases ───────────────────────────────────────────

TEST(VoiceLeading, same_size_chords) {
  OrderedChord chord1("C4 E4 G4");
  OrderedChord chord2("C4 F4 A4");
  auto result = find_voice_leading(chord1, chord2);
  EXPECT_EQ(result.vec.size(), 3);
  EXPECT_EQ(result.vec[0], 0);  // C4 → C4
  EXPECT_EQ(result.vec[1], 1);  // E4 → F4
  EXPECT_EQ(result.vec[2], 2);  // G4 → A4
  EXPECT_EQ(result.sv, 3);
}

TEST(VoiceLeading, identical_chords) {
  OrderedChord chord("C4 E4 G4");
  auto result = find_voice_leading(chord, chord);
  EXPECT_EQ(result.vec, (std::vector<int>{0, 0, 0}));
  EXPECT_EQ(result.sv, 0);
}

TEST(VoiceLeading, single_note) {
  OrderedChord chord1("C4");
  OrderedChord chord2("D4");
  auto result = find_voice_leading(chord1, chord2);
  EXPECT_EQ(result.vec.size(), 1);
  EXPECT_EQ(result.vec[0], 2);
  EXPECT_EQ(result.sv, 2);
}
