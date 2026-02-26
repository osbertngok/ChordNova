#include "model/bigramchordstatistics.h"
#include "model/chord.h"
#include "constant.h"
#include "utility.h"
#include <algorithm>
#include <cmath>
#include <set>
#include <string>

using namespace std;

namespace {

// Port of: 6 - (5 * (midi_note % 12) + 6) % 12
// Must use signed range (not COFUnit 0–11) because overflow wrapping needs values like ±18
int midi_to_cof(int midi_note) {
  return 6 - (5 * (midi_note % chordnovarw::ET_SIZE) + 6) % chordnovarw::ET_SIZE;
}

// Port of chromatonum (functions.cpp:371)
int chroma_to_midi_pc(int chroma) {
  const int table[7] = {5, 0, 7, 2, 9, 4, 11};
  const int result = table[(chroma + 36) % 7] + ((chroma + 36) / 7 - 5);
  return result;
}

// Port of chromatoname (functions.cpp:381)
// Same logic as PitchClass::to_string() but takes raw chroma int
string chroma_to_name(int chroma) {
  const char table[7] = {'F', 'C', 'G', 'D', 'A', 'E', 'B'};
  string result(1, table[(chroma + 36) % 7]);
  const int accidental = (chroma + 36) / 7 - 5;
  switch (accidental) {
    case -2: result += "bb"; break;
    case -1: result += "b"; break;
    case 1: result += "#"; break;
    case 2: result += "x"; break;
    default: break;
  }
  return result;
}

struct SpanResult {
  int span;
  int sspan;
  vector<int> adjusted_single_chroma;
};

// Faithful port of set_span() (chord.cpp:723–825)
SpanResult compute_span_and_adjust(
    vector<int> curr_single_chroma,
    const vector<int> &prev_single_chroma
) {
  const int n = curr_single_chroma.size();
  vector<int> copy(curr_single_chroma);
  sort(copy.begin(), copy.end());

  int min_diff1 = copy[n - 1] - copy[0];
  int min_bound = max(abs(copy[0]), abs(copy[n - 1]));
  int index = 0;

  const bool initial = prev_single_chroma.empty();

  if (initial) {
    for (int i = 1; i < n; ++i) {
      const int diff1 = copy[i - 1] + chordnovarw::ET_SIZE - copy[i];
      if (diff1 < min_diff1) {
        min_diff1 = diff1;
        min_bound = max(abs(copy[i - 1] + chordnovarw::ET_SIZE), abs(copy[i]));
        index = i;
      } else if (diff1 == min_diff1) {
        const int bound = max(abs(copy[i - 1] + chordnovarw::ET_SIZE), abs(copy[i]));
        if (bound < min_bound) {
          min_bound = bound;
          index = i;
        }
      }
    }

    // Apply adjustment
    copy.assign(curr_single_chroma.begin(), curr_single_chroma.end());
    sort(copy.begin(), copy.end());
    if (index > 0) {
      for (int i = 0; i < n; ++i) {
        if (curr_single_chroma[i] <= copy[index - 1])
          curr_single_chroma[i] += chordnovarw::ET_SIZE;
      }
    } else if (index < 0) {
      for (int i = 0; i < n; ++i) {
        if (curr_single_chroma[i] >= copy[-index - 1])
          curr_single_chroma[i] -= chordnovarw::ET_SIZE;
      }
    }

    return {min_diff1, 0, curr_single_chroma};
  }

  // Non-initial path
  int min_diff2;
  vector<int> merged_single_chroma = set_union(prev_single_chroma, copy);
  min_diff2 = *(merged_single_chroma.rbegin()) - *(merged_single_chroma.begin());

  // Forward rotations (+12)
  for (int i = 1; i <= n; ++i) {
    copy[i - 1] += chordnovarw::ET_SIZE;
    const int diff1 = copy[i - 1] - copy[i % n];
    if (diff1 < min_diff1) {
      min_diff1 = diff1;
      merged_single_chroma = set_union(prev_single_chroma, copy);
      min_diff2 = *(merged_single_chroma.rbegin()) - *(merged_single_chroma.begin());
      min_bound = max(abs(copy[i - 1]), abs(copy[i % n]));
      index = i;
    } else if (diff1 == min_diff1) {
      merged_single_chroma = set_union(prev_single_chroma, copy);
      const int diff2 = *(merged_single_chroma.rbegin()) - *(merged_single_chroma.begin());
      if (diff2 < min_diff2) {
        min_diff2 = diff2;
        min_bound = max(abs(copy[i - 1]), abs(copy[i % n]));
        index = i;
      } else if (diff2 == min_diff2) {
        const int bound = max(abs(copy[i - 1]), abs(copy[i % n]));
        if (bound < min_bound) {
          min_bound = bound;
          index = i;
        }
      }
    }
  }

  // Backward rotations (-12)
  copy.assign(curr_single_chroma.begin(), curr_single_chroma.end());
  sort(copy.begin(), copy.end());
  for (int i = n; i > 0; --i) {
    const int j = (i - 2 + n) % n;
    copy[i - 1] -= chordnovarw::ET_SIZE;
    const int diff1 = copy[j] - copy[i - 1];
    if (diff1 < min_diff1) {
      min_diff1 = diff1;
      merged_single_chroma = set_union(prev_single_chroma, copy);
      min_diff2 = *(merged_single_chroma.rbegin()) - *(merged_single_chroma.begin());
      min_bound = max(abs(copy[j]), abs(copy[i - 1]));
      index = -i;
    } else if (diff1 == min_diff1) {
      merged_single_chroma = set_union(prev_single_chroma, copy);
      const int diff2 = *(merged_single_chroma.rbegin()) - *(merged_single_chroma.begin());
      if (diff2 < min_diff2) {
        min_diff2 = diff2;
        min_bound = max(abs(copy[j]), abs(copy[i - 1]));
        index = -i;
      } else if (diff2 == min_diff2) {
        const int bound = max(abs(copy[j]), abs(copy[i - 1]));
        if (bound < min_bound) {
          min_bound = bound;
          index = -i;
        }
      }
    }
  }

  // Apply adjustment based on index
  copy.assign(curr_single_chroma.begin(), curr_single_chroma.end());
  sort(copy.begin(), copy.end());
  if (index > 0) {
    for (int i = 0; i < n; ++i) {
      if (curr_single_chroma[i] <= copy[index - 1])
        curr_single_chroma[i] += chordnovarw::ET_SIZE;
    }
  } else if (index < 0) {
    for (int i = 0; i < n; ++i) {
      if (curr_single_chroma[i] >= copy[-index - 1])
        curr_single_chroma[i] -= chordnovarw::ET_SIZE;
    }
  }

  return {min_diff1, min_diff2, curr_single_chroma};
}

struct ChromaOldResult {
  double chroma_old;
  chordnovarw::model::OverflowState overflow_state;
  vector<int> adjusted_single_chroma;
};

// Faithful port of set_chroma_old() (chord.cpp:834–860)
ChromaOldResult compute_chroma_old(
    vector<int> single_chroma,
    double prev_chroma_old
) {
  vector<int> copy(single_chroma);
  sort(copy.begin(), copy.end());
  copy.erase(unique(copy.begin(), copy.end()), copy.end());

  double chroma_old = 0.0;
  const int s_size = copy.size();
  for (int i = 0; i < s_size; ++i)
    chroma_old += copy[i];
  chroma_old /= static_cast<double>(s_size);
  chroma_old = floor(chroma_old * 100) / 100.0;

  int val = 0;
  if (chroma_old - prev_chroma_old < -18.0)
    val = chordnovarw::ET_SIZE * 2;
  else if (chroma_old - prev_chroma_old < -6.0)
    val = chordnovarw::ET_SIZE * 1;
  else if (chroma_old - prev_chroma_old > 18.0)
    val = chordnovarw::ET_SIZE * -2;
  else if (chroma_old - prev_chroma_old > 6.0)
    val = chordnovarw::ET_SIZE * -1;

  auto overflow_state = chordnovarw::model::NoOverflow;
  if (val != 0) {
    for (auto &sc : single_chroma)
      sc += val;
    chroma_old += val;
    overflow_state = chordnovarw::model::Total;
  }

  return {chroma_old, overflow_state, single_chroma};
}

// Faithful port of set_chroma() (chord.cpp:873–888)
double compute_chroma(
    const vector<int> &prev_single_chroma,
    const vector<int> &curr_single_chroma,
    double prev_chroma_old,
    double curr_chroma_old
) {
  vector<int> A(prev_single_chroma);
  vector<int> B(curr_single_chroma);
  sort(A.begin(), A.end());
  sort(B.begin(), B.end());
  A.erase(unique(A.begin(), A.end()), A.end());
  B.erase(unique(B.begin(), B.end()), B.end());

  const vector<int> A_unique = set_complement(A, B);
  const vector<int> B_unique = set_complement(B, A);

  int val = 0;
  for (int i = 0; i < static_cast<int>(A_unique.size()); ++i)
    for (int j = 0; j < static_cast<int>(B_unique.size()); ++j)
      val += abs(A_unique[i] - B_unique[j]);

  const int s = sign(curr_chroma_old - prev_chroma_old);
  return s * 2.0 / 3.1416 * atan(val / 54.0) * 100.0;
}

struct NameResult {
  string name;
  string name_with_octave;
  string root_name;
  int overflow_amount;
  chordnovarw::model::OverflowState final_state;
  double final_chroma_old;
  double final_prev_chroma_old;
  vector<int> final_single_chroma;
};

// Faithful port of set_name() (chord.cpp:895–944)
NameResult compute_name(
    vector<int> single_chroma,
    const vector<int> &midi_notes,
    const chordnovarw::model::PitchClass &root_pc,
    chordnovarw::model::OverflowState overflow_state,
    double chroma_old,
    double prev_chroma_old
) {
  const int n = single_chroma.size();
  vector<int> copy(single_chroma);
  sort(copy.begin(), copy.end());

  int overflow_amount = 0;
  if (copy[n - 1] < -6)
    overflow_amount = chordnovarw::ET_SIZE * -1;
  else if (copy[0] > 6)
    overflow_amount = chordnovarw::ET_SIZE * 1;
  else if (copy[n - 1] >= 13 && copy[0] >= 4)
    overflow_amount = chordnovarw::ET_SIZE * 1;
  else if (copy[0] <= -9 && copy[n - 1] <= 0)
    overflow_amount = chordnovarw::ET_SIZE * -1;

  for (int i = 0; i < n; ++i)
    single_chroma[i] -= overflow_amount;

  if (overflow_state == chordnovarw::model::NoOverflow && overflow_amount != 0)
    overflow_state = chordnovarw::model::Single;

  chroma_old -= overflow_amount;
  prev_chroma_old -= overflow_amount;

  string name_str;
  string name_with_octave_str;

  for (int i = 0; i < n; ++i) {
    const string note_name = chroma_to_name(single_chroma[i]);
    name_str += note_name;

    name_with_octave_str += note_name;
    const int octave = (midi_notes[i] - chroma_to_midi_pc(single_chroma[i])) / chordnovarw::ET_SIZE - 1;
    name_with_octave_str += to_string(octave);

    if (i < n - 1) {
      name_str += " ";
      name_with_octave_str += " ";
    }
  }

  // Find root position and get root name
  int position = 0;
  for (int i = 0; i < n; ++i) {
    if ((midi_notes[i] - root_pc.value()) % chordnovarw::ET_SIZE == 0) {
      position = i;
      break;
    }
  }
  const string root_name = chroma_to_name(single_chroma[position]);

  return {name_str, name_with_octave_str, root_name,
          overflow_amount, overflow_state, chroma_old, prev_chroma_old,
          single_chroma};
}

} // anonymous namespace

