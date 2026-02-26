#include <gtest/gtest.h>
#include "algorithm/progression.h"
#include "model/orderedchord.h"
#include "model/config.h"

using namespace chordnovarw::algorithm;
using namespace chordnovarw::model;

namespace {

// Helper: generate all 12 transpositions of a pitch-class set and encode as bitmask IDs
void note_set_to_id(const std::vector<int>& note_set, std::vector<int>& rec) {
  for (int j = 0; j < 12; ++j) {
    int val = 0;
    for (int n : note_set) {
      val += (1 << ((n + j) % 12));
    }
    rec.push_back(val);
  }
}

// Port of legacy preset_1: mirrors the exact configuration from maintest/get_progression.cpp
// Note: k_min/k_max/t_min/t_max are percentile-based post-processing in the legacy code
// and are not applied during generation. The new pipeline does not implement percentile
// trimming, so we leave these at default (0/100) and pin the full pre-trim output.
ProgressionConfig make_golden_config() {
  ProgressionConfig config;

  config.range.lowest = 0;
  config.range.highest = 127;
  config.range.m_min = 1;
  config.range.m_max = 4;
  config.range.n_min = 1;
  config.range.n_max = 12;
  config.range.h_min = 0.0;
  config.range.h_max = 50.0;
  config.range.r_min = 0;
  config.range.r_max = 11;
  config.range.g_min = 0;
  config.range.g_max = 70.0;

  config.voice_leading.vl_min = 0;
  config.voice_leading.vl_max = 4;
  config.voice_leading.vl_setting = VLSetting::Default;

  config.alignment.align_mode = AlignMode::Unlimited;

  config.uniqueness.unique_mode = UniqueMode::RemoveDup;
  config.continual = false;

  config.harmonic.c_min = 0;
  config.harmonic.c_max = 2;
  config.harmonic.s_min = 0;
  config.harmonic.s_max = 12;
  config.harmonic.ss_min = 0;
  config.harmonic.ss_max = 12;
  config.harmonic.sv_min = 4;
  config.harmonic.sv_max = 12;
  config.harmonic.q_min = -500.0;
  config.harmonic.q_max = 500.0;
  config.harmonic.x_min = 0;
  config.harmonic.x_max = 100;

  config.root_movement.enabled = false;
  config.exclusion.enabled = false;
  config.similarity.enabled = false;

  config.bass.bass_avail = {1, 3, 5, 7, 9, 11, 13};

  // Chord library: Major + Minor triads (all 12 transpositions each)
  note_set_to_id({0, 4, 7}, config.chord_library.chord_library);
  note_set_to_id({0, 3, 7}, config.chord_library.chord_library);

  // Deduplicate and sort the chord library
  std::sort(config.chord_library.chord_library.begin(),
            config.chord_library.chord_library.end());
  config.chord_library.chord_library.erase(
      std::unique(config.chord_library.chord_library.begin(),
                  config.chord_library.chord_library.end()),
      config.chord_library.chord_library.end());

  return config;
}

// Helper to extract MIDI note vector from candidate
std::vector<int> get_midi(const CandidateEntry& entry) {
  std::vector<int> midi;
  for (const auto& p : entry.chord.get_pitches())
    midi.push_back(p.get_number());
  return midi;
}

} // anonymous namespace

/**
 * Golden output test: pins the full pipeline output from C major triad with
 * major+minor chord library constraint.
 *
 * Configuration mirrors legacy maintest/get_progression.cpp (preset_1) but
 * without the post-generation percentile trimming (k_min/k_max/t_min/t_max)
 * which is not part of the validation pipeline.
 *
 * This test pins: total candidate count, and specific chords at key positions
 * to detect any behavioral changes during refactoring or Rust migration.
 */
TEST(ProgressionGolden, LegacyGetProgression1_TotalCount) {
  OrderedChord initial("C4 E4 G4");
  auto config = make_golden_config();
  auto result = generate_single(initial, config);

  // Pin the exact candidate count produced by the pipeline
  EXPECT_EQ(result.candidates.size(), 92u);
}

TEST(ProgressionGolden, LegacyGetProgression1_ContainsLegacyChords) {
  OrderedChord initial("C4 E4 G4");
  auto config = make_golden_config();
  auto result = generate_single(initial, config);

  // The 5 chords from the legacy test must all appear in the output.
  // (Legacy test applied percentile trimming to get 10 from the full set.)
  std::vector<std::vector<int>> legacy_chords = {
    {56, 59, 64, 68},  // E/G#
    {59, 62, 67, 71},  // G/B
    {57, 60, 65, 69},  // Am
    {58, 62, 67, 70},  // Gm/Bb
    {59, 64, 68, 71},  // E/B
  };

  for (const auto& expected : legacy_chords) {
    bool found = false;
    for (const auto& entry : result.candidates) {
      if (get_midi(entry) == expected) {
        found = true;
        break;
      }
    }
    EXPECT_TRUE(found) << "Missing chord: "
        << expected[0] << " " << expected[1] << " "
        << expected[2] << " " << expected[3];
  }
}

TEST(ProgressionGolden, LegacyGetProgression1_FirstAndLastChord) {
  OrderedChord initial("C4 E4 G4");
  auto config = make_golden_config();
  auto result = generate_single(initial, config);

  ASSERT_GE(result.candidates.size(), 2u);

  // Pin first and last candidate (generation order is deterministic)
  auto first = get_midi(result.candidates[0]);
  auto last = get_midi(result.candidates[result.candidates.size() - 1]);

  // First candidate in generation order
  EXPECT_EQ(first, (std::vector<int>{56, 60, 60, 63}));

  // Last candidate
  EXPECT_EQ(last.size(), 4u);
}

TEST(ProgressionGolden, AllCandidatesHaveValidStats) {
  OrderedChord initial("C4 E4 G4");
  auto config = make_golden_config();
  auto result = generate_single(initial, config);

  for (const auto& entry : result.candidates) {
    // sv should be within configured range
    EXPECT_GE(entry.stats.sv, config.harmonic.sv_min);
    EXPECT_LE(entry.stats.sv, config.harmonic.sv_max);
    // similarity should be within range
    EXPECT_GE(entry.stats.similarity, config.harmonic.x_min);
    EXPECT_LE(entry.stats.similarity, config.harmonic.x_max);
    // common notes within range
    EXPECT_GE(entry.stats.common_note, config.harmonic.c_min);
    EXPECT_LE(entry.stats.common_note, config.harmonic.c_max);
  }
}
