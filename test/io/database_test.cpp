#include <gtest/gtest.h>
#include "io/database.h"
#include <cstdio>
#include <fstream>
#include <algorithm>

using namespace chordnovarw::io;

namespace {

// Helper: create a temporary file with given content and return its path.
class TempFile {
public:
  explicit TempFile(const std::string& content) {
    path_ = std::tmpnam(nullptr);
    std::ofstream out(path_);
    out << content;
  }
  ~TempFile() { std::remove(path_.c_str()); }
  const std::string& path() const { return path_; }
private:
  std::string path_;
};

} // anonymous namespace

// ── read_chord_database ─────────────────────────────────────────

TEST(DatabaseTest, NonexistentFileReturnsEmpty) {
  auto result = read_chord_database("/nonexistent/path/to/file.txt");
  EXPECT_TRUE(result.empty());
}

TEST(DatabaseTest, EmptyFileReturnsEmpty) {
  TempFile f("");
  auto result = read_chord_database(f.path());
  EXPECT_TRUE(result.empty());
}

TEST(DatabaseTest, CommentLinesSkipped) {
  // Lines starting with '/' or 't' are comments
  TempFile f("// This is a comment\ntThis is also skipped\n");
  auto result = read_chord_database(f.path());
  EXPECT_TRUE(result.empty());
}

TEST(DatabaseTest, SingleNoteChord) {
  // Single note: pitch class 0 (C). Should produce 12 transpositions.
  // Each transposition is (1 << ((0 + j) % 12)) for j=0..11,
  // which gives {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048}.
  TempFile f("0\n");
  auto result = read_chord_database(f.path());
  ASSERT_EQ(result.size(), 12u);
  // Check that all 12 single-bit values are present
  for (int i = 0; i < 12; ++i) {
    EXPECT_TRUE(std::binary_search(result.begin(), result.end(), 1 << i))
        << "Missing bitmask for transposition " << i;
  }
}

TEST(DatabaseTest, TwoNoteChord) {
  // Pitch classes 0, 7 (C and G, perfect fifth).
  // Bitmask for j=0: (1<<0) + (1<<7) = 1 + 128 = 129
  TempFile f("0 7\n");
  auto result = read_chord_database(f.path());
  // 12 transpositions, no omissions for 2-note chords
  ASSERT_EQ(result.size(), 12u);
  // Check transposition at j=0: bitmask should be (1<<0)+(1<<7) = 129
  EXPECT_TRUE(std::binary_search(result.begin(), result.end(), 129));
}

TEST(DatabaseTest, ThreeNoteChordWithOmissions) {
  // Major triad: 0 4 7 (C E G).
  // 3-note chords: essential degrees are {1, 3, 5} → root(1), 3rd(3), 5th(5).
  // note_pos maps: 0→1(root), 4→3(3rd), 7→5(5th).
  // All are essential, so no omissible notes → omit_choice is empty.
  // Only 12 transpositions of the full chord.
  TempFile f("0 4 7\n");
  auto result = read_chord_database(f.path());
  ASSERT_EQ(result.size(), 12u);
  // Check transposition at j=0: (1<<0)+(1<<4)+(1<<7) = 1+16+128 = 145
  EXPECT_TRUE(std::binary_search(result.begin(), result.end(), 145));
}

TEST(DatabaseTest, FourNoteChordWithOmissions) {
  // Dom7: 0 4 7 10 (C E G Bb).
  // 4-note chords: essential degrees are {1, 3, 7}.
  // note_pos: 0→1(root), 4→3(3rd), 7→5(5th), 10→7(7th).
  // Root(1), 3rd(3), 7th(7) are essential. 5th(5) is omissible.
  // So omit_choice = {7} (pitch class 7 = G).
  // Full chord + one omission subset = full + minus-G variant.
  // Each has 12 transpositions.
  TempFile f("0 4 7 10\n");
  auto result = read_chord_database(f.path());
  // Full chord: 12 transpositions. Omitted (0,4,10): 12 transpositions.
  // Some might overlap with other entries, but for dom7 they shouldn't.
  // Full: bitmask at j=0 = (1<<0)+(1<<4)+(1<<7)+(1<<10) = 1+16+128+1024 = 1169
  // Omitted: bitmask at j=0 = (1<<0)+(1<<4)+(1<<10) = 1+16+1024 = 1041
  EXPECT_TRUE(std::binary_search(result.begin(), result.end(), 1169));
  EXPECT_TRUE(std::binary_search(result.begin(), result.end(), 1041));
  // Should have more than 12 entries (full + omission variants)
  EXPECT_GT(result.size(), 12u);
}

