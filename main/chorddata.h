// ChordNova v3.0 [Build: 2021.1.14]
// (c) 2020 Wenge Chen, Ji-woon Sim.
// chorddata.h

#ifndef CHORDDATA
#define CHORDDATA

#include <vector>
using std::vector;

namespace chordnova {
namespace chord {
enum Language { English, Chinese };
enum OverflowState { NoOverflow, Single, Total };

/**
 * \brief A structure that contains the notes of a chord and the calculated properties of a chord.
 */
class ChordData {
 public:
  int orig_pos;                    // Used in substitution.
  vector<int> pedal_notes_set;
  vector<int> pedal_notes;

  ChordData() = default;;
  ChordData(const ChordData &data) = default;

  size_t m_notes_size;      // n; size of m_notes
  size_t s_size;      // m; size of pitch_class_set
  double tension;     // t
  double thickness;   // h
  int root;           // r
  int g_center;       // g
  double chroma_old;  // kk
  double prev_chroma_old;
  double chroma;      // k
  double Q_indicator; // Q
  int common_note;    // c
  int sv;             // sv, Σvec
  int span;           // s
  int sspan;          // ss
  int similarity;     // x
  int sim_orig;       // p, default = 100%
  int steady_count;
  int ascending_count;
  int descending_count;
  int root_movement;
  char root_name[3];
  bool hide_octave;
  char name[50];             // name of each note in the chord
  char name_with_octave[50]; // name and octave of each note in the chord
  OverflowState overflow_state;
  int overflow_amount;

  vector<int> m_notes;         // always regarded as a sorted (L -> H) vector
  vector<int> pitch_class_set;      // set, not necessarily beginning with 0,
  // and is always regarded as a sorted (L -> H) vector

  /**
   * \brief The chroma value corresponding to each note, in the order of Circle of Fifths.
   *
   * It does not distinguish spelling difference of two enharmonically equivalent pitch classes, like C# and Db.
   *
   * For example, converting Csus2 (C4 D4 G4) to the positions of Circle of Fifths would use 0, 2, 1.
   * This single_chroma variable would always store the values in monotonically increasing order,
   * so the final result is [0, 1, 2].
   *
   * Calculating this would help to calculate the minimal span on the Circle of Fifths.
   */
  vector<int> single_chroma;
  vector<int> vec;           // v
  vector<int> self_diff;     // d
  vector<int> count_vec;     // vec
  vector<int> alignment;     // e.g. [3, 1, 7] means the chord is aligned as '3rd note, root, 7th note'.

  void inverse_param(); // This is used when we reverse the direction of a progression in chord substitution.
  void printInitial(Language);  // prints data of the initial chord
  void print(const ChordData &, Language); // prints data of a single chord
  void print_analysis(const ChordData &, const ChordData &, const char *, const char *, Language);
  // prints data of both chords in chord analysis
  void print_substitution(const char *, bool, bool, const ChordData &, const ChordData &, Language);
  // prints data of a single progression in chord substitution

  size_t &get_t_size() { return m_notes_size; }
  size_t &get_s_size() { return s_size; }
  double &get_tension() { return tension; }
  double &get_thickness() { return thickness; }
  int &get_root() { return root; }
  int &get_g_center() { return g_center; }
  double &get_chroma_old() { return chroma_old; }
  double &get_chroma() { return chroma; }
  double &get_Q_indicator() { return Q_indicator; }
  int &get_common_note() { return common_note; }
  int &get_sv() { return sv; }
  int &get_span() { return span; }
  int &get_sspan() { return sspan; }
  int &get_similarity() { return similarity; }
  int &get_sim_orig() { return sim_orig; }
  int &get_steady_count() { return steady_count; }
  int &get_ascending_count() { return ascending_count; }
  int &get_descending_count() { return descending_count; }
  int &get_root_movement() { return root_movement; }
  int &get_overflow_amount() { return overflow_amount; }

  vector<int> &get_notes() { return m_notes; }
  vector<int> &get_note_set() { return pitch_class_set; }
  vector<int> &get_single_chroma() { return single_chroma; }
  vector<int> &get_vec() { return vec; }
  vector<int> &get_self_diff() { return self_diff; }
  vector<int> &get_count_vec() { return count_vec; }
  vector<int> &get_alignment() { return alignment; }
  vector<int> &get_pedal_notes() { return pedal_notes; }
  vector<int> &get_pedal_notes_set() { return pedal_notes_set; }

