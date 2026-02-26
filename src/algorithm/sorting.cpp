#include "algorithm/sorting.h"
#include <algorithm>
#include <cmath>
#include <functional>
#include <numeric>

namespace chordnovarw::algorithm {

using Stats = model::BigramChordStatistics;

namespace {

struct SortKey {
  std::function<double(const CandidateEntry&)> extractor;
  bool ascending;
};

// Extract a double value from stats given a sort key character
std::function<double(const CandidateEntry&)> get_extractor(char ch) {
  switch (ch) {
    case 'P': return [](const CandidateEntry& e) { return static_cast<double>(e.stats.sim_orig); };
    case 'N': return [](const CandidateEntry& e) { return static_cast<double>(e.stats.pitch_class_set.size()); };
    case 'T': return [](const CandidateEntry& e) { return e.chord.get_tension(); };
    case 'K': return [](const CandidateEntry& e) { return e.stats.chroma; };
    case 'C': return [](const CandidateEntry& e) { return static_cast<double>(e.stats.common_note); };
    case 'a': return [](const CandidateEntry& e) { return static_cast<double>(e.stats.span); };
    case 'A': return [](const CandidateEntry& e) { return static_cast<double>(e.stats.sspan); };
    case 'm': return [](const CandidateEntry& e) { return static_cast<double>(e.stats.notes.size()); };
    case 'h': return [](const CandidateEntry& e) { return e.chord.get_thickness(); };
    case 'g': return [](const CandidateEntry& e) { return e.chord.get_geometrical_center(); };
    case 'S': return [](const CandidateEntry& e) { return static_cast<double>(e.stats.sv); };
    case 'Q': return [](const CandidateEntry& e) { return e.stats.Q_indicator; };
    case 'X': return [](const CandidateEntry& e) { return static_cast<double>(e.stats.similarity); };
    case 'k': return [](const CandidateEntry& e) { return e.stats.chroma_old; };
    case 'R': return [](const CandidateEntry& e) { return static_cast<double>(e.stats.root_movement); };
    case 'V': return [](const CandidateEntry& e) { return static_cast<double>(e.stats.root_movement); };
    default: return nullptr;
  }
}

// Parse sort_order string (right-to-left) into a vector of SortKeys
// ordered primary-first (leftmost key = most significant = first in vector).
std::vector<SortKey> parse_sort_keys(const std::string& sort_order) {
  std::vector<SortKey> keys;
  int pos = static_cast<int>(sort_order.size()) - 1;
  while (pos >= 0) {
    bool ascending = false;
    char ch = sort_order[pos];
    if (ch == '+') {
      ascending = true;
      --pos;
      if (pos < 0) break;
      ch = sort_order[pos];
    }

    auto extractor = get_extractor(ch);
    if (extractor) {
      keys.push_back({std::move(extractor), ascending});
    }
    --pos;
  }
  // Reverse so leftmost key (primary) is first
  std::reverse(keys.begin(), keys.end());
  return keys;
}

} // anonymous namespace

void sort_candidates(std::vector<CandidateEntry>& candidates,
                     const std::string& sort_order) {
  if (sort_order.empty() || candidates.empty()) return;

  auto keys = parse_sort_keys(sort_order);
  if (keys.empty()) return;

  // Since CandidateEntry contains BigramChordStatistics with const fields
  // (non-move-assignable), we sort indices and then rearrange via a permutation.
  const size_t n = candidates.size();
  std::vector<size_t> indices(n);
  std::iota(indices.begin(), indices.end(), 0);

  // Single stable_sort with composite comparator
  std::stable_sort(indices.begin(), indices.end(),
      [&](size_t a, size_t b) {
        for (const auto& key : keys) {
          double va = key.extractor(candidates[a]);
          double vb = key.extractor(candidates[b]);
          if (va != vb) {
            return key.ascending ? (va < vb) : (va > vb);
          }
        }
        return false;  // equal on all keys
      });

  // Apply the permutation
  std::vector<CandidateEntry> sorted;
  sorted.reserve(n);
  for (size_t idx : indices) {
    sorted.push_back(std::move(candidates[idx]));
  }
  candidates = std::move(sorted);
}

} // namespace chordnovarw::algorithm