namespace chordnovarw {
  namespace model {

    BigramChordStatistics::BigramChordStatistics(
        double chroma_old, double prev_chroma_old, double chroma,
        double Q_indicator, int common_note, int sv, int span, int sspan,
        int similarity, int sim_orig, int steady_count, int ascending_count,
        int descending_count, int root_movement,
        string root_name, bool hide_octave,
        string name, string name_with_octave,
        OverflowState overflow_state, int overflow_amount,
        vector<int> notes, vector<int> pitch_class_set,
        vector<int> single_chroma, vector<int> vec,
        vector<int> self_diff, vector<int> count_vec,
        vector<int> alignment
    ) : chroma_old(chroma_old), prev_chroma_old(prev_chroma_old), chroma(chroma),
        Q_indicator(Q_indicator), common_note(common_note), sv(sv), span(span), sspan(sspan),
        similarity(similarity), sim_orig(sim_orig),
        steady_count(steady_count), ascending_count(ascending_count),
        descending_count(descending_count), root_movement(root_movement),
        root_name(std::move(root_name)), hide_octave(hide_octave),
        name(std::move(name)), name_with_octave(std::move(name_with_octave)),
        overflow_state(overflow_state), overflow_amount(overflow_amount),
        notes(std::move(notes)), pitch_class_set(std::move(pitch_class_set)),
        single_chroma(std::move(single_chroma)), vec(std::move(vec)),
        self_diff(std::move(self_diff)), count_vec(std::move(count_vec)),
        alignment(std::move(alignment)) {
    }

