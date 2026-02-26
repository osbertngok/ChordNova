#include "gtest/gtest.h"
#include "utility/mixedradix.h"
#include <set>

using namespace chordnovarw::utility;

TEST(MixedRadix, total_count_no_deadzone) {
  // vl_max=4, width=3, vl_min=0: each voice has 9 choices -> 9^3 = 729
  MixedRadixRange range(4, 3, 0);
  EXPECT_EQ(range.total_count(), 729);
}

TEST(MixedRadix, total_count_with_deadzone) {
  // vl_max=4, width=3, vl_min=2: each voice has 2*(4-2+1) = 6 choices -> 6^3 = 216
  MixedRadixRange range(4, 3, 2);
  EXPECT_EQ(range.total_count(), 216);
}

TEST(MixedRadix, iteration_count_matches) {
  MixedRadixRange range(4, 3, 0);
  long long count = 0;
  for (const auto& vec : range) {
    (void)vec;
    ++count;
  }
  EXPECT_EQ(count, 729);
}

TEST(MixedRadix, iteration_count_with_deadzone) {
  MixedRadixRange range(4, 3, 2);
  long long count = 0;
  for (const auto& vec : range) {
    (void)vec;
    ++count;
  }
  EXPECT_EQ(count, 216);
}

TEST(MixedRadix, all_values_in_range) {
  MixedRadixRange range(4, 3, 0);
  for (const auto& vec : range) {
    EXPECT_EQ(vec.size(), 3);
    for (int v : vec) {
      EXPECT_GE(v, -4);
      EXPECT_LE(v, 4);
    }
  }
}

TEST(MixedRadix, deadzone_skipping) {
  // vl_min=2: values -1, 0, 1 should never appear
  MixedRadixRange range(4, 2, 2);
  for (const auto& vec : range) {
    for (int v : vec) {
      EXPECT_TRUE(v <= -2 || v >= 2)
          << "Dead zone violation: got " << v;
    }
  }
}

TEST(MixedRadix, first_and_last_vectors) {
  MixedRadixRange range(1, 2, 0);
  std::vector<std::vector<int>> all;
  for (const auto& vec : range) {
    all.push_back(vec);
  }
  // vl_max=1, width=2, vl_min=0: 3^2 = 9 vectors
  EXPECT_EQ(all.size(), 9);
  // First: [-1, -1], Last: [1, 1]
  EXPECT_EQ(all.front(), (std::vector<int>{-1, -1}));
  EXPECT_EQ(all.back(), (std::vector<int>{1, 1}));
}

TEST(MixedRadix, all_unique) {
  MixedRadixRange range(2, 3, 0);
  std::set<std::vector<int>> seen;
  for (const auto& vec : range) {
    auto [_, inserted] = seen.insert(vec);
    EXPECT_TRUE(inserted) << "Duplicate vector found";
  }
  EXPECT_EQ(seen.size(), static_cast<size_t>(range.total_count()));
}

TEST(MixedRadix, width_one) {
  MixedRadixRange range(3, 1, 0);
  EXPECT_EQ(range.total_count(), 7);
  int count = 0;
  for (const auto& vec : range) {
    EXPECT_EQ(vec.size(), 1);
    ++count;
  }
  EXPECT_EQ(count, 7);
}

TEST(MixedRadix, deadzone_equals_vlmax) {
  // vl_min == vl_max: only -vl_max and +vl_max are valid (2 choices per voice)
  MixedRadixRange range(3, 2, 3);
  EXPECT_EQ(range.total_count(), 4); // 2^2
  for (const auto& vec : range) {
    for (int v : vec) {
      EXPECT_TRUE(v == -3 || v == 3);
    }
  }
}
