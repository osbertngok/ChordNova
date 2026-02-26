#include "utility/combinatorics.h"
#include <stdexcept>

namespace chordnovarw::utility {

namespace {

/**
 * Inserts elements into an array at specified positions (mirrors legacy `insert()`).
 * Used to build expansion index tables.
 */
void insert_positions(std::vector<int>& ar, const std::vector<int>& pos) {
  for (int i = static_cast<int>(pos.size()) - 1; i >= 0; --i) {
    int len1 = static_cast<int>(ar.size());
    ar.push_back(0); // extend by one
    for (int j = len1 - 1; j > pos[i]; --j)
      ar[j + 1] = ar[j];
    ar[pos[i] + 1] = pos[i];
  }
}

/**
 * Computes all expansion indexes for a given (min_size, max_size) pair.
 * Returns a vector of vectors, each of length max_size.
 *
 * Algorithm mirrors legacy `set_expansion_indexes()` from functions.cpp.
 */
std::vector<std::vector<int>> compute_expansions(int min_size, int max_size) {
  const int diff = max_size - min_size;
  const int total = comb(max_size - 1, min_size - 1);

  // Initialize base arrays: [0, 1, 2, ..., min_size-1]
  std::vector<std::vector<int>> result(total);
  for (int i = 0; i < total; ++i) {
    result[i].resize(min_size);
    for (int j = 0; j < min_size; ++j)
      result[i][j] = j;
  }

  if (diff == 0) return result;

  std::vector<int> pos(15, 0);
  int index1 = 0, index2 = 0, index3 = 0;

  while (index1 >= 0) {
    if (index2 >= min_size) {
      index2 = pos[--index1] + 1;
    } else if (index1 == diff) {
      std::vector<int> positions(pos.begin(), pos.begin() + diff);
      insert_positions(result[index3], positions);
      --index1;
      ++index2;
      ++index3;
    } else {
      pos[index1] = index2;
      ++index1;
    }
  }

  return result;
}

} // anonymous namespace

std::vector<int> ExpansionIndexCache::get(int min_size, int max_size, int index) const {
  // Thread-safe lazy computation via local statics
  // For simplicity and correctness, compute on demand rather than caching the full 15x15 table
  if (min_size < 1 || min_size > 15 || max_size < min_size || max_size > 15) {
    throw std::out_of_range("ExpansionIndexCache::get: invalid sizes");
  }
  const int total = comb(max_size - 1, min_size - 1);
  if (index < 0 || index >= total) {
    throw std::out_of_range("ExpansionIndexCache::get: index out of range");
  }

  auto expansions = compute_expansions(min_size, max_size);
  return expansions[index];
}

const ExpansionIndexCache& ExpansionIndexCache::instance() {
  static ExpansionIndexCache cache;
  return cache;
}

} // namespace chordnovarw::utility
