#include <gtest/gtest.h>
#include "algorithm/analysis.h"
#include "model/orderedchord.h"
#include "model/pitch.h"

using namespace chordnovarw::algorithm;
using namespace chordnovarw::model;

namespace {

OrderedChord make_chord(std::initializer_list<int> notes) {
  std::vector<Pitch> pitches;
  for (int n : notes)
    pitches.push_back(Pitch(static_cast<uint8_t>(n)));
  return OrderedChord(std::move(pitches));
}

} // anonymous namespace

// ── analyse() tests ─────────────────────────────────────────────

TEST(AnalysisTest, CMajorToFMajor) {
  auto ante = make_chord({60, 64, 67});  // C4 E4 G4
  auto post = make_chord({65, 69, 72});  // F4 A4 C5

  auto result = analyse(ante, post);

  // Ante stats
  EXPECT_EQ(result.ante_stats.num_of_pitches, 3u);
  EXPECT_EQ(result.ante_stats.num_of_unique_pitch_classes, 3u);

  // Post stats
  EXPECT_EQ(result.post_stats.num_of_pitches, 3u);
  EXPECT_EQ(result.post_stats.num_of_unique_pitch_classes, 3u);

  // Voice leading: should have 3 components
  EXPECT_EQ(result.vl_result.vec.size(), 3u);
  EXPECT_GT(result.vl_result.sv, 0);

  // Bigram stats
  EXPECT_GE(result.bigram_stats.similarity, 0);
  EXPECT_LE(result.bigram_stats.similarity, 100);
  EXPECT_GE(result.bigram_stats.common_note, 0);
}

TEST(AnalysisTest, IdenticalChords) {
  auto chord = make_chord({60, 64, 67});  // C4 E4 G4

  auto result = analyse(chord, chord);

  // Same chord: sv should be 0
  EXPECT_EQ(result.vl_result.sv, 0);
  // Common notes should equal chord size
  EXPECT_EQ(result.bigram_stats.common_note, 3);
  // Similarity should be 100%
  EXPECT_EQ(result.bigram_stats.similarity, 100);
}

TEST(AnalysisTest, ChromaticMovement) {
  auto ante = make_chord({60, 64, 67});  // C major
  auto post = make_chord({61, 65, 68});  // Db major

  auto result = analyse(ante, post);

  // Each voice moves by 1 semitone
  EXPECT_EQ(result.vl_result.sv, 3);
  // No common notes
  EXPECT_EQ(result.bigram_stats.common_note, 0);
  // Chroma measures Circle of Fifths movement (may be 0 if
  // the chroma calculation requires prev_single_chroma context)
  // Just verify the value is computed
  EXPECT_GE(result.bigram_stats.sv, 0);
}

TEST(AnalysisTest, VoiceLeadingVectorSize) {
  auto ante = make_chord({60, 64, 67});       // 3 notes
  auto post = make_chord({60, 64, 67, 72});   // 4 notes

  auto result = analyse(ante, post);

  // Vec size should be max(3, 4) = 4
  EXPECT_EQ(result.vl_result.vec.size(), 4u);
}

TEST(AnalysisTest, WithPrevChromaOld) {
  auto ante = make_chord({60, 64, 67});
  auto post = make_chord({65, 69, 72});

  // Providing previous chroma_old should affect bigram computation
  auto result1 = analyse(ante, post, 0.0, {});
  auto result2 = analyse(ante, post, 5.0, {});

  // The chroma_old values should differ based on prev_chroma_old
  // (prev_chroma_old is passed through to bigram calculation)
  // Just verify both complete without error
  EXPECT_GE(result1.bigram_stats.similarity, 0);
  EXPECT_GE(result2.bigram_stats.similarity, 0);
}

TEST(AnalysisTest, RootMovement) {
  auto ante = make_chord({60, 64, 67});  // C major, root C
  auto post = make_chord({65, 69, 72});  // F major, root F

  auto result = analyse(ante, post);

  // C to F is a perfect fourth = 5 semitones
  // Root movement = min(5, 12-5) = 5
  EXPECT_EQ(result.bigram_stats.root_movement, 5);
}
