#include "io/database.h"
#include "constant.h"
#include <algorithm>
#include <fstream>
#include <set>
#include <sstream>

namespace chordnovarw::io {

namespace {

// Legacy note_pos: maps interval (in semitones from root) to scale degree position
const int note_pos[12] = {1, 9, 9, 3, 3, 11, 11, 5, 13, 13, 7, 7};

// Legacy omission table: which scale degrees are non-omissible for chord size 3-7
// Empty for sizes 0,1,2. For sizes 3-7, contains scale degrees that CANNOT be omitted.
const std::vector<int> omission[8] = {
    {},          // 0
    {},          // 1
    {},          // 2
    {1, 3, 5},   // 3-note chords: root, 3rd, 5th are essential
    {1, 3, 7},   // 4-note: root, 3rd, 7th essential
    {1, 3, 7},   // 5-note
    {1, 3, 7},   // 6-note
    {1, 3, 7}    // 7-note
};

// Port of find_root for pitch class sets
int find_root(const std::vector<int>& note_set) {
  // Simple interval-based root finding (port of legacy find_root)
  if (note_set.empty()) return 0;
  int best_root = note_set[0];
  int best_score = 0;
  int s = static_cast<int>(note_set.size());

  for (int candidate : note_set) {
    int score = 0;
    for (int n : note_set) {
      int interval = (n - candidate + chordnovarw::ET_SIZE) % chordnovarw::ET_SIZE;
      // Perfect fifth (7) and major/minor third (3,4) are strong indicators
      if (interval == 7) score += 3;
      else if (interval == 4) score += 2;
      else if (interval == 3) score += 2;
      else if (interval == 0) score += 1;
    }
    if (score > best_score) {
      best_score = score;
      best_root = candidate;
    }
  }
  return best_root;
}

// Generate all 12 transpositions of a pitch class set and encode as set_id bitmasks
void note_set_to_id(const std::vector<int>& note_set, std::vector<int>& rec) {
  for (int j = 0; j < chordnovarw::ET_SIZE; ++j) {
    int val = 0;
    for (int n : note_set) {
      val += (1 << ((n + j) % chordnovarw::ET_SIZE));
    }
    rec.push_back(val);
  }
}

} // anonymous namespace

std::vector<int> read_chord_database(const std::string& filename) {
  std::vector<int> chord_library;
  std::ifstream fin(filename);
  if (!fin.is_open()) return chord_library;

  std::string line;
  while (std::getline(fin, line)) {
    if (line.empty()) continue;
    if (line[0] == '/' || line[0] == 't') continue;

    std::istringstream iss(line);
    std::vector<int> note_set;
    int note;
    while (iss >> note) {
      note_set.push_back(note);
    }
    if (note_set.empty()) continue;

    std::sort(note_set.begin(), note_set.end());
    int s_size = static_cast<int>(note_set.size());
    int root = find_root(note_set);

    // Identify omissible notes
    std::vector<int> omit_choice;
    if (s_size >= 3 && s_size <= 7) {
      for (int n : note_set) {
        int diff = (n - root + chordnovarw::ET_SIZE) % chordnovarw::ET_SIZE;
        int pos = note_pos[diff];
        // Check if this scale degree is in the non-omissible list
        bool essential = false;
        for (int o : omission[s_size]) {
          if (o == pos) { essential = true; break; }
        }
        if (!essential) {
          omit_choice.push_back(n);
        }
      }
    }

    // Always add the full chord
    note_set_to_id(note_set, chord_library);

    // Iterate through all non-empty subsets of omissible notes
    int max_id = (1 << omit_choice.size()) - 1;
    for (int id = 1; id <= max_id; ++id) {
      // Build the omitted version: note_set minus subset of omit_choice
      std::vector<int> omitted;
      for (int n : note_set) {
        bool in_subset = false;
        for (int bit = 0; bit < static_cast<int>(omit_choice.size()); ++bit) {
          if ((id & (1 << bit)) && omit_choice[bit] == n) {
            in_subset = true;
            break;
          }
        }
        if (!in_subset) {
          omitted.push_back(n);
        }
      }
      if (!omitted.empty() && omitted.size() < static_cast<size_t>(s_size)) {
        note_set_to_id(omitted, chord_library);
      }
    }
  }

  // Sort and deduplicate
  std::sort(chord_library.begin(), chord_library.end());
  chord_library.erase(
      std::unique(chord_library.begin(), chord_library.end()),
      chord_library.end());

  return chord_library;
}

std::vector<std::vector<int>> read_alignment_database(const std::string& filename) {
  std::vector<std::vector<int>> alignment_list;
  std::ifstream fin(filename);
  if (!fin.is_open()) return alignment_list;

  // Skip 5 header lines
  std::string line;
  for (int i = 0; i < 5 && std::getline(fin, line); ++i) {}

  while (std::getline(fin, line)) {
    if (line.empty()) continue;
    std::istringstream iss(line);
    std::vector<int> single_align;
    int num;
    while (iss >> num) single_align.push_back(num);
    if (single_align.empty()) continue;

    // Add all cyclic rotations
    int len = static_cast<int>(single_align.size());
    for (int i = 0; i < len; ++i) {
      alignment_list.push_back(single_align);
      // Rotate: move first element to end
      single_align.push_back(single_align[0]);
      single_align.erase(single_align.begin());
    }
  }

  return alignment_list;
}

} // namespace chordnovarw::io
