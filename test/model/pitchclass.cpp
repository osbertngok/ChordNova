#include "gtest/gtest.h"
#include "model/chord.h"
#include "model/chordstatistics.h"
#include "logger.h"
using namespace std;

using namespace chordnovarw::model;
using namespace chordnovarw::logger;

TEST(ChordRW, chroma) {
  EXPECT_EQ(Pitch("C").get_chroma(), Chroma(0));
  EXPECT_EQ(Pitch("G").get_chroma(), Chroma(1));
  EXPECT_EQ(Pitch("D").get_chroma(), Chroma(2));
}

TEST(ChordRW, pitch_class) {
  EXPECT_EQ(PitchClass::B.value(), 11);
  EXPECT_EQ(PitchClass::B.value(), 11);
}
