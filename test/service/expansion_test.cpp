#include "gtest/gtest.h"
#include "service/expansion.h"
#include "model/orderedchord.h"
#include "utility/combinatorics.h"

using namespace chordnovarw::service;
using namespace chordnovarw::model;
using namespace chordnovarw::utility;

TEST(Expansion, identity_same_size) {
  OrderedChord chord("C4 E4 G4");
  auto results = expand(chord, 3);
  EXPECT_EQ(results.size(), 1);
  EXPECT_EQ(results[0].get_num_of_pitches(), 3);
}

TEST(Expansion, three_to_five_count) {
  // 3-note chord to 5 voices: C(4,2) = 6 expansions
  OrderedChord chord("C4 E4 G4");
  auto results = expand(chord, 5);
  EXPECT_EQ(results.size(), 6);
}

TEST(Expansion, three_to_five_all_valid) {
  OrderedChord chord("C4 E4 G4");
  auto results = expand(chord, 5);
  for (const auto& expansion : results) {
    EXPECT_EQ(expansion.get_num_of_pitches(), 5);
    // Each expansion should only contain original pitch classes
    EXPECT_TRUE(expansion.contains_pitch_class(PitchClass::C));
    EXPECT_TRUE(expansion.contains_pitch_class(PitchClass::E));
    EXPECT_TRUE(expansion.contains_pitch_class(PitchClass::G));
    // Pitches should be sorted ascending
    auto pitches = expansion.get_pitches();
    for (size_t i = 1; i < pitches.size(); ++i) {
      EXPECT_FALSE(pitches[i] < pitches[i - 1]);
    }
  }
}

TEST(Expansion, expand_single_index_zero) {
  OrderedChord chord("C4 E4 G4");
  auto result = expand_single(chord, 4, 0);
  EXPECT_EQ(result.get_num_of_pitches(), 4);
}

TEST(Expansion, single_note_expansion) {
  OrderedChord chord("C4");
  auto results = expand(chord, 3);
  EXPECT_EQ(results.size(), 1);
  auto pitches = results[0].get_pitches();
  EXPECT_EQ(pitches.size(), 3);
  // All should be C4
  for (const auto& p : pitches) {
    EXPECT_EQ(p.get_pitch_class(), PitchClass::C);
  }
}

TEST(Expansion, target_less_than_source_throws) {
  OrderedChord chord("C4 E4 G4");
  EXPECT_THROW(expand(chord, 2), std::invalid_argument);
}

TEST(Expansion, two_to_four_count) {
  // 2-note chord to 4 voices: C(3,1) = 3 expansions
  OrderedChord chord("C4 G4");
  auto results = expand(chord, 4);
  EXPECT_EQ(results.size(), 3);
}
