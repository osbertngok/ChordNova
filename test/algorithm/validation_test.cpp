#include <gtest/gtest.h>
#include "algorithm/validation.h"
#include "model/orderedchord.h"
#include "model/chordstatistics.h"
#include "model/config.h"
#include "service/voiceleading.h"

using namespace chordnovarw::algorithm;
using namespace chordnovarw::model;
using namespace chordnovarw::service;

namespace {

// Helper: create a minimal ValidationContext with defaults
struct TestContext {
  ProgressionConfig config;
  OrderedChord prev_chord;
  OrderedChordStatistics prev_stats;
  VoiceLeadingResult vl_result;
  std::vector<int> rec_ids;
  std::vector<long long> vec_ids;
  std::vector<int> prev_single_chroma;
  double prev_chroma_old = 0.0;
  std::vector<OrderedChord> record;

  TestContext()
      : prev_chord("C4 E4 G4"),
        prev_stats(calculate_statistics(prev_chord)) {}

  ValidationContext ctx() {
    return ValidationContext{
        config, prev_chord, prev_stats,
        vl_result, std::nullopt,
        rec_ids, vec_ids,
        prev_single_chroma, prev_chroma_old,
        record
    };
  }
};

} // anonymous namespace

// ── Stage 1: Monotonicity ─────────────────────────────────────────

TEST(ValidationTest, MonotonicityPassesAscending) {
  TestContext tc;
  auto ctx = tc.ctx();
  OrderedChord chord("C4 E4 G4");
  EXPECT_TRUE(validate_monotonicity(ctx, chord));
}

TEST(ValidationTest, MonotonicityPassesUnison) {
  TestContext tc;
  auto ctx = tc.ctx();
  OrderedChord chord("C4 C4 G4");
  EXPECT_TRUE(validate_monotonicity(ctx, chord));
}

TEST(ValidationTest, MonotonicityFailsDescending) {
  TestContext tc;
  auto ctx = tc.ctx();
  OrderedChord chord("G4 E4 C4");
  EXPECT_FALSE(validate_monotonicity(ctx, chord));
}

// ── Stage 2: Range ────────────────────────────────────────────────

TEST(ValidationTest, RangePassesWithinBounds) {
  TestContext tc;
  tc.config.range.lowest = 48;
  tc.config.range.highest = 84;
  auto ctx = tc.ctx();
  OrderedChord chord("C4 E4 G4");
  EXPECT_TRUE(validate_range(ctx, chord));
}

TEST(ValidationTest, RangeFailsTooLow) {
  TestContext tc;
  tc.config.range.lowest = 62;
  tc.config.range.highest = 84;
  auto ctx = tc.ctx();
  OrderedChord chord("C4 E4 G4");
  EXPECT_FALSE(validate_range(ctx, chord));
}

TEST(ValidationTest, RangeFailsTooHigh) {
  TestContext tc;
  tc.config.range.lowest = 48;
  tc.config.range.highest = 65;
  auto ctx = tc.ctx();
  OrderedChord chord("C4 E4 G4");
  EXPECT_FALSE(validate_range(ctx, chord));
}

// ── Stage 3: Alignment ───────────────────────────────────────────

TEST(ValidationTest, AlignmentUnlimitedAlwaysPasses) {
  TestContext tc;
  tc.config.alignment.align_mode = AlignMode::Unlimited;
  auto ctx = tc.ctx();
  OrderedChord chord("C4 E4 G4");
  EXPECT_TRUE(validate_alignment(ctx, chord));
}

TEST(ValidationTest, AlignmentIntervalModePass) {
  TestContext tc;
  tc.config.alignment.align_mode = AlignMode::Interval;
  tc.config.alignment.i_low = 1;
  tc.config.alignment.i_high = 12;
  tc.config.alignment.i_min = 1;
  tc.config.alignment.i_max = 12;
  auto ctx = tc.ctx();
  OrderedChord chord("C4 E4 G4");
  EXPECT_TRUE(validate_alignment(ctx, chord));
}

TEST(ValidationTest, AlignmentIntervalModeFail) {
  TestContext tc;
  tc.config.alignment.align_mode = AlignMode::Interval;
  tc.config.alignment.i_low = 5;
  tc.config.alignment.i_high = 12;
  tc.config.alignment.i_min = 1;
  tc.config.alignment.i_max = 12;
  auto ctx = tc.ctx();
  OrderedChord chord("C4 E4 G4");
  EXPECT_FALSE(validate_alignment(ctx, chord));
}

