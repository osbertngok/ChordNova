#include <gtest/gtest.h>
#include "algorithm/substitution.h"
#include "model/orderedchord.h"
#include "model/pitch.h"
#include "model/substitution_config.h"

using namespace chordnovarw::algorithm;
using namespace chordnovarw::model;

namespace {

OrderedChord make_chord(std::initializer_list<int> notes) {
  std::vector<Pitch> pitches;
  for (int n : notes)
    pitches.push_back(Pitch(static_cast<char>(n)));
  return OrderedChord(std::move(pitches));
}

} // anonymous namespace

// ── id_to_chord() tests ─────────────────────────────────────────

TEST(SubstitutionTest, IdToChordBit0) {
  auto chord = id_to_chord(1);
  auto pitches = chord.get_pitches();
  ASSERT_EQ(pitches.size(), 1u);
  EXPECT_EQ(pitches[0].get_number(), 72);
}

TEST(SubstitutionTest, IdToChordAllBits) {
  auto chord = id_to_chord(4095);
  auto pitches = chord.get_pitches();
  ASSERT_EQ(pitches.size(), 12u);
  for (int i = 0; i < 12; ++i)
    EXPECT_EQ(pitches[i].get_number(), 72 + i);
}

TEST(SubstitutionTest, IdToChordBit1And3) {
  // id=0b1010 = 10 -> bits 1 and 3 -> MIDI 73, 75
  auto chord = id_to_chord(10);
  auto pitches = chord.get_pitches();
  ASSERT_EQ(pitches.size(), 2u);
  EXPECT_EQ(pitches[0].get_number(), 73);
  EXPECT_EQ(pitches[1].get_number(), 75);
}

// ── reduce_to_octave6() tests ───────────────────────────────────

TEST(SubstitutionTest, ReduceToOctave6) {
  auto chord = make_chord({60, 64, 67});  // C4 E4 G4
  auto reduced = reduce_to_octave6(chord);
  auto pitches = reduced.get_pitches();

  ASSERT_EQ(pitches.size(), 3u);
  // C=0 -> 72, E=4 -> 76, G=7 -> 79
  EXPECT_EQ(pitches[0].get_number(), 72);
  EXPECT_EQ(pitches[1].get_number(), 76);
  EXPECT_EQ(pitches[2].get_number(), 79);
}

TEST(SubstitutionTest, ReduceRemovesDuplicatePitchClasses) {
  auto chord = make_chord({60, 64, 67, 72});  // C4 E4 G4 C5
  auto reduced = reduce_to_octave6(chord);
  auto pitches = reduced.get_pitches();
  ASSERT_EQ(pitches.size(), 3u);  // C, E, G deduplicated
}

// ── compute_substitution_similarity() tests ─────────────────────

TEST(SubstitutionTest, SimilarityZeroSv) {
  EXPECT_EQ(compute_substitution_similarity(0, false), 100);
}

TEST(SubstitutionTest, SimilarityMaxSv) {
  EXPECT_EQ(compute_substitution_similarity(36, false), 0);
}

TEST(SubstitutionTest, SimilaritySameRootBoost) {
  int sim_no_root = compute_substitution_similarity(18, false);
  int sim_root = compute_substitution_similarity(18, true);
  EXPECT_GT(sim_root, sim_no_root);
}

// ── compute_tolerance_range() tests ─────────────────────────────

TEST(SubstitutionTest, ToleranceAbsolute) {
  ParamTolerance tol;
  tol.center = 50.0;
  tol.radius = 10.0;
  tol.use_percentage = false;
  compute_tolerance_range(tol);
  EXPECT_DOUBLE_EQ(tol.min_sub, 40.0);
  EXPECT_DOUBLE_EQ(tol.max_sub, 60.0);
}

TEST(SubstitutionTest, TolerancePercentage) {
  ParamTolerance tol;
  tol.center = 100.0;
  tol.radius = 25.0;
  tol.use_percentage = true;
  compute_tolerance_range(tol);
  EXPECT_DOUBLE_EQ(tol.min_sub, 75.0);
  EXPECT_DOUBLE_EQ(tol.max_sub, 125.0);
}

// ── substitute() tests ──────────────────────────────────────────

TEST(SubstitutionTest, SubstitutePostchordFindsResults) {
  auto ante = make_chord({72, 76, 79});  // C E G at octave 6
  auto post = make_chord({77, 81, 84});  // F A C at octave 6

  SubstitutionConfig config;
  config.object = SubstituteObj::Postchord;
  config.sort_order = "S";
  config.sv.center = 0;
  config.sv.radius = 100;

  auto result = substitute(ante, post, config);

  // Should find at least some substitutes
  EXPECT_GT(result.entries.size(), 0u);
  // 4094 because the original is skipped
  EXPECT_EQ(result.total_evaluated, 4094);
}

TEST(SubstitutionTest, SubstituteAntechordFindsResults) {
  auto ante = make_chord({72, 76, 79});
  auto post = make_chord({77, 81, 84});

  SubstitutionConfig config;
  config.object = SubstituteObj::Antechord;
  config.sort_order = "S";
  config.sv.center = 0;
  config.sv.radius = 100;

  auto result = substitute(ante, post, config);

  EXPECT_GT(result.entries.size(), 0u);
  EXPECT_EQ(result.total_evaluated, 4094);
}

TEST(SubstitutionTest, SubstituteRestrictiveConfigFewerResults) {
  auto ante = make_chord({72, 76, 79});
  auto post = make_chord({77, 81, 84});

  // Permissive config
  SubstitutionConfig config1;
  config1.object = SubstituteObj::Postchord;
  config1.sort_order = "S";
  config1.sv.center = 0;
  config1.sv.radius = 100;

  // Restrictive config: require sv very close to 0
  SubstitutionConfig config2;
  config2.object = SubstituteObj::Postchord;
  config2.sort_order = "S";
  config2.sv.center = 0;
  config2.sv.radius = 1;

  auto result1 = substitute(ante, post, config1);
  auto result2 = substitute(ante, post, config2);

  EXPECT_GT(result1.entries.size(), result2.entries.size());
}

TEST(SubstitutionTest, SubstituteProgressCallbackInvoked) {
  auto ante = make_chord({72, 76, 79});
  auto post = make_chord({77, 81, 84});

  SubstitutionConfig config;
  config.object = SubstituteObj::Postchord;
  config.sort_order = "S";
  config.sv.center = 0;
  config.sv.radius = 100;

  int callback_count = 0;
  auto result = substitute(ante, post, config,
      [&](long long, long long) { callback_count++; });

  EXPECT_GT(callback_count, 0);
}

TEST(SubstitutionTest, SubstituteEntriesHaveStats) {
  auto ante = make_chord({72, 76, 79});
  auto post = make_chord({77, 81, 84});

  SubstitutionConfig config;
  config.object = SubstituteObj::Postchord;
  config.sort_order = "S";
  config.sv.center = 0;
  config.sv.radius = 100;

  auto result = substitute(ante, post, config);

  for (const auto& entry : result.entries) {
    EXPECT_GE(entry.sim_orig, 0);
    EXPECT_LE(entry.sim_orig, 100);
    EXPECT_GE(entry.stats.sv, 0);
  }
}
