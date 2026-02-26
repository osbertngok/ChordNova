#include <gtest/gtest.h>
#include "algorithm/sorting.h"
#include "model/bigramchordstatistics.h"
#include "model/orderedchord.h"

using namespace chordnovarw::algorithm;
using namespace chordnovarw::model;

namespace {

// Helper to build a CandidateEntry with controlled stats values.
// Most fields are zeroed; callers set the fields they care about via the
// named parameters below.
struct StatsBuilder {
  double chroma_old = 0.0;
  double prev_chroma_old = 0.0;
  double chroma = 0.0;
  double Q_indicator = 0.0;
  int common_note = 0;
  int sv = 0;
  int span = 0;
  int sspan = 0;
  int similarity = 0;
  int sim_orig = 100;
  int steady_count = 0;
  int ascending_count = 0;
  int descending_count = 0;
  int root_movement = 0;
  std::string root_name = "C";
  std::string name = "C E G";
  std::string name_with_octave = "C4 E4 G4";
  std::vector<int> notes = {60, 64, 67};
  std::vector<int> pitch_class_set = {0, 4, 7};

  BigramChordStatistics build() const {
    return BigramChordStatistics(
        chroma_old, prev_chroma_old, chroma, Q_indicator,
        common_note, sv, span, sspan, similarity, sim_orig,
        steady_count, ascending_count, descending_count, root_movement,
        root_name, false, name, name_with_octave,
        OverflowState::NoOverflow, 0,
        notes, pitch_class_set,
        /*single_chroma=*/{}, /*vec=*/{}, /*self_diff=*/{},
        /*count_vec=*/{}, /*alignment=*/{}
    );
  }
};

CandidateEntry make_entry(const std::string& chord_str, const StatsBuilder& sb) {
  return CandidateEntry{OrderedChord(chord_str), sb.build()};
}

} // anonymous namespace

// ── Empty / single-element edge cases ────────────────────────────

TEST(SortingTest, EmptyCandidatesNoOp) {
  std::vector<CandidateEntry> candidates;
  sort_candidates(candidates, "T");
  EXPECT_TRUE(candidates.empty());
}

TEST(SortingTest, EmptySortOrderNoOp) {
  std::vector<CandidateEntry> candidates;
  StatsBuilder sb;
  candidates.push_back(make_entry("C4 E4 G4", sb));
  sort_candidates(candidates, "");
  ASSERT_EQ(candidates.size(), 1u);
}

TEST(SortingTest, SingleCandidateNoOp) {
  std::vector<CandidateEntry> candidates;
  StatsBuilder sb;
  sb.sv = 5;
  candidates.push_back(make_entry("C4 E4 G4", sb));
  sort_candidates(candidates, "S");
  ASSERT_EQ(candidates.size(), 1u);
  EXPECT_EQ(candidates[0].stats.sv, 5);
}

// ── Single-key descending (default) ─────────────────────────────

TEST(SortingTest, SortBySvDescending) {
  std::vector<CandidateEntry> candidates;
  StatsBuilder sb1; sb1.sv = 3;
  StatsBuilder sb2; sb2.sv = 7;
  StatsBuilder sb3; sb3.sv = 1;
  candidates.push_back(make_entry("C4 E4 G4", sb1));
  candidates.push_back(make_entry("D4 F4 A4", sb2));
  candidates.push_back(make_entry("E4 G4 B4", sb3));

  sort_candidates(candidates, "S");

  EXPECT_EQ(candidates[0].stats.sv, 7);
  EXPECT_EQ(candidates[1].stats.sv, 3);
  EXPECT_EQ(candidates[2].stats.sv, 1);
}

// ── Single-key ascending (+) ────────────────────────────────────

TEST(SortingTest, SortBySvAscending) {
  std::vector<CandidateEntry> candidates;
  StatsBuilder sb1; sb1.sv = 3;
  StatsBuilder sb2; sb2.sv = 7;
  StatsBuilder sb3; sb3.sv = 1;
  candidates.push_back(make_entry("C4 E4 G4", sb1));
  candidates.push_back(make_entry("D4 F4 A4", sb2));
  candidates.push_back(make_entry("E4 G4 B4", sb3));

  sort_candidates(candidates, "S+");

  EXPECT_EQ(candidates[0].stats.sv, 1);
  EXPECT_EQ(candidates[1].stats.sv, 3);
  EXPECT_EQ(candidates[2].stats.sv, 7);
}