// ── Stage 4: Exclusion ───────────────────────────────────────────

TEST(ValidationTest, ExclusionDisabledPasses) {
  TestContext tc;
  tc.config.exclusion.enabled = false;
  auto ctx = tc.ctx();
  OrderedChord chord("C4 E4 G4");
  EXPECT_TRUE(validate_exclusion(ctx, chord));
}

TEST(ValidationTest, ExclusionNoteRejects) {
  TestContext tc;
  tc.config.exclusion.enabled = true;
  tc.config.exclusion.exclusion_notes = {64};
  auto ctx = tc.ctx();
  OrderedChord chord("C4 E4 G4");
  EXPECT_FALSE(validate_exclusion(ctx, chord));
}

TEST(ValidationTest, ExclusionRootRejects) {
  TestContext tc;
  tc.config.exclusion.enabled = true;
  tc.config.exclusion.exclusion_roots = {0};
  auto ctx = tc.ctx();
  OrderedChord chord("C4 E4 G4");
  EXPECT_FALSE(validate_exclusion(ctx, chord));
}

// ── Stage 5: Pedal ───────────────────────────────────────────────

TEST(ValidationTest, PedalDisabledPasses) {
  TestContext tc;
  tc.config.pedal.enabled = false;
  auto ctx = tc.ctx();
  OrderedChord chord("C4 E4 G4");
  EXPECT_TRUE(validate_pedal(ctx, chord));
}

TEST(ValidationTest, PedalInBassPass) {
  TestContext tc;
  tc.config.pedal.enabled = true;
  tc.config.pedal.in_bass = true;
  tc.config.pedal.pedal_notes = {60};
  tc.config.continual = true;
  auto ctx = tc.ctx();
  OrderedChord chord("C4 E4 G4");
  EXPECT_TRUE(validate_pedal(ctx, chord));
}

TEST(ValidationTest, PedalInBassFail) {
  TestContext tc;
  tc.config.pedal.enabled = true;
  tc.config.pedal.in_bass = true;
  tc.config.pedal.pedal_notes = {62};
  tc.config.continual = true;
  auto ctx = tc.ctx();
  OrderedChord chord("C4 E4 G4");
  EXPECT_FALSE(validate_pedal(ctx, chord));
}

// ── Stage 6: Cardinality ─────────────────────────────────────────

TEST(ValidationTest, CardinalityPass) {
  TestContext tc;
  tc.config.range.m_min = 3;
  tc.config.range.m_max = 5;
  tc.config.range.n_min = 3;
  tc.config.range.n_max = 5;
  auto ctx = tc.ctx();
  OrderedChord chord("C4 E4 G4");
  EXPECT_TRUE(validate_cardinality(ctx, chord));
}

TEST(ValidationTest, CardinalityFailTooFewNotes) {
  TestContext tc;
  tc.config.range.m_min = 4;
  tc.config.range.m_max = 6;
  auto ctx = tc.ctx();
  OrderedChord chord("C4 E4 G4");
  EXPECT_FALSE(validate_cardinality(ctx, chord));
}

// ── Stage 7: Single-chord stats ──────────────────────────────────

TEST(ValidationTest, SingleChordStatsPass) {
  TestContext tc;
  tc.config.range.h_min = 0.0;
  tc.config.range.h_max = 50.0;
  tc.config.range.r_min = 0;
  tc.config.range.r_max = 11;
  tc.config.range.g_min = 0;
  tc.config.range.g_max = 100;
  auto ctx = tc.ctx();
  OrderedChord chord("C4 E4 G4");
  EXPECT_TRUE(validate_single_chord_stats(ctx, chord));
}

TEST(ValidationTest, SingleChordStatsThicknessFail) {
  TestContext tc;
  tc.config.range.h_min = 10.0;
  tc.config.range.h_max = 50.0;
  tc.config.range.r_min = 0;
  tc.config.range.r_max = 11;
  tc.config.range.g_min = 0;
  tc.config.range.g_max = 100;
  auto ctx = tc.ctx();
  OrderedChord chord("C4 E4 G4");
  auto stats = calculate_statistics(chord);
  if (stats.thickness < 10.0)
    EXPECT_FALSE(validate_single_chord_stats(ctx, chord));
}

