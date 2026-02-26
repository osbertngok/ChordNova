#include "gtest/gtest.h"
#include "chord.h"
using namespace std;

using namespace chordnova::chord;

TEST(Chord, calculate_pitch_class_set_1) {
  vector<int> notes = {10, 20, 30};
  vector<int> result = calculate_pitch_class_set(notes);
  EXPECT_EQ(result[0], 6);
  EXPECT_EQ(result[1], 8);
  EXPECT_EQ(result[2], 10);
}

TEST(Chord, calculate_pitch_class_set_2) {
  vector<int> notes = {12, 24, 30, 36, 25, 9};
  vector<int> result = calculate_pitch_class_set(notes);
  EXPECT_EQ(result[0], 0);
  EXPECT_EQ(result[1], 1);
  EXPECT_EQ(result[2], 6);
  EXPECT_EQ(result[3], 9);
}

TEST(Chord, chroma1) {
  Chord chord;
  chord.set_notes_from_text("C3 G3 D4");
  chord.initialize_with_notes(chord.m_notes, 0); // This calls Chord::set_span()
  // The original implementation is
  //
  // 6 - (5 * (chord.m_notes[i] % ET_SIZE) + 6) % ET_SIZE
  //
  EXPECT_EQ(chord.single_chroma.size(), 3);
  EXPECT_EQ(chord.single_chroma[0], 0); // C
  EXPECT_EQ(chord.single_chroma[1], 1); // G
  EXPECT_EQ(chord.single_chroma[2], 2); // D
}

TEST(Chord, chroma2) {
  Chord chord;
  chord.set_notes_from_text("F3 C4 A4 E5 G5");
  chord.initialize_with_notes(chord.m_notes, 0);
  EXPECT_EQ(chord.single_chroma.size(), 5);
  EXPECT_EQ(chord.single_chroma[0], -1); // F
  EXPECT_EQ(chord.single_chroma[1], 0); // C
  EXPECT_EQ(chord.single_chroma[2], 3); // A
  EXPECT_EQ(chord.single_chroma[3], 4); // E
  EXPECT_EQ(chord.single_chroma[4], 1); // G
}