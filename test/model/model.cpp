#include "gtest/gtest.h"
#include "model/chord.h"
#include "model/chordstatistics.h"
#include "model/bigramchordstatistics.h"
#include "model/pitchset.h"
#include "utility.h"
#include "logger.h"
using namespace std;

using namespace chordnovarw::model;
using namespace chordnovarw::logger;

TEST(ChordRW, get_circle_of_fifth_distance) {
  EXPECT_EQ(get_circle_of_fifth_distance(PitchClass::C, PitchClass::G), COFUnit(1));
  EXPECT_EQ(get_circle_of_fifth_distance(PitchClass::C, PitchClass::D), COFUnit(2));
}


TEST(ChordRW, pitch_serialization) {
  EXPECT_EQ(Pitch("B"), Pitch(PitchClass::B, Octave::OMinus1));
  EXPECT_EQ(Pitch("C4"), Pitch(PitchClass::C, Octave::O4));
  EXPECT_EQ(Pitch("A-5"), Pitch(PitchClass::Ab, Octave::O5));
  EXPECT_EQ(Pitch("D#"), Pitch(PitchClass::Ds, Octave::OMinus1));
  EXPECT_EQ(Pitch("C4").get_number(), static_cast<char>(60));
  EXPECT_EQ(Pitch("A4").get_number(), static_cast<char>(69));
}

TEST(ChordRW, contains_pitch_class) {
  OrderedChord chord("C4 E4 G4");
  EXPECT_EQ(chord.contains_pitch_class(PitchClass::C), true);
  EXPECT_EQ(chord.contains_pitch_class(PitchClass::E), true);
  EXPECT_EQ(chord.contains_pitch_class(PitchClass::G), true);
  EXPECT_EQ(chord.contains_pitch_class(PitchClass::A), false);
}

TEST(ChordRW, contains_pitch) {
  OrderedChord chord("C2 E3 G#4");
  EXPECT_EQ(chord.contains_pitch(Pitch("C2")), true);
  EXPECT_EQ(chord.contains_pitch(Pitch("E3")), true);
  EXPECT_EQ(chord.contains_pitch(Pitch("G#4")), true);
  EXPECT_EQ(chord.contains_pitch(Pitch("A4")), false);
  EXPECT_EQ(chord.contains_pitch(Pitch("C4")), false);
}

TEST(ChordRW, calculate_statistics1) {
  OrderedChord chord("C4 E4 G4");
  auto chord_statistics = calculate_statistics(chord);
  EXPECT_EQ(chord_statistics.num_of_pitches, 3);
  EXPECT_EQ(chord_statistics.num_of_unique_pitch_classes, 3);
  ASSERT_NEAR(chord_statistics.tension, 1.4, 0.01);
  ASSERT_NEAR(chord_statistics.thickness, 0.0, 0.01);
  EXPECT_EQ(chord_statistics.root, PitchClass::C);
  ASSERT_NEAR(chord_statistics.geometrical_center, 0.5238095238095235, 0.01);
}

TEST(ChordRW, calculate_statistics2) {
  OrderedChord chord("G4 C4 E4 G4 E4");
  auto chord_statistics = calculate_statistics(chord);
  EXPECT_EQ(chord_statistics.num_of_pitches, 5);
  EXPECT_EQ(chord_statistics.num_of_unique_pitch_classes, 3);
  ASSERT_NEAR(chord_statistics.tension, 1.4, 0.01);
  ASSERT_NEAR(chord_statistics.thickness, 0.0, 0.01);
  EXPECT_EQ(chord_statistics.root, PitchClass::C);
  ASSERT_NEAR(chord_statistics.geometrical_center, 0.5238095238095235, 0.01);
}

TEST(ChordRW, calculate_statistics3) {
  OrderedChord chord("F4 A4 C5 D5 E5");
  auto chord_statistics = calculate_statistics(chord);
  EXPECT_EQ(chord_statistics.num_of_pitches, 5);
  EXPECT_EQ(chord_statistics.num_of_unique_pitch_classes, 5);
  ASSERT_NEAR(chord_statistics.thickness, 0.0, 0.01);
  EXPECT_EQ(chord_statistics.root, PitchClass::F); // Although not sure why
  ASSERT_NEAR(chord_statistics.geometrical_center, 0.56363636363636394, 0.01);
}

