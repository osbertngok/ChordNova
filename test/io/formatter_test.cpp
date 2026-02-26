#include <gtest/gtest.h>
#include <sstream>
#include "io/formatter.h"
#include "model/orderedchord.h"
#include "model/bigramchordstatistics.h"

using namespace chordnovarw::io;
using namespace chordnovarw::algorithm;
using namespace chordnovarw::model;

namespace {

// Build a CandidateEntry with controlled stats for formatter testing.
CandidateEntry make_entry(const std::string& chord_str,
                          const std::string& name_with_octave,
                          const std::string& root_name,
                          int sv, double chroma, int span,
                          int similarity, double q_indicator) {
  BigramChordStatistics stats(
      /*chroma_old=*/0.0, /*prev_chroma_old=*/0.0, /*chroma=*/chroma,
      /*Q_indicator=*/q_indicator, /*common_note=*/0, /*sv=*/sv,
      /*span=*/span, /*sspan=*/0, /*similarity=*/similarity, /*sim_orig=*/100,
      /*steady_count=*/0, /*ascending_count=*/0, /*descending_count=*/0,
      /*root_movement=*/0, /*root_name=*/root_name, /*hide_octave=*/false,
      /*name=*/"", /*name_with_octave=*/name_with_octave,
      /*overflow_state=*/OverflowState::NoOverflow, /*overflow_amount=*/0,
      /*notes=*/{}, /*pitch_class_set=*/{},
      /*single_chroma=*/{}, /*vec=*/{}, /*self_diff=*/{},
      /*count_vec=*/{}, /*alignment=*/{}
  );
  return CandidateEntry{OrderedChord(chord_str), std::move(stats)};
}

} // anonymous namespace

TEST(FormatterTest, EmptyCandidateList) {
  std::ostringstream out;
  std::vector<CandidateEntry> candidates;
  format_candidates(out, candidates);
  EXPECT_EQ(out.str(), "");
}

TEST(FormatterTest, SingleCandidateWithNumbering) {
  std::ostringstream out;
  std::vector<CandidateEntry> candidates;
  candidates.push_back(make_entry("C4 E4 G4", "C4 E4 G4", "C",
                                   5, 1.5, 4, 80, 3.2));
  format_candidates(out, candidates, 1);
  std::string result = out.str();

  EXPECT_NE(result.find("1. "), std::string::npos);
  EXPECT_NE(result.find("C4 E4 G4"), std::string::npos);
  EXPECT_NE(result.find("sv=5"), std::string::npos);
  EXPECT_NE(result.find("root=C"), std::string::npos);
  EXPECT_EQ(result.back(), '\n');
}

TEST(FormatterTest, NoNumberingWhenStartIndexZero) {
  std::ostringstream out;
  std::vector<CandidateEntry> candidates;
  candidates.push_back(make_entry("C4 E4 G4", "C4 E4 G4", "C",
                                   5, 1.5, 4, 80, 3.2));
  format_candidates(out, candidates, 0);
  std::string result = out.str();

  EXPECT_EQ(result.find("1. "), std::string::npos);
  EXPECT_NE(result.find("C4 E4 G4"), std::string::npos);
}

TEST(FormatterTest, MultipleCandidatesNumberedSequentially) {
  std::ostringstream out;
  std::vector<CandidateEntry> candidates;
  candidates.push_back(make_entry("C4 E4 G4", "C4 E4 G4", "C",
                                   5, 1.0, 4, 80, 3.0));
  candidates.push_back(make_entry("D4 F4 A4", "D4 F4 A4", "D",
                                   3, 2.0, 3, 60, 1.0));
  format_candidates(out, candidates, 1);
  std::string result = out.str();

  EXPECT_NE(result.find("1. "), std::string::npos);
  EXPECT_NE(result.find("2. "), std::string::npos);
}

TEST(FormatterTest, CustomStartIndex) {
  std::ostringstream out;
  std::vector<CandidateEntry> candidates;
  candidates.push_back(make_entry("C4 E4 G4", "C4 E4 G4", "C",
                                   5, 1.0, 4, 80, 3.0));
  format_candidates(out, candidates, 10);
  std::string result = out.str();

  EXPECT_NE(result.find("10. "), std::string::npos);
}

TEST(FormatterTest, EmptyRootNameOmitsRootField) {
  std::ostringstream out;
  std::vector<CandidateEntry> candidates;
  candidates.push_back(make_entry("C4 E4 G4", "C4 E4 G4", "",
                                   5, 1.0, 4, 80, 3.0));
  format_candidates(out, candidates);
  std::string result = out.str();

  EXPECT_EQ(result.find("root="), std::string::npos);
}

TEST(FormatterTest, FloatingPointPrecision) {
  std::ostringstream out;
  std::vector<CandidateEntry> candidates;
  candidates.push_back(make_entry("C4 E4 G4", "C4 E4 G4", "C",
                                   5, 1.23456, 4, 80, -3.7));
  format_candidates(out, candidates);
  std::string result = out.str();

  // Chroma and Q should have 1 decimal place
  EXPECT_NE(result.find("k=1.2"), std::string::npos);
  EXPECT_NE(result.find("Q=-3.7"), std::string::npos);
}