// ── Sort by common_note (C key) ─────────────────────────────────

TEST(SortingTest, SortByCommonNoteDescending) {
  std::vector<CandidateEntry> candidates;
  StatsBuilder sb1; sb1.common_note = 2;
  StatsBuilder sb2; sb2.common_note = 0;
  StatsBuilder sb3; sb3.common_note = 3;
  candidates.push_back(make_entry("C4 E4 G4", sb1));
  candidates.push_back(make_entry("D4 F4 A4", sb2));
  candidates.push_back(make_entry("E4 G4 B4", sb3));

  sort_candidates(candidates, "C");

  EXPECT_EQ(candidates[0].stats.common_note, 3);
  EXPECT_EQ(candidates[1].stats.common_note, 2);
  EXPECT_EQ(candidates[2].stats.common_note, 0);
}

// ── Sort by chroma (K key) ──────────────────────────────────────

TEST(SortingTest, SortByChromaDescending) {
  std::vector<CandidateEntry> candidates;
  StatsBuilder sb1; sb1.chroma = 10.5;
  StatsBuilder sb2; sb2.chroma = -3.2;
  StatsBuilder sb3; sb3.chroma = 50.0;
  candidates.push_back(make_entry("C4 E4 G4", sb1));
  candidates.push_back(make_entry("D4 F4 A4", sb2));
  candidates.push_back(make_entry("E4 G4 B4", sb3));

  sort_candidates(candidates, "K");

  EXPECT_DOUBLE_EQ(candidates[0].stats.chroma, 50.0);
  EXPECT_DOUBLE_EQ(candidates[1].stats.chroma, 10.5);
  EXPECT_DOUBLE_EQ(candidates[2].stats.chroma, -3.2);
}

// ── Sort by Q_indicator (Q key) ─────────────────────────────────

TEST(SortingTest, SortByQIndicatorAscending) {
  std::vector<CandidateEntry> candidates;
  StatsBuilder sb1; sb1.Q_indicator = 20.0;
  StatsBuilder sb2; sb2.Q_indicator = -5.0;
  StatsBuilder sb3; sb3.Q_indicator = 10.0;
  candidates.push_back(make_entry("C4 E4 G4", sb1));
  candidates.push_back(make_entry("D4 F4 A4", sb2));
  candidates.push_back(make_entry("E4 G4 B4", sb3));

  sort_candidates(candidates, "Q+");

  EXPECT_DOUBLE_EQ(candidates[0].stats.Q_indicator, -5.0);
  EXPECT_DOUBLE_EQ(candidates[1].stats.Q_indicator, 10.0);
  EXPECT_DOUBLE_EQ(candidates[2].stats.Q_indicator, 20.0);
}

// ── Sort by similarity (X key) ──────────────────────────────────

TEST(SortingTest, SortBySimilarityDescending) {
  std::vector<CandidateEntry> candidates;
  StatsBuilder sb1; sb1.similarity = 80;
  StatsBuilder sb2; sb2.similarity = 95;
  StatsBuilder sb3; sb3.similarity = 60;
  candidates.push_back(make_entry("C4 E4 G4", sb1));
  candidates.push_back(make_entry("D4 F4 A4", sb2));
  candidates.push_back(make_entry("E4 G4 B4", sb3));

  sort_candidates(candidates, "X");

  EXPECT_EQ(candidates[0].stats.similarity, 95);
  EXPECT_EQ(candidates[1].stats.similarity, 80);
  EXPECT_EQ(candidates[2].stats.similarity, 60);
}

// ── Sort by root_movement (R key) ───────────────────────────────

TEST(SortingTest, SortByRootMovementAscending) {
  std::vector<CandidateEntry> candidates;
  StatsBuilder sb1; sb1.root_movement = 5;
  StatsBuilder sb2; sb2.root_movement = 1;
  StatsBuilder sb3; sb3.root_movement = 3;
  candidates.push_back(make_entry("C4 E4 G4", sb1));
  candidates.push_back(make_entry("D4 F4 A4", sb2));
  candidates.push_back(make_entry("E4 G4 B4", sb3));

  sort_candidates(candidates, "R+");

  EXPECT_EQ(candidates[0].stats.root_movement, 1);
  EXPECT_EQ(candidates[1].stats.root_movement, 3);
  EXPECT_EQ(candidates[2].stats.root_movement, 5);
}