TEST(ChordRW, calculate_statistics4) {
  OrderedChord chord("D4 F4 A4 C5 E5");
  auto chord_statistics = calculate_statistics(chord);
  EXPECT_EQ(chord_statistics.num_of_pitches, 5);
  EXPECT_EQ(chord_statistics.num_of_unique_pitch_classes, 5);
  ASSERT_NEAR(chord_statistics.thickness, 0.0, 0.01);
  EXPECT_EQ(chord_statistics.root, PitchClass::D);
  ASSERT_NEAR(chord_statistics.geometrical_center, 0.48571428571428549, 0.01);
}

TEST(ChordRW, calculate_statistics5) {
  OrderedChord chord("C3 G3 C4 E4");
  auto chord_statistics = calculate_statistics(chord);
  EXPECT_EQ(chord_statistics.num_of_pitches, 4);
  EXPECT_EQ(chord_statistics.num_of_unique_pitch_classes, 4);
  ASSERT_NEAR(chord_statistics.thickness, 1.0, 0.01);
  EXPECT_EQ(chord_statistics.root, PitchClass::C);
  ASSERT_NEAR(chord_statistics.geometrical_center, 0.546875, 0.01);
}

TEST(ChordRW, pitchset_circle_of_fifths_ordering) {
  PitchSet ps("C4 E4 G4");
  auto ordered = ps.get_pitch_classes_ordered_by_circle_of_fifths();
  ASSERT_EQ(ordered.size(), 3);
  // Chroma order: C=0, G=1, E=4 (sorted ascending)
  EXPECT_EQ(ordered[0], PitchClass::C);
  EXPECT_EQ(ordered[1], PitchClass::G);
  EXPECT_EQ(ordered[2], PitchClass::E);
}

TEST(ChordRW, statistics_alignment) {
  OrderedChord chord("C4 E4 G4");
  auto stats = calculate_statistics(chord);
  vector<int> expected_alignment = {1, 3, 5};
  EXPECT_EQ(stats.alignment, expected_alignment);
}

TEST(ChordRW, statistics_count_vec) {
  OrderedChord chord("C4 E4 G4");
  auto stats = calculate_statistics(chord);
  vector<int> expected_count_vec = {0, 0, 1, 1, 1, 0};
  EXPECT_EQ(stats.count_vec, expected_count_vec);
}

TEST(ChordRW, pitchclass_to_string) {
  EXPECT_EQ(PitchClass::C.to_string(), "C");
  EXPECT_EQ(PitchClass::Fs.to_string(), "F#");
  EXPECT_EQ(PitchClass::Bb.to_string(), "Bb");
  EXPECT_EQ(PitchClass::G.to_string(), "G");
  EXPECT_EQ(PitchClass::D.to_string(), "D");
  EXPECT_EQ(PitchClass::A.to_string(), "A");
  EXPECT_EQ(PitchClass::E.to_string(), "E");
  EXPECT_EQ(PitchClass::B.to_string(), "B");
  EXPECT_EQ(PitchClass::F.to_string(), "F");
}

TEST(ChordRW, pitch_to_string) {
  EXPECT_EQ(Pitch("C4").to_string(), "C4");
  EXPECT_EQ(Pitch("F#5").to_string(), "F#5");
}

// ── Utility helper tests ──────────────────────────────────────────

TEST(Utility, set_union) {
  vector<int> result = set_union({1, 3, 5}, {2, 3, 6});
  vector<int> expected = {1, 2, 3, 5, 6};
  EXPECT_EQ(result, expected);
}

TEST(Utility, set_complement) {
  vector<int> result = set_complement({1, 3, 5, 7}, {3, 7});
  vector<int> expected = {1, 5};
  EXPECT_EQ(result, expected);
}

TEST(Utility, set_intersect) {
  vector<int> result = set_intersect({1, 2, 3}, {2, 3, 4});
  vector<int> expected = {2, 3};
  EXPECT_EQ(result, expected);
}

TEST(Utility, sign_function) {
  EXPECT_EQ(sign(-5.0), -1);
  EXPECT_EQ(sign(0.0), 0);
  EXPECT_EQ(sign(3.0), 1);
}

// ── Bigram component tests ─────────────────────────────────────────

TEST(Bigram, root_movement_C_to_G) {
  // C->G: distance = 7 semitones, shorter way = 5
  OrderedChord c_maj("C4 E4 G4");
  OrderedChord g_maj("B3 D4 G4");
  auto c_stats = calculate_statistics(c_maj);
  auto g_stats = calculate_statistics(g_maj);
  // vec and sv are Tier 3 concerns; provide reasonable dummy values
  vector<int> vec = {-1, -2, 0};
  int sv = 3;
  auto result = calculate_bigram_statistics(
      c_maj, g_maj, c_stats, g_stats, vec, sv, 4, 0.0, {});
  EXPECT_EQ(result.root_movement, 5); // C->G = 5 semitones (shorter way)
}

TEST(Bigram, root_movement_C_to_Fs) {
  // C->F#: distance = 6 (tritone)
  OrderedChord c_maj("C4 E4 G4");
  OrderedChord fs_maj("F#4 A4 D-5");
  auto c_stats = calculate_statistics(c_maj);
  auto fs_stats = calculate_statistics(fs_maj);
  vector<int> vec = {6, 5, 5};
  int sv = 16;
  auto result = calculate_bigram_statistics(
      c_maj, fs_maj, c_stats, fs_stats, vec, sv, 6, 0.0, {});
  EXPECT_EQ(result.root_movement, 6);
}

TEST(Bigram, common_notes_C_to_F) {
  // C4 E4 G4 -> C4 F4 A4: C4 is shared
  OrderedChord c_maj("C4 E4 G4");
  OrderedChord f_maj("C4 F4 A4");
  auto c_stats = calculate_statistics(c_maj);
  auto f_stats = calculate_statistics(f_maj);
  vector<int> vec = {0, 1, 2};
  int sv = 3;
  auto result = calculate_bigram_statistics(
      c_maj, f_maj, c_stats, f_stats, vec, sv, 4, 0.0, {});
  EXPECT_EQ(result.common_note, 1);
}

TEST(Bigram, voice_movement_counts) {
  OrderedChord c_maj("C4 E4 G4");
  OrderedChord f_maj("C4 F4 A4");
  auto c_stats = calculate_statistics(c_maj);
  auto f_stats = calculate_statistics(f_maj);
  vector<int> vec = {0, 1, 2}; // steady, ascending, ascending
  int sv = 3;
  auto result = calculate_bigram_statistics(
      c_maj, f_maj, c_stats, f_stats, vec, sv, 4, 0.0, {});
  EXPECT_EQ(result.steady_count, 1);
  EXPECT_EQ(result.ascending_count, 2);
  EXPECT_EQ(result.descending_count, 0);
}

TEST(Bigram, similarity_same_root) {
  // Two chords with same root should get boosted similarity (sqrt)
  OrderedChord c1("C4 E4 G4");
  OrderedChord c2("C4 E4 G4");
  auto stats1 = calculate_statistics(c1);
  auto stats2 = calculate_statistics(c2);
  vector<int> vec = {0, 0, 0};
  int sv = 0;
  auto result = calculate_bigram_statistics(
      c1, c2, stats1, stats2, vec, sv, 4, 0.0, {});
  EXPECT_EQ(result.similarity, 100); // perfect similarity
}

TEST(Bigram, initial_chord_span_only) {
  // When prev_single_chroma is empty, should compute span but sspan=0
  OrderedChord c_maj("C4 E4 G4");
  OrderedChord f_maj("C4 F4 A4");
  auto c_stats = calculate_statistics(c_maj);
  auto f_stats = calculate_statistics(f_maj);
  vector<int> vec = {0, 1, 2};
  int sv = 3;
  auto result = calculate_bigram_statistics(
      c_maj, f_maj, c_stats, f_stats, vec, sv, 4, 0.0, {});
  EXPECT_EQ(result.sspan, 0); // initial: no sspan
  EXPECT_GT(result.span, 0); // span should be computed
}

TEST(Bigram, name_generation_C_major) {
  OrderedChord c_maj("C4 E4 G4");
  OrderedChord f_maj("C4 F4 A4");
  auto c_stats = calculate_statistics(c_maj);
  auto f_stats = calculate_statistics(f_maj);
  vector<int> vec = {0, 1, 2};
  int sv = 3;
  auto result = calculate_bigram_statistics(
      c_maj, f_maj, c_stats, f_stats, vec, sv, 4, 0.0, {});
  // F major chord: notes should be named C F A
  EXPECT_EQ(result.name, "C F A");
  EXPECT_EQ(result.name_with_octave, "C4 F4 A4");
  EXPECT_EQ(result.root_name, "F");
}

TEST(Bigram, bigram_C_to_F_integration) {
  // C major → F major (C4 E4 G4 → C4 F4 A4)
  OrderedChord c_maj("C4 E4 G4");
  OrderedChord f_maj("C4 F4 A4");
  auto c_stats = calculate_statistics(c_maj);
  auto f_stats = calculate_statistics(f_maj);
  vector<int> vec = {0, 1, 2};
  int sv = 3;
  // First compute initial chord (C major) to get its single_chroma/chroma_old
  auto c_bigram = calculate_bigram_statistics(
      c_maj, c_maj, c_stats, c_stats, {0, 0, 0}, 0, 4, 0.0, {});

  // Now compute C -> F bigram using C's single_chroma and chroma_old
  auto result = calculate_bigram_statistics(
      c_maj, f_maj, c_stats, f_stats, vec, sv, 4,
      c_bigram.chroma_old, c_bigram.single_chroma);

  EXPECT_EQ(result.common_note, 1);
  EXPECT_EQ(result.sv, 3);
  EXPECT_GT(result.span, 0);
  EXPECT_GT(result.sspan, 0); // non-initial, should have sspan
  EXPECT_EQ(result.root_movement, 5); // C->F = 5 semitones
  EXPECT_EQ(result.name, "C F A");
  EXPECT_EQ(result.root_name, "F");
  // Verify pitch_class_set is correct for F major: {0, 5, 9} (C, F, A)
  vector<int> expected_pcs = {0, 5, 9};
  EXPECT_EQ(result.pitch_class_set, expected_pcs);
}

TEST(Bigram, bigram_C_to_G_integration) {
  // C major → G major (C4 E4 G4 → B3 D4 G4)
  OrderedChord c_maj("C4 E4 G4");
  OrderedChord g_maj("B3 D4 G4");
  auto c_stats = calculate_statistics(c_maj);
  auto g_stats = calculate_statistics(g_maj);
  vector<int> vec = {-1, -2, 0};
  int sv = 3;
  auto c_bigram = calculate_bigram_statistics(
      c_maj, c_maj, c_stats, c_stats, {0, 0, 0}, 0, 4, 0.0, {});

  auto result = calculate_bigram_statistics(
      c_maj, g_maj, c_stats, g_stats, vec, sv, 4,
      c_bigram.chroma_old, c_bigram.single_chroma);

  EXPECT_EQ(result.common_note, 1); // B3 D4 G4 shares G4 (MIDI 67) with C4 E4 G4
  EXPECT_EQ(result.sv, 3);
  EXPECT_EQ(result.root_movement, 5); // C->G = 5 (shorter way around)
  // G major should have notes B D G
  EXPECT_EQ(result.name, "B D G");
  EXPECT_EQ(result.root_name, "G");
}

TEST(Bigram, self_diff_and_count_vec_from_stats) {
  // Verify bigram carries over Tier 1 self_diff and count_vec from curr_stats
  OrderedChord c_maj("C4 E4 G4");
  OrderedChord f_maj("C4 F4 A4");
  auto c_stats = calculate_statistics(c_maj);
  auto f_stats = calculate_statistics(f_maj);
  vector<int> vec = {0, 1, 2};
  int sv = 3;
  auto result = calculate_bigram_statistics(
      c_maj, f_maj, c_stats, f_stats, vec, sv, 4, 0.0, {});
  EXPECT_EQ(result.self_diff, f_stats.self_diff);
  EXPECT_EQ(result.count_vec, f_stats.count_vec);
  EXPECT_EQ(result.alignment, f_stats.alignment);
}
