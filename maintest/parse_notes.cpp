#include "gtest/gtest.h"
#include "chord.h"
using namespace std;

using namespace chordnova::chord;

TEST(Chord, parse_notes_1) {
  // C
  Chord chord1;
  bool ret = chord1.set_notes_from_text(string("C4 E4 G4"));
  EXPECT_EQ(ret, true);
  EXPECT_EQ(chord1.m_notes.size(), 3);
  EXPECT_EQ(chord1.m_notes[0], 60);
  EXPECT_EQ(chord1.m_notes[1], 64);
  EXPECT_EQ(chord1.m_notes[2], 67);

  // B7
  Chord chord2;
  ret = chord2.set_notes_from_text(string("B3 D#4 F#4 A4"));
  EXPECT_EQ(ret, true);
  EXPECT_EQ(chord2.m_notes.size(), 4);
  EXPECT_EQ(chord2.m_notes[0], 59);
  EXPECT_EQ(chord2.m_notes[1], 63);
  EXPECT_EQ(chord2.m_notes[2], 66);
  EXPECT_EQ(chord2.m_notes[3], 69);
}