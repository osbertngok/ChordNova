#include "algorithm/sorting.h"
#include <algorithm>
#include <cmath>
#include <functional>
#include <numeric>

namespace chordnovarw::algorithm {

using Stats = model::BigramChordStatistics;

namespace {

// Extract a double value from stats given a sort key character
std::function<double(const CandidateEntry&)> get_key(char ch) {
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

} // anonymous namespace

void sort_candidates(std::vector<CandidateEntry>& candidates,
                     const std::string& sort_order) {
  if (sort_order.empty() || candidates.empty()) return;

  // Since CandidateEntry contains BigramChordStatistics with const fields
  // (non-move-assignable), we sort indices and then rearrange via a permutation.
  const size_t n = candidates.size();
  std::vector<size_t> indices(n);
  std::iota(indices.begin(), indices.end(), 0);

  // Read right-to-left
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

    auto key_fn = get_key(ch);
    if (key_fn) {
      if (ascending) {
        std::stable_sort(indices.begin(), indices.end(),
            [&](size_t a, size_t b) {
              return key_fn(candidates[a]) < key_fn(candidates[b]);
            });
      } else {
        std::stable_sort(indices.begin(), indices.end(),
            [&](size_t a, size_t b) {
              return key_fn(candidates[a]) > key_fn(candidates[b]);
            });
      }
    }
    --pos;
  }

  // Apply the permutation using a temporary vector of unique_ptrs
  // to avoid move-assignment issues with const fields
  std::vector<CandidateEntry> sorted;
  sorted.reserve(n);
  for (size_t idx : indices) {
    sorted.push_back(std::move(candidates[idx]));
  }
  candidates = std::move(sorted);
}

} // namespace chordnovarw::algorithm