TEST(DatabaseTest, ResultIsSortedAndDeduplicated) {
  // Two identical lines should produce the same bitmasks, deduplicated
  TempFile f("0 7\n0 7\n");
  auto result = read_chord_database(f.path());
  ASSERT_EQ(result.size(), 12u);  // Deduplicated: still 12 unique bitmasks
  // Verify sorted
  for (size_t i = 1; i < result.size(); ++i) {
    EXPECT_LE(result[i - 1], result[i]);
  }
}

TEST(DatabaseTest, MultipleChordLines) {
  // Two different chords
  TempFile f("0\n0 7\n");
  auto result = read_chord_database(f.path());
  // Single note: 12 bitmasks. Fifth: 12 bitmasks. Some may overlap.
  // Single-note bitmask 128 = (1<<7), fifth bitmask at j=0 = 129 = (1<<0)+(1<<7)
  // No overlap expected between 1-bit and 2-bit values.
  EXPECT_EQ(result.size(), 24u);
}

TEST(DatabaseTest, BlankLinesSkipped) {
  TempFile f("\n\n0\n\n");
  auto result = read_chord_database(f.path());
  EXPECT_EQ(result.size(), 12u);
}

// ── read_alignment_database ─────────────────────────────────────

TEST(AlignmentDatabaseTest, NonexistentFileReturnsEmpty) {
  auto result = read_alignment_database("/nonexistent/path/to/file.txt");
  EXPECT_TRUE(result.empty());
}

TEST(AlignmentDatabaseTest, HeaderOnlyFileReturnsEmpty) {
  // 5 header lines, no data
  TempFile f("h1\nh2\nh3\nh4\nh5\n");
  auto result = read_alignment_database(f.path());
  EXPECT_TRUE(result.empty());
}

TEST(AlignmentDatabaseTest, SingleAlignmentExpandsCyclicRotations) {
  // 5 header lines + one alignment "1 3 5"
  TempFile f("h1\nh2\nh3\nh4\nh5\n1 3 5\n");
  auto result = read_alignment_database(f.path());
  // 3 elements → 3 cyclic rotations
  ASSERT_EQ(result.size(), 3u);
  EXPECT_EQ(result[0], (std::vector<int>{1, 3, 5}));
  EXPECT_EQ(result[1], (std::vector<int>{3, 5, 1}));
  EXPECT_EQ(result[2], (std::vector<int>{5, 1, 3}));
}

TEST(AlignmentDatabaseTest, TwoElementAlignment) {
  TempFile f("h1\nh2\nh3\nh4\nh5\n1 5\n");
  auto result = read_alignment_database(f.path());
  ASSERT_EQ(result.size(), 2u);
  EXPECT_EQ(result[0], (std::vector<int>{1, 5}));
  EXPECT_EQ(result[1], (std::vector<int>{5, 1}));
}

TEST(AlignmentDatabaseTest, MultipleAlignments) {
  // Two alignment lines
  TempFile f("h1\nh2\nh3\nh4\nh5\n1 3\n1 3 5\n");
  auto result = read_alignment_database(f.path());
  // First: 2 rotations. Second: 3 rotations. Total: 5.
  ASSERT_EQ(result.size(), 5u);
  EXPECT_EQ(result[0], (std::vector<int>{1, 3}));
  EXPECT_EQ(result[1], (std::vector<int>{3, 1}));
  EXPECT_EQ(result[2], (std::vector<int>{1, 3, 5}));
  EXPECT_EQ(result[3], (std::vector<int>{3, 5, 1}));
  EXPECT_EQ(result[4], (std::vector<int>{5, 1, 3}));
}

TEST(AlignmentDatabaseTest, BlankDataLinesSkipped) {
  TempFile f("h1\nh2\nh3\nh4\nh5\n\n1 3 5\n\n");
  auto result = read_alignment_database(f.path());
  ASSERT_EQ(result.size(), 3u);
}

TEST(AlignmentDatabaseTest, SingleElementAlignment) {
  TempFile f("h1\nh2\nh3\nh4\nh5\n7\n");
  auto result = read_alignment_database(f.path());
  // Single element → 1 rotation (itself)
  ASSERT_EQ(result.size(), 1u);
  EXPECT_EQ(result[0], (std::vector<int>{7}));
}
