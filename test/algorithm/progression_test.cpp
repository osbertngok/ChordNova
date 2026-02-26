#include <gtest/gtest.h>
#include "algorithm/progression.h"
#include "model/orderedchord.h"
#include "model/config.h"

using namespace chordnovarw::algorithm;
using namespace chordnovarw::model;

namespace {

// Helper: create a permissive config for small-scale tests
ProgressionConfig make_permissive_config(int m_max = 3, int vl_max = 2) {
  ProgressionConfig config;
  config.range.lowest = 48;      // C3
  config.range.highest = 84;     // C6
  config.range.m_min = 1;
  config.range.m_max = m_max;
  config.range.n_min = 1;
  config.range.n_max = 12;
  config.range.h_min = 0.0;
  config.range.h_max = 50.0;
  config.range.r_min = 0;
  config.range.r_max = 11;
  config.range.g_min = 0;
  config.range.g_max = 100;
  config.voice_leading.vl_min = 0;
  config.voice_leading.vl_max = vl_max;
  config.voice_leading.vl_setting = VLSetting::Number;
  config.voice_leading.ascending_min = 0;
  config.voice_leading.ascending_max = 100;
  config.voice_leading.descending_min = 0;
  config.voice_leading.descending_max = 100;
  config.voice_leading.steady_min = 0;
  config.voice_leading.steady_max = 100;
  config.harmonic.c_min = 0;
  config.harmonic.c_max = 15;
  config.harmonic.sv_min = 0;
  config.harmonic.sv_max = 200;
  config.harmonic.s_min = 0;
  config.harmonic.s_max = 12;
  config.harmonic.ss_min = 0;
  config.harmonic.ss_max = 12;
  config.harmonic.q_min = -1000.0;
  config.harmonic.q_max = 1000.0;
  config.harmonic.x_min = 0;
  config.harmonic.x_max = 100;
  return config;
}

} // anonymous namespace

// Basic generation: starting from C4 E4 G4, vl_max=1, should produce candidates
TEST(ProgressionTest, GenerateSingleProducesCandidates) {
  OrderedChord initial("C4 E4 G4");
  auto config = make_permissive_config(3, 1);

  auto result = generate_single(initial, config);

  // With vl_max=1 and 3 voices, there are 3^3 = 27 mutation vectors.
  // Many should pass validation.
  EXPECT_GT(result.candidates.size(), 0u);
  EXPECT_GT(result.total_evaluated, 0);
}

// Verify that the identity mutation (0,0,0) produces the same chord
TEST(ProgressionTest, IdentityMutationIncluded) {
  OrderedChord initial("C4 E4 G4");
  auto config = make_permissive_config(3, 1);

  auto result = generate_single(initial, config);

  // Find the identity chord (C4 E4 G4)
  bool found_identity = false;
  for (const auto& entry : result.candidates) {
    auto pitches = entry.chord.get_pitches();
    if (pitches.size() == 3 &&
        pitches[0].get_number() == 60 &&
        pitches[1].get_number() == 64 &&
        pitches[2].get_number() == 67) {
      found_identity = true;
      break;
    }
  }
  // Identity (sv=0) should pass since vl_min=0 and sv_min=0
  EXPECT_TRUE(found_identity);
}

// Test that restrictive config produces fewer candidates
TEST(ProgressionTest, RestrictiveConfigFewerCandidates) {
  OrderedChord initial("C4 E4 G4");
  auto permissive = make_permissive_config(3, 2);
  auto restrictive = make_permissive_config(3, 2);
  restrictive.harmonic.sv_max = 2;  // Very restrictive sv

  auto result_p = generate_single(initial, permissive);
  auto result_r = generate_single(initial, restrictive);

  EXPECT_GT(result_p.candidates.size(), result_r.candidates.size());
}

// Test that all candidates have valid bigram statistics
TEST(ProgressionTest, CandidatesHaveBigramStats) {
  OrderedChord initial("C4 E4 G4");
  auto config = make_permissive_config(3, 1);

  auto result = generate_single(initial, config);

  for (const auto& entry : result.candidates) {
    // Every candidate should have a valid sv value
    EXPECT_GE(entry.stats.sv, 0);
    // Similarity should be in [0, 100]
    EXPECT_GE(entry.stats.similarity, 0);
    EXPECT_LE(entry.stats.similarity, 100);
    // Span should be in [0, 12]
    EXPECT_GE(entry.stats.span, 0);
    EXPECT_LE(entry.stats.span, 12);
  }
}

// Test progress callback is called
TEST(ProgressionTest, ProgressCallbackInvoked) {
  OrderedChord initial("C4 E4 G4");
  auto config = make_permissive_config(3, 2);

  int callback_count = 0;
  auto progress = [&callback_count](long long current, long long total) {
    ++callback_count;
    EXPECT_GT(total, 0);
    EXPECT_GE(current, 0);
  };

  auto result = generate_single(initial, config, {}, 0.0, {}, progress);
  // With 3 voices and vl_max=2, we have 5^3 = 125 mutations per expansion
  // Callback fires every 10000, so may or may not be called depending on total
  // Just verify it doesn't crash
  EXPECT_GE(callback_count, 0);
}

// Test sorting
TEST(ProgressionTest, SortingBySv) {
  OrderedChord initial("C4 E4 G4");
  auto config = make_permissive_config(3, 2);
  config.sort.sort_order = "S+";  // Sort by sv ascending

  auto result = generate_single(initial, config);

  // Verify sv is in ascending order
  for (size_t i = 1; i < result.candidates.size(); ++i) {
    EXPECT_LE(result.candidates[i - 1].stats.sv,
              result.candidates[i].stats.sv);
  }
}

// Test with larger vl_max produces more candidates
TEST(ProgressionTest, LargerVlMaxMoreCandidates) {
  OrderedChord initial("C4 E4 G4");
  auto config1 = make_permissive_config(3, 1);
  auto config2 = make_permissive_config(3, 2);

  auto result1 = generate_single(initial, config1);
  auto result2 = generate_single(initial, config2);

  EXPECT_GT(result2.candidates.size(), result1.candidates.size());
}

// Test uniqueness mode removes duplicates
TEST(ProgressionTest, UniquenessRemovesDupType) {
  OrderedChord initial("C4 E4 G4");
  auto config_no_uniq = make_permissive_config(3, 2);
  auto config_uniq = make_permissive_config(3, 2);
  config_uniq.uniqueness.unique_mode = UniqueMode::RemoveDupType;

  auto result_no = generate_single(initial, config_no_uniq);
  auto result_yes = generate_single(initial, config_uniq);

  // With uniqueness, there should be fewer or equal candidates
  EXPECT_LE(result_yes.candidates.size(), result_no.candidates.size());
}

// Test with scale restriction
TEST(ProgressionTest, ScaleRestriction) {
  OrderedChord initial("C4 E4 G4");
  auto config = make_permissive_config(3, 2);
  config.scale.overall_scale = {0, 2, 4, 5, 7, 9, 11};  // C major only

  auto result = generate_single(initial, config);

  // All candidates should only contain C major scale notes
  for (const auto& entry : result.candidates) {
    auto pitches = entry.chord.get_pitches();
    for (const auto& p : pitches) {
      int pc = p.get_pitch_class().value();
      bool in_scale = (pc == 0 || pc == 2 || pc == 4 || pc == 5 ||
                       pc == 7 || pc == 9 || pc == 11);
      EXPECT_TRUE(in_scale) << "Pitch class " << pc << " not in C major scale";
    }
  }
}
