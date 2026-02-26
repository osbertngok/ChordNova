#include "gtest/gtest.h"
#include "chord.h"
#include "functions.h"
using namespace std;

using namespace chordnova::chord;

void find_vec_test(const char* chord1_text, const char* chord2_text, vector<int> result) {
  // Initialize set_expansion_indexes. This is important, otherwise @ref Chord::find_vec would yield incorrect result.
  chordnova::functions::set_expansion_indexes();

  Chord chord1;
  bool ret = chord1.set_notes_from_text(string(chord1_text));
  EXPECT_EQ(ret, true);
  // This is mandatory. otherwise m_notes_size would not be initialized, and calling find_vec would likely crash.
  chord1.set_param1();
  Chord chord2;
  ret = chord2.set_notes_from_text(string(chord2_text));
  EXPECT_EQ(ret, true);
  // Same as above
  chord2.set_param1();
  EXPECT_EQ(chord2.vec.size(), 0);

  chord1.find_vec(chord2, false, false);
  // examine vec and sv of chord2
  EXPECT_EQ(chord2.vec.size(), result.size());
  for (int i = 0; i < result.size(); i++) {
    EXPECT_EQ(chord2.vec[i], result[i]);
  }

}

TEST(Chord, find_vec_test_1) {
  /**
   *
   * In this test, we would like to find the movement vector between C4 E4 G4 and C4 E4 G4 A4.
   * It is quite obvious that we need to expand the former chord to C4 E4 G4 G4.
   *
   * Applying diff we have:
   * * C4 - C4 = 0
   * * E4 - E4 = 0
   * * G4 - G4 = 0
   * * A4 - G4 = 2
   */

  // Initialize set_expansion_indexes. This is important, otherwise @ref Chord::find_vec would yield incorrect result.
  chordnova::functions::set_expansion_indexes();

  Chord chord1;
  bool ret = chord1.set_notes_from_text(string("C4 E4 G4"));
  EXPECT_EQ(ret, true);
  // This is mandatory. otherwise m_notes_size would not be initialized, and calling find_vec would likely crash.
  chord1.set_param1();
  Chord chord2;
  ret = chord2.set_notes_from_text(string("C4 E4 G4 A4"));
  EXPECT_EQ(ret, true);
  // Same as above
  chord2.set_param1();
  EXPECT_EQ(chord2.vec.size(), 0);

  chord1.find_vec(chord2, false, false);
  // examine vec and sv of chord2
  EXPECT_EQ(chord2.vec.size(), 4);
  EXPECT_EQ(chord2.vec[0], 0);
  EXPECT_EQ(chord2.vec[1], 0);
  EXPECT_EQ(chord2.vec[2], 0);
  EXPECT_EQ(chord2.vec[3], 2);
}

TEST(Chord, find_vec_test_2) {
  find_vec_test("G2 F3 C4 Eb4 Bb4 C5 D5", "Eb4 D5 F5 C6", {20, 10, 3, 0, 4, 5, 10});
  find_vec_test("Eb3 G3 Bb3 Bb4 D5 Eb5 F5", "D2 F#3 C4 Eb4", {-13, -1, 2, -7, -11, -12, -14});
}