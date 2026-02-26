#include <gtest/gtest.h>
#include "service/chordlibrary.h"
#include "model/orderedchord.h"

using namespace chordnovarw::service;
using namespace chordnovarw::model;

TEST(ChordLibraryTest, SingletonReturnsSameInstance) {
  auto& inst1 = ChordLibrary::getInstance();
  auto& inst2 = ChordLibrary::getInstance();
  EXPECT_EQ(&inst1, &inst2);
}

TEST(ChordLibraryTest, GetChordDataReturnsValidStats) {
  auto& lib = ChordLibrary::getInstance();
  OrderedChord chord("C4 E4 G4");
  const auto& stats = lib.get_chord_data(chord);

  // Should have 3 pitches
  EXPECT_EQ(stats.num_of_pitches, 3u);
  // Root should be present for a major triad
  EXPECT_TRUE(stats.root.has_value());
}

TEST(ChordLibraryTest, CachedResultIsSameReference) {
  auto& lib = ChordLibrary::getInstance();
  OrderedChord chord("D4 F4 A4");
  const auto& stats1 = lib.get_chord_data(chord);
  const auto& stats2 = lib.get_chord_data(chord);

  // Same reference means it was cached
  EXPECT_EQ(&stats1, &stats2);
}

TEST(ChordLibraryTest, DifferentChordsGetDifferentStats) {
  auto& lib = ChordLibrary::getInstance();
  OrderedChord chord1("C4 E4 G4");
  OrderedChord chord2("D4 F#4 A4");
  const auto& stats1 = lib.get_chord_data(chord1);
  const auto& stats2 = lib.get_chord_data(chord2);

  EXPECT_NE(&stats1, &stats2);
}