// ── Sort by span (a key) ────────────────────────────────────────

TEST(SortingTest, SortBySpanDescending) {
  std::vector<CandidateEntry> candidates;
  StatsBuilder sb1; sb1.span = 4;
  StatsBuilder sb2; sb2.span = 8;
  StatsBuilder sb3; sb3.span = 2;
  candidates.push_back(make_entry("C4 E4 G4", sb1));
  candidates.push_back(make_entry("D4 F4 A4", sb2));
  candidates.push_back(make_entry("E4 G4 B4", sb3));

  sort_candidates(candidates, "a");

  EXPECT_EQ(candidates[0].stats.span, 8);
  EXPECT_EQ(candidates[1].stats.span, 4);
  EXPECT_EQ(candidates[2].stats.span, 2);
}

// ── Sort by sspan (A key) ───────────────────────────────────────

TEST(SortingTest, SortBySspanAscending) {
  std::vector<CandidateEntry> candidates;
  StatsBuilder sb1; sb1.sspan = 6;
  StatsBuilder sb2; sb2.sspan = 10;
  StatsBuilder sb3; sb3.sspan = 3;
  candidates.push_back(make_entry("C4 E4 G4", sb1));
  candidates.push_back(make_entry("D4 F4 A4", sb2));
  candidates.push_back(make_entry("E4 G4 B4", sb3));

  sort_candidates(candidates, "A+");

  EXPECT_EQ(candidates[0].stats.sspan, 3);
  EXPECT_EQ(candidates[1].stats.sspan, 6);
  EXPECT_EQ(candidates[2].stats.sspan, 10);
}

// ── Sort by sim_orig (P key) ────────────────────────────────────

TEST(SortingTest, SortBySimOrigDescending) {
  std::vector<CandidateEntry> candidates;
  StatsBuilder sb1; sb1.sim_orig = 70;
  StatsBuilder sb2; sb2.sim_orig = 100;
  StatsBuilder sb3; sb3.sim_orig = 50;
  candidates.push_back(make_entry("C4 E4 G4", sb1));
  candidates.push_back(make_entry("D4 F4 A4", sb2));
  candidates.push_back(make_entry("E4 G4 B4", sb3));

  sort_candidates(candidates, "P");

  EXPECT_EQ(candidates[0].stats.sim_orig, 100);
  EXPECT_EQ(candidates[1].stats.sim_orig, 70);
  EXPECT_EQ(candidates[2].stats.sim_orig, 50);
}

// ── Sort by chroma_old (k key) ──────────────────────────────────

TEST(SortingTest, SortByChromaOldDescending) {
  std::vector<CandidateEntry> candidates;
  StatsBuilder sb1; sb1.chroma_old = 1.5;
  StatsBuilder sb2; sb2.chroma_old = -2.0;
  StatsBuilder sb3; sb3.chroma_old = 4.0;
  candidates.push_back(make_entry("C4 E4 G4", sb1));
  candidates.push_back(make_entry("D4 F4 A4", sb2));
  candidates.push_back(make_entry("E4 G4 B4", sb3));

  sort_candidates(candidates, "k");

  EXPECT_DOUBLE_EQ(candidates[0].stats.chroma_old, 4.0);
  EXPECT_DOUBLE_EQ(candidates[1].stats.chroma_old, 1.5);
  EXPECT_DOUBLE_EQ(candidates[2].stats.chroma_old, -2.0);
}

// ── Multi-key sort (right-to-left priority) ─────────────────────