    BigramChordStatistics calculate_bigram_statistics(
        const OrderedChord &prev_chord,
        const OrderedChord &curr_chord,
        const OrderedChordStatistics &prev_stats,
        const OrderedChordStatistics &curr_stats,
        const vector<int> &vec,
        int sv,
        int vl_max,
        double prev_chroma_old,
        const vector<int> &prev_single_chroma
    ) {
      // 1. Count ascending/steady/descending from vec
      int ascending_count = 0;
      int steady_count = 0;
      int descending_count = 0;
      for (const int v : vec) {
        if (v > 0) ++ascending_count;
        else if (v == 0) ++steady_count;
        else ++descending_count;
      }

      // 2. Compute root_movement from both roots
      int root_movement = 0;
      if (prev_stats.root.has_value() && curr_stats.root.has_value()) {
        root_movement = (curr_stats.root->value() - prev_stats.root->value() + ET_SIZE) % ET_SIZE;
        if (root_movement > 6)
          root_movement = ET_SIZE - root_movement;
      }

      // 3. Count common_note via set_intersect on sorted MIDI arrays
      const auto prev_pitches = prev_chord.get_pitches();
      const auto curr_pitches = curr_chord.get_pitches();
      vector<int> prev_midi, curr_midi;
      for (const auto &p : prev_pitches) prev_midi.push_back(p.get_number());
      for (const auto &p : curr_pitches) curr_midi.push_back(p.get_number());
      sort(prev_midi.begin(), prev_midi.end());
      sort(curr_midi.begin(), curr_midi.end());
      const int common_note = static_cast<int>(set_intersect(prev_midi, curr_midi).size());

      // 4. Compute similarity from sv, vl_max, note counts, root match
      const double max_sv = vl_max * 1.0 * max(prev_stats.num_of_pitches, curr_stats.num_of_pitches);
      double sim_temp = pow(1.0 - sv / max_sv, 1);
      if (prev_stats.root.has_value() && curr_stats.root.has_value()
          && prev_stats.root.value() == curr_stats.root.value()) {
        sim_temp = sqrt(sim_temp);
      }
      const int similarity = static_cast<int>(round(100.0 * sim_temp));
      const int sim_orig = 100;

      // 5. Build single_chroma from curr chord MIDI notes via midi_to_cof
      vector<int> curr_single_chroma;
      for (const int m : curr_midi) {
        curr_single_chroma.push_back(midi_to_cof(m));
      }

      // 6. compute_span_and_adjust -> span, sspan, adjusted single_chroma
      auto span_result = compute_span_and_adjust(curr_single_chroma, prev_single_chroma);

      // 7. compute_chroma_old -> chroma_old, overflow state
      auto chroma_old_result = compute_chroma_old(
          span_result.adjusted_single_chroma, prev_chroma_old);

      // 8. compute_chroma -> harmonic distance
      double chroma = 0.0;
      if (!prev_single_chroma.empty()) {
        chroma = compute_chroma(
            prev_single_chroma,
            chroma_old_result.adjusted_single_chroma,
            prev_chroma_old,
            chroma_old_result.chroma_old);
      }

      // 9. compute_name -> note names, overflow correction
      NameResult name_result = {
          "", "", "", 0,
          chroma_old_result.overflow_state,
          chroma_old_result.chroma_old,
          prev_chroma_old,
          chroma_old_result.adjusted_single_chroma
      };
      if (curr_stats.root.has_value()) {
        name_result = compute_name(
            chroma_old_result.adjusted_single_chroma,
            curr_midi,
            curr_stats.root.value(),
            chroma_old_result.overflow_state,
            chroma_old_result.chroma_old,
            prev_chroma_old);
      }

      // 10. Q_indicator = chroma * (t1 + t2) / 2 / max(n1, n2)
      const double Q_indicator = chroma * (prev_stats.tension + curr_stats.tension)
          / 2.0 / static_cast<double>(max(prev_stats.num_of_pitches, curr_stats.num_of_pitches));

      // 11. Copy Tier 1 fields from curr_stats
      // Build pitch_class_set from curr chord
      set<int> pc_set_sorted;
      for (const auto &p : curr_pitches) {
        pc_set_sorted.insert(p.get_pitch_class().value());
      }
      const vector<int> pitch_class_set(pc_set_sorted.begin(), pc_set_sorted.end());

      // 12. Assemble and return
      return BigramChordStatistics(
          name_result.final_chroma_old,
          name_result.final_prev_chroma_old,
          chroma,
          Q_indicator,
          common_note,
          sv,
          span_result.span,
          span_result.sspan,
          similarity,
          sim_orig,
          steady_count,
          ascending_count,
          descending_count,
          root_movement,
          name_result.root_name,
          false, // hide_octave
          name_result.name,
          name_result.name_with_octave,
          name_result.final_state,
          name_result.overflow_amount,
          curr_midi,
          pitch_class_set,
          name_result.final_single_chroma,
          vec,
          curr_stats.self_diff,
          curr_stats.count_vec,
          curr_stats.alignment
      );
    }
  }
}