  friend bool larger_chroma(const ChordData &, const ChordData &);
  friend bool larger_chroma_old(const ChordData &, const ChordData &);
  friend bool larger_tension(const ChordData &, const ChordData &);
  friend bool larger_common(const ChordData &, const ChordData &);
  friend bool larger_sv(const ChordData &, const ChordData &);
  friend bool larger_t_size(const ChordData &, const ChordData &);
  friend bool larger_s_size(const ChordData &, const ChordData &);
  friend bool larger_root(const ChordData &, const ChordData &);
  friend bool larger_span(const ChordData &, const ChordData &);
  friend bool larger_sspan(const ChordData &, const ChordData &);
  friend bool larger_thickness(const ChordData &, const ChordData &);
  friend bool larger_g_center(const ChordData &, const ChordData &);
  friend bool larger_similarity(const ChordData &, const ChordData &);
  friend bool larger_sim_orig(const ChordData &, const ChordData &);
  friend bool larger_Q_indicator(const ChordData &, const ChordData &);
  friend bool superior_rm(const ChordData &, const ChordData &);

  friend bool smaller_chroma(const ChordData &, const ChordData &);
  friend bool smaller_chroma_old(const ChordData &, const ChordData &);
  friend bool smaller_tension(const ChordData &, const ChordData &);
  friend bool smaller_common(const ChordData &, const ChordData &);
  friend bool smaller_sv(const ChordData &, const ChordData &);
  friend bool smaller_t_size(const ChordData &, const ChordData &);
  friend bool smaller_s_size(const ChordData &, const ChordData &);
  friend bool smaller_root(const ChordData &, const ChordData &);
  friend bool smaller_span(const ChordData &, const ChordData &);
  friend bool smaller_sspan(const ChordData &, const ChordData &);
  friend bool smaller_thickness(const ChordData &, const ChordData &);
  friend bool smaller_g_center(const ChordData &, const ChordData &);
  friend bool smaller_similarity(const ChordData &, const ChordData &);
  friend bool smaller_sim_orig(const ChordData &, const ChordData &);
  friend bool smaller_Q_indicator(const ChordData &, const ChordData &);
  friend bool inferior_rm(const ChordData &, const ChordData &);
};

extern bool larger_chroma(const ChordData &, const ChordData &);
extern bool larger_chroma_old(const ChordData &, const ChordData &);
extern bool larger_tension(const ChordData &, const ChordData &);
extern bool larger_common(const ChordData &, const ChordData &);
extern bool larger_sv(const ChordData &, const ChordData &);
extern bool larger_t_size(const ChordData &, const ChordData &);
extern bool larger_s_size(const ChordData &, const ChordData &);
extern bool larger_root(const ChordData &, const ChordData &);
extern bool larger_span(const ChordData &, const ChordData &);
extern bool larger_sspan(const ChordData &, const ChordData &);
extern bool larger_thickness(const ChordData &, const ChordData &);
extern bool larger_g_center(const ChordData &, const ChordData &);
extern bool larger_similarity(const ChordData &, const ChordData &);
extern bool larger_sim_orig(const ChordData &, const ChordData &);
extern bool larger_Q_indicator(const ChordData &, const ChordData &);
extern bool superior_rm(const ChordData &, const ChordData &);

extern bool smaller_chroma(const ChordData &, const ChordData &);
extern bool smaller_chroma_old(const ChordData &, const ChordData &);
extern bool smaller_tension(const ChordData &, const ChordData &);
extern bool smaller_common(const ChordData &, const ChordData &);
extern bool smaller_sv(const ChordData &, const ChordData &);
extern bool smaller_t_size(const ChordData &, const ChordData &);
extern bool smaller_s_size(const ChordData &, const ChordData &);
extern bool smaller_root(const ChordData &, const ChordData &);
extern bool smaller_span(const ChordData &, const ChordData &);
extern bool smaller_sspan(const ChordData &, const ChordData &);
extern bool smaller_thickness(const ChordData &, const ChordData &);
extern bool smaller_g_center(const ChordData &, const ChordData &);
extern bool smaller_similarity(const ChordData &, const ChordData &);
extern bool smaller_sim_orig(const ChordData &, const ChordData &);
extern bool smaller_Q_indicator(const ChordData &, const ChordData &);
extern bool inferior_rm(const ChordData &, const ChordData &);

const int VAR_TOTAL = 16;
const char var[VAR_TOTAL] = {'P', 'N', 'T', 'K', 'C', 'a', 'A', 'm', 'h', 'g', 'S', 'Q', 'X', 'k', 'R', 'V'};
// name of parameters (a = S, A = SS, S = sv, k = KK)
extern bool (*compare[VAR_TOTAL][2])(const ChordData &, const ChordData &);
// to unify the compare functions
extern vector<int> rm_priority;
}
}

#endif
