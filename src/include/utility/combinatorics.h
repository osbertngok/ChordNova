#ifndef CHORDNOVARW_SRC_INCLUDE_UTILITY_COMBINATORICS_H_
#define CHORDNOVARW_SRC_INCLUDE_UTILITY_COMBINATORICS_H_

#include <map>
#include <utility>
#include <vector>

namespace chordnovarw::utility {

/**
 * \brief Computes the binomial coefficient C(n, k).
 *
 * Returns the number of ways to choose \e k items from \e n items.
 * Uses iterative multiplication to avoid overflow for small values.
 *
 * Legacy: `comb()` in `functions.cpp`.
 *
 * \param n Total number of items.
 * \param k Number of items to choose.
 * \return C(n, k), or 0 if k > n or k < 0.
 */
[[nodiscard]] constexpr int comb(int n, int k) noexcept {
  if (k < 0 || k > n) return 0;
  if (k == 0 || k == n) return 1;
  // Use symmetry: C(n,k) == C(n, n-k)
  if (k > n - k) k = n - k;
  int result = 1;
  for (int i = 1; i <= k; ++i) {
    result = result * (n + 1 - i) / i;
  }
  return result;
}

/**
 * \brief Lazily-computed cache for chord expansion index tables.
 *
 * Replaces the legacy `expansion_indexes[16][16][3432][15]` static array
 * (~15 MB) with an on-demand computed cache. Each entry
 * `get(min_size, max_size, index)` returns a vector of length `max_size`
 * describing how to expand a chord of `min_size` distinct notes into
 * `max_size` voices.
 *
 * Legacy: `set_expansion_indexes()` / `expansion_indexes` in `functions.cpp`.
 */
class ExpansionIndexCache {
public:
  /**
   * \brief Returns the expansion index vector for a given combination.
   *
   * \param min_size  Number of distinct notes in the source chord (1..15).
   * \param max_size  Target voice count (min_size..15).
   * \param index     Which combination to use (0..C(max_size-1, min_size-1)-1).
   * \return Vector of length \p max_size with values in [0, min_size-1],
   *         sorted ascending, representing which source note each voice maps to.
   */
  [[nodiscard]] std::vector<int> get(int min_size, int max_size, int index) const;

  /**
   * \brief Returns the total number of expansions for given sizes.
   *
   * \param min_size Number of distinct notes.
   * \param max_size Target voice count.
   * \return C(max_size - 1, min_size - 1).
   */
  [[nodiscard]] constexpr int count(int min_size, int max_size) const noexcept {
    return comb(max_size - 1, min_size - 1);
  }

  /**
   * \brief Global singleton accessor.
   */
  [[nodiscard]] static const ExpansionIndexCache& instance();

private:
  mutable std::map<std::pair<int,int>, std::vector<std::vector<int>>> cache_;
};

} // namespace chordnovarw::utility

#endif // CHORDNOVARW_SRC_INCLUDE_UTILITY_COMBINATORICS_H_