TEST(SortingTest, MultiKeySortPrimarySecondary) {
  // Sort order "SC" → read right-to-left: first sort by C (common_note desc),
  // then by S (sv desc). S is the primary (last applied, highest priority).
  std::vector<CandidateEntry> candidates;

  StatsBuilder sb1; sb1.sv = 5; sb1.common_note = 2;
  StatsBuilder sb2; sb2.sv = 5; sb2.common_note = 3;
  StatsBuilder sb3; sb3.sv = 3; sb3.common_note = 3;
  candidates.push_back(make_entry("C4 E4 G4", sb1));
  candidates.push_back(make_entry("D4 F4 A4", sb2));
  candidates.push_back(make_entry("E4 G4 B4", sb3));

  sort_candidates(candidates, "SC");

  // Primary sort by S desc: sv=5 before sv=3.
  // Among sv=5 ties, stable_sort preserves earlier secondary order (C desc):
  // common_note=3 before common_note=2.
  EXPECT_EQ(candidates[0].stats.sv, 5);
  EXPECT_EQ(candidates[0].stats.common_note, 3);
  EXPECT_EQ(candidates[1].stats.sv, 5);
  EXPECT_EQ(candidates[1].stats.common_note, 2);
  EXPECT_EQ(candidates[2].stats.sv, 3);
  EXPECT_EQ(candidates[2].stats.common_note, 3);
}

TEST(SortingTest, MultiKeyWithMixedAscendingDescending) {
  // "S+C" → read right-to-left: first sort by C (common_note desc),
  // then by S ascending (primary).
  std::vector<CandidateEntry> candidates;

  StatsBuilder sb1; sb1.sv = 5; sb1.common_note = 2;
  StatsBuilder sb2; sb2.sv = 3; sb2.common_note = 3;
  StatsBuilder sb3; sb3.sv = 5; sb3.common_note = 1;
  candidates.push_back(make_entry("C4 E4 G4", sb1));
  candidates.push_back(make_entry("D4 F4 A4", sb2));
  candidates.push_back(make_entry("E4 G4 B4", sb3));

  sort_candidates(candidates, "S+C");

  // Primary: S ascending → sv=3 first, then two sv=5.
  // Among sv=5 ties, stable sort preserves secondary C desc order:
  // common_note=2 before common_note=1.
  EXPECT_EQ(candidates[0].stats.sv, 3);
  EXPECT_EQ(candidates[1].stats.sv, 5);
  EXPECT_EQ(candidates[1].stats.common_note, 2);
  EXPECT_EQ(candidates[2].stats.sv, 5);
  EXPECT_EQ(candidates[2].stats.common_note, 1);
}

// ── Unknown key character is ignored ────────────────────────────

TEST(SortingTest, UnknownKeyIgnored) {
  std::vector<CandidateEntry> candidates;
  StatsBuilder sb1; sb1.sv = 3;
  StatsBuilder sb2; sb2.sv = 7;
  candidates.push_back(make_entry("C4 E4 G4", sb1));
  candidates.push_back(make_entry("D4 F4 A4", sb2));

  // 'Z' is unknown, should be skipped; no sorting applied
  sort_candidates(candidates, "Z");

  EXPECT_EQ(candidates[0].stats.sv, 3);
  EXPECT_EQ(candidates[1].stats.sv, 7);
}

// ── Stability test: equal elements preserve original order ──────

TEST(SortingTest, StableSortPreservesOrder) {
  std::vector<CandidateEntry> candidates;
  StatsBuilder sb1; sb1.sv = 5; sb1.common_note = 1;
  StatsBuilder sb2; sb2.sv = 5; sb2.common_note = 2;
  StatsBuilder sb3; sb3.sv = 5; sb3.common_note = 3;
  candidates.push_back(make_entry("C4 E4 G4", sb1));
  candidates.push_back(make_entry("D4 F4 A4", sb2));
  candidates.push_back(make_entry("E4 G4 B4", sb3));

  // Sort by S only; all sv are equal, so original order should be preserved
  sort_candidates(candidates, "S");

  EXPECT_EQ(candidates[0].stats.common_note, 1);
  EXPECT_EQ(candidates[1].stats.common_note, 2);
  EXPECT_EQ(candidates[2].stats.common_note, 3);
}

// ── V key is alias for R (root_movement) ────────────────────────

TEST(SortingTest, VKeyIsAliasForR) {
  std::vector<CandidateEntry> candidates;
  StatsBuilder sb1; sb1.root_movement = 4;
  StatsBuilder sb2; sb2.root_movement = 1;
  candidates.push_back(make_entry("C4 E4 G4", sb1));
  candidates.push_back(make_entry("D4 F4 A4", sb2));

  sort_candidates(candidates, "V");

  EXPECT_EQ(candidates[0].stats.root_movement, 4);
  EXPECT_EQ(candidates[1].stats.root_movement, 1);
}