// ── Stage 8: Scale membership ────────────────────────────────────

TEST(ValidationTest, ScaleMembershipChromaticPasses) {
  TestContext tc;
  auto ctx = tc.ctx();
  OrderedChord chord("C4 E4 G4");
  EXPECT_TRUE(validate_scale_membership(ctx, chord));
}

TEST(ValidationTest, ScaleMembershipCMajorPass) {
  TestContext tc;
  tc.config.scale.overall_scale = {0, 2, 4, 5, 7, 9, 11};
  auto ctx = tc.ctx();
  OrderedChord chord("C4 E4 G4");
  EXPECT_TRUE(validate_scale_membership(ctx, chord));
}

TEST(ValidationTest, ScaleMembershipFail) {
  TestContext tc;
  tc.config.scale.overall_scale = {0, 2, 4, 5, 7, 9, 11};
  auto ctx = tc.ctx();
  OrderedChord chord("C4 E-4 G4");
  EXPECT_FALSE(validate_scale_membership(ctx, chord));
}

// ── Stage 9: Bass and library ────────────────────────────────────

TEST(ValidationTest, BassAndLibraryEmptyLibraryPasses) {
  TestContext tc;
  auto ctx = tc.ctx();
  OrderedChord chord("C4 E4 G4");
  EXPECT_TRUE(validate_bass_and_library(ctx, chord));
}

// ── Stage 10: Uniqueness ─────────────────────────────────────────

TEST(ValidationTest, UniquenessDisabledPasses) {
  TestContext tc;
  tc.config.uniqueness.unique_mode = UniqueMode::Disabled;
  auto ctx = tc.ctx();
  OrderedChord chord("C4 E4 G4");
  EXPECT_TRUE(validate_uniqueness(ctx, chord));
}

TEST(ValidationTest, UniquenessDupTypeRejectsDuplicate) {
  TestContext tc;
  tc.config.uniqueness.unique_mode = UniqueMode::RemoveDupType;
  auto ctx = tc.ctx();
  OrderedChord chord1("C4 E4 G4");
  EXPECT_TRUE(validate_uniqueness(ctx, chord1));

  OrderedChord chord2("C5 E5 G5");
  EXPECT_FALSE(validate_uniqueness(ctx, chord2));
}

// ── Stage 11: Voice leading ──────────────────────────────────────

TEST(ValidationTest, VoiceLeadingPassSmallMovement) {
  TestContext tc;
  tc.config.voice_leading.vl_min = 0;
  tc.config.voice_leading.vl_max = 4;
  tc.config.voice_leading.vl_setting = VLSetting::Number;
  tc.config.voice_leading.ascending_min = 0;
  tc.config.voice_leading.ascending_max = 100;
  tc.config.voice_leading.descending_min = 0;
  tc.config.voice_leading.descending_max = 100;
  tc.config.voice_leading.steady_min = 0;
  tc.config.voice_leading.steady_max = 100;
  tc.config.harmonic.c_min = 0;
  tc.config.harmonic.c_max = 15;
  tc.config.harmonic.sv_min = 0;
  tc.config.harmonic.sv_max = 100;
  auto ctx = tc.ctx();
  OrderedChord chord("D4 F4 A4");
  EXPECT_TRUE(validate_voice_leading(ctx, chord));
}

TEST(ValidationTest, VoiceLeadingFailLargeMovement) {
  TestContext tc;
  tc.config.voice_leading.vl_min = 0;
  tc.config.voice_leading.vl_max = 2;
  tc.config.harmonic.c_min = 0;
  tc.config.harmonic.c_max = 15;
  tc.config.harmonic.sv_min = 0;
  tc.config.harmonic.sv_max = 100;
  auto ctx = tc.ctx();
  OrderedChord chord("C4 E4 C5");
  EXPECT_FALSE(validate_voice_leading(ctx, chord));
}

// ── Stage 12: Similarity ─────────────────────────────────────────

