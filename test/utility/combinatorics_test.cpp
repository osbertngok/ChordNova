#include "gtest/gtest.h"
#include "utility/combinatorics.h"

using namespace chordnovarw::utility;

// ── comb() tests ────────────────────────────────────────────────────

TEST(Combinatorics, comb_zero_zero) {
  static_assert(comb(0, 0) == 1);
  EXPECT_EQ(comb(0, 0), 1);
}

TEST(Combinatorics, comb_n_zero) {
  static_assert(comb(5, 0) == 1);
  EXPECT_EQ(comb(5, 0), 1);
}

TEST(Combinatorics, comb_n_n) {
  static_assert(comb(5, 5) == 1);
  EXPECT_EQ(comb(5, 5), 1);
}

TEST(Combinatorics, comb_n_one) {
  static_assert(comb(14, 1) == 14);
  EXPECT_EQ(comb(14, 1), 14);
}

TEST(Combinatorics, comb_symmetry) {
  // C(n,k) == C(n, n-k)
  EXPECT_EQ(comb(10, 3), comb(10, 7));
  EXPECT_EQ(comb(8, 2), comb(8, 6));
}

TEST(Combinatorics, comb_known_values) {
  EXPECT_EQ(comb(4, 2), 6);   // used in expansion: 3-note to 5-voice
  EXPECT_EQ(comb(5, 2), 10);
  EXPECT_EQ(comb(6, 3), 20);
  EXPECT_EQ(comb(10, 5), 252);
}

TEST(Combinatorics, comb_k_greater_than_n) {
  EXPECT_EQ(comb(3, 5), 0);
}

TEST(Combinatorics, comb_negative_k) {
  EXPECT_EQ(comb(5, -1), 0);
}

// ── ExpansionIndexCache tests ───────────────────────────────────────

TEST(ExpansionIndexCache, identity_expansion) {
  // min_size == max_size: only 1 expansion, identity mapping
  const auto& cache = ExpansionIndexCache::instance();
  EXPECT_EQ(cache.count(3, 3), 1);
  auto result = cache.get(3, 3, 0);
  EXPECT_EQ(result, (std::vector<int>{0, 1, 2}));
}

TEST(ExpansionIndexCache, three_to_five_count) {
  // 3-note chord expanded to 5 voices: C(4,2) = 6
  const auto& cache = ExpansionIndexCache::instance();
  EXPECT_EQ(cache.count(3, 5), 6);
}

TEST(ExpansionIndexCache, three_to_five_all_sorted) {
  const auto& cache = ExpansionIndexCache::instance();
  for (int i = 0; i < 6; ++i) {
    auto expansion = cache.get(3, 5, i);
    EXPECT_EQ(expansion.size(), 5);
    // Must be sorted ascending
    for (size_t j = 1; j < expansion.size(); ++j) {
      EXPECT_LE(expansion[j - 1], expansion[j]);
    }
    // All values in [0, 2]
    for (int val : expansion) {
      EXPECT_GE(val, 0);
      EXPECT_LE(val, 2);
    }
  }
}

TEST(ExpansionIndexCache, one_to_any) {
  // 1 note expanded to N voices: always [0, 0, ..., 0]
  const auto& cache = ExpansionIndexCache::instance();
  EXPECT_EQ(cache.count(1, 5), 1);
  auto result = cache.get(1, 5, 0);
  EXPECT_EQ(result, (std::vector<int>{0, 0, 0, 0, 0}));
}

TEST(ExpansionIndexCache, out_of_range_throws) {
  const auto& cache = ExpansionIndexCache::instance();
  EXPECT_THROW(cache.get(0, 5, 0), std::out_of_range);
  EXPECT_THROW(cache.get(3, 5, 6), std::out_of_range);
  EXPECT_THROW(cache.get(16, 16, 0), std::out_of_range);
}
