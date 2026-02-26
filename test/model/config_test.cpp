#include "gtest/gtest.h"
#include "model/config.h"
#include "model/config_enums.h"

using namespace chordnovarw::model;

// ── Default construction tests ──────────────────────────────────────

TEST(Config, voice_leading_defaults) {
  VoiceLeadingConstraints vlc;
  EXPECT_EQ(vlc.vl_min, 0);
  EXPECT_EQ(vlc.vl_max, 4);
  EXPECT_EQ(vlc.vl_setting, VLSetting::Default);
  EXPECT_DOUBLE_EQ(vlc.steady_min, 0.0);
  EXPECT_DOUBLE_EQ(vlc.ascending_max, 100.0);
}

TEST(Config, range_defaults) {
  RangeConstraints rc;
  EXPECT_EQ(rc.lowest, 0);
  EXPECT_EQ(rc.highest, 127);
  EXPECT_EQ(rc.m_min, 1);
  EXPECT_EQ(rc.m_max, 15);
  EXPECT_EQ(rc.n_min, 1);
  EXPECT_EQ(rc.n_max, 12);
}

TEST(Config, harmonic_defaults) {
  HarmonicConstraints hc;
  EXPECT_DOUBLE_EQ(hc.k_min, 0.0);
  EXPECT_DOUBLE_EQ(hc.k_max, 100.0);
  EXPECT_EQ(hc.c_min, 0);
  EXPECT_EQ(hc.sv_min, 0);
  EXPECT_DOUBLE_EQ(hc.q_min, -500.0);
}

TEST(Config, alignment_defaults) {
  AlignmentConfig ac;
  EXPECT_EQ(ac.align_mode, AlignMode::Unlimited);
  EXPECT_TRUE(ac.alignment_list.empty());
}

TEST(Config, exclusion_defaults) {
  ExclusionConfig ec;
  EXPECT_FALSE(ec.enabled);
  EXPECT_TRUE(ec.exclusion_notes.empty());
}

TEST(Config, pedal_defaults) {
  PedalConfig pc;
  EXPECT_FALSE(pc.enabled);
  EXPECT_EQ(pc.period, 1);
  EXPECT_FALSE(pc.in_bass);
}

TEST(Config, scale_defaults) {
  ScaleConfig sc;
  EXPECT_EQ(sc.overall_scale.size(), 12);
  EXPECT_EQ(sc.overall_scale[0], 0);
  EXPECT_EQ(sc.overall_scale[11], 11);
}

TEST(Config, progression_config_defaults) {
  ProgressionConfig pc;
  EXPECT_FALSE(pc.continual);
  EXPECT_EQ(pc.loop_count, 1);
  EXPECT_EQ(pc.output_mode, OutputMode::Both);
  EXPECT_EQ(pc.voice_leading.vl_max, 4);
  EXPECT_EQ(pc.range.m_max, 15);
  EXPECT_EQ(pc.uniqueness.unique_mode, UniqueMode::Disabled);
}

// ── Custom construction tests ───────────────────────────────────────

TEST(Config, custom_voice_leading) {
  VoiceLeadingConstraints vlc;
  vlc.vl_min = 1;
  vlc.vl_max = 6;
  vlc.vl_setting = VLSetting::Percentage;
  EXPECT_EQ(vlc.vl_min, 1);
  EXPECT_EQ(vlc.vl_max, 6);
  EXPECT_EQ(vlc.vl_setting, VLSetting::Percentage);
}

TEST(Config, custom_progression_config) {
  ProgressionConfig pc;
  pc.continual = true;
  pc.loop_count = 5;
  pc.voice_leading.vl_max = 8;
  pc.harmonic.k_min = 30.0;
  pc.harmonic.k_max = 70.0;
  pc.exclusion.enabled = true;
  pc.exclusion.exclusion_notes = {60, 64};

  EXPECT_TRUE(pc.continual);
  EXPECT_EQ(pc.loop_count, 5);
  EXPECT_EQ(pc.voice_leading.vl_max, 8);
  EXPECT_DOUBLE_EQ(pc.harmonic.k_min, 30.0);
  EXPECT_TRUE(pc.exclusion.enabled);
  EXPECT_EQ(pc.exclusion.exclusion_notes.size(), 2);
}

// ── Enum tests ──────────────────────────────────────────────────────

TEST(ConfigEnums, output_mode_values) {
  EXPECT_NE(OutputMode::Both, OutputMode::MidiOnly);
  EXPECT_NE(OutputMode::MidiOnly, OutputMode::TextOnly);
}

TEST(ConfigEnums, unique_mode_values) {
  EXPECT_NE(UniqueMode::Disabled, UniqueMode::RemoveDup);
  EXPECT_NE(UniqueMode::RemoveDup, UniqueMode::RemoveDupType);
}

TEST(ConfigEnums, align_mode_values) {
  EXPECT_NE(AlignMode::Interval, AlignMode::List);
  EXPECT_NE(AlignMode::List, AlignMode::Unlimited);
}

TEST(ConfigEnums, vl_setting_values) {
  EXPECT_NE(VLSetting::Percentage, VLSetting::Number);
  EXPECT_NE(VLSetting::Number, VLSetting::Default);
}