TEST(ValidationTest, SimilarityPassDefault) {
  TestContext tc;
  tc.config.harmonic.x_min = 0;
  tc.config.harmonic.x_max = 100;
  tc.config.voice_leading.vl_max = 4;
  OrderedChord chord("D4 F4 A4");
  tc.vl_result = find_voice_leading(tc.prev_chord, chord);
  auto ctx = tc.ctx();
  EXPECT_TRUE(validate_similarity(ctx, chord));
}

// ── Stage 13: Span ───────────────────────────────────────────────

TEST(ValidationTest, SpanPassDefault) {
  TestContext tc;
  tc.config.harmonic.s_min = 0;
  tc.config.harmonic.s_max = 12;
  tc.config.harmonic.ss_min = 0;
  tc.config.harmonic.ss_max = 12;
  auto ctx = tc.ctx();
  OrderedChord chord("C4 E4 G4");
  EXPECT_TRUE(validate_span(ctx, chord));
}

TEST(ValidationTest, SpanFailTooNarrow) {
  TestContext tc;
  tc.config.harmonic.s_min = 10;
  tc.config.harmonic.s_max = 12;
  auto ctx = tc.ctx();
  OrderedChord chord("C4 E4 G4");
  EXPECT_FALSE(validate_span(ctx, chord));
}

// ── Stage 14: Q-indicator ────────────────────────────────────────

TEST(ValidationTest, QIndicatorPassDefault) {
  TestContext tc;
  tc.config.harmonic.q_min = -500.0;
  tc.config.harmonic.q_max = 500.0;
  auto ctx = tc.ctx();
  OrderedChord chord("C4 E4 G4");
  EXPECT_TRUE(validate_q_indicator(ctx, chord));
}

// ── Stage 15: Vec uniqueness ─────────────────────────────────────

TEST(ValidationTest, VecUniquenessFirstAlwaysPasses) {
  TestContext tc;
  tc.vl_result.vec = {1, 2, 3};
  auto ctx = tc.ctx();
  OrderedChord chord("C4 E4 G4");
  EXPECT_TRUE(validate_vec_uniqueness(ctx, chord));
}

TEST(ValidationTest, VecUniquenessRejectsDuplicate) {
  TestContext tc;
  tc.vl_result.vec = {1, 2, 3};
  auto ctx = tc.ctx();
  OrderedChord chord1("C4 E4 G4");
  EXPECT_TRUE(validate_vec_uniqueness(ctx, chord1));

  OrderedChord chord2("D4 F4 A4");
  EXPECT_FALSE(validate_vec_uniqueness(ctx, chord2));
}

// ── Pipeline integration ─────────────────────────────────────────

TEST(ValidationPipelineTest, BasicAcceptance) {
  TestContext tc;
  tc.config.range.lowest = 0;
  tc.config.range.highest = 127;
  tc.config.voice_leading.vl_min = 0;
  tc.config.voice_leading.vl_max = 12;
  tc.config.voice_leading.vl_setting = VLSetting::Number;
  tc.config.voice_leading.ascending_min = 0;
  tc.config.voice_leading.ascending_max = 100;
  tc.config.voice_leading.descending_min = 0;
  tc.config.voice_leading.descending_max = 100;
  tc.config.voice_leading.steady_min = 0;
  tc.config.voice_leading.steady_max = 100;
  tc.config.harmonic.c_min = 0;
  tc.config.harmonic.c_max = 15;
  tc.config.harmonic.sv_min = 0;
  tc.config.harmonic.sv_max = 200;
  tc.config.harmonic.s_min = 0;
  tc.config.harmonic.s_max = 12;
  tc.config.harmonic.ss_min = 0;
  tc.config.harmonic.ss_max = 12;
  tc.config.harmonic.q_min = -1000.0;
  tc.config.harmonic.q_max = 1000.0;
  tc.config.harmonic.x_min = 0;
  tc.config.harmonic.x_max = 100;

  ChordValidationPipeline pipeline;
  auto ctx = tc.ctx();
  OrderedChord chord("D4 F4 A4");
  EXPECT_TRUE(pipeline.validate(ctx, chord));
}

TEST(ValidationPipelineTest, RejectedByRange) {
  TestContext tc;
  tc.config.range.lowest = 70;
  tc.config.range.highest = 127;

  ChordValidationPipeline pipeline;
  auto ctx = tc.ctx();
  OrderedChord chord("C4 E4 G4");
  EXPECT_FALSE(pipeline.validate(ctx, chord));
}
