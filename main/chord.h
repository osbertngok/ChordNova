// ChordNova v3.0 [Build: 2021.1.14]
// (c) 2020 Wenge Chen, Ji-woon Sim.
// chord.h

#ifndef CHORD
#define CHORD

#include <ctime>
#include <fstream>
#include <vector>

#include "constant.h"
#include "chorddata.h"
#ifdef QT_CORE_LIB
#include <QApplication>
#include <QProgressDialog>
#endif

using std::vector;

namespace chordnova {
namespace chord {
enum OutputMode { Both, MidiOnly, TextOnly };
enum UniqueMode { Disabled, RemoveDup, RemoveDupType };
enum AlignMode { Interval, List, Unlimited };
enum VLSetting { Percentage, Number, Default };
enum SubstituteObj { Postchord, Antechord, BothChords };

struct intervalData {
  int interval;
  int octave_min;
  int octave_max;
  int num_min;
  int num_max;
};

/**
 * \brief Chord Generation / Substitution Process Manager.
 *
 * This is more like a Chord Manager class.
 * It contains several methods and store the state in itself.
 */
class Chord : public ChordData {

 public:
  Chord();
  Chord(const vector<int> &_notes, double _chroma_old = 0.0);
  Chord(const Chord &);

  /**
   * \brief The language used for output. Support either Chinese and English.
   */
  Language language;
  /**
   * \brief The path of the output file.
   *
   * Eventually txt file would be output_path + output_name + ".txt",
   * midi file would be output_path +output_name + ".mid".
   * Used in @ref Chord::Main().
   */
  char output_path[100];
  /**
   * \brief The file name of the output file.
   *
   * Eventually txt file would be output_path + output_name + ".txt",
   * midi file would be output_path +output_name + ".mid".
   * Used in @ref Chord::Main().
   */
  char output_name[100];
  /**
   * \brief Whether it is a one-off generation, or we would like to generate a series of chords.
   *
   * The behavior might differ in @ref Chord::set_new_chords .
   */
  bool continual;
  /**
   * \brief Whether to output txt, mid or both.
   */
  OutputMode output_mode;
  int loop_count;
  bool m_unchanged;
  bool nm_same;
  char database[2][100];  // English and Chinese name of chord database
  char database_filename[100];
  int database_size;
  bool enable_pedal;
  bool automatic;
  bool connect_pedal;
  bool interlace;
  char str_notes[100];
  UniqueMode unique_mode;
  vector<int> bass_avail;
  char align_db[2][100];  // English and Chinese name of align database
  char align_db_filename[100];
  int align_db_size;
  AlignMode align_mode;
  bool in_bass;
  bool realign;
  int period;
  VLSetting vl_setting;
  bool enable_steady;
  bool enable_ascending;
  bool enable_descending;
  bool custom_vl_range;
  bool enable_rm;
  bool enable_ex;
  bool enable_sim;
  char exclusion[200];
  char str_sim[100];
  vector<int> exclusion_notes;
  vector<int> exclusion_roots;
  vector<intervalData> exclusion_intervals;
  vector<int> sim_period;
  vector<int> sim_min;
  vector<int> sim_max;
  char sort_order[30]; // It is also used to determine which parameter is enabled in 'moreparamgui'.

  vector<int> ante_notes;
  vector<int> post_notes;
  vector<int> reduced_ante_notes;
  vector<int> reduced_post_notes;
  // Since we use set to represent a chord in chord substitution,
  // we have to apply the same rules to the chords in chord analysis.
  char str_ante_notes[100];
  char str_post_notes[100];
  int sample_size;
  bool test_all;
  SubstituteObj object;
  bool detailed_ref;
  char output_name_sub[100];
  OutputMode output_mode_sub;
  char reset_list[30];
  char percentage_list[30];
  /**
   * It is also used to determine which parameter is enabled in 'subsettingsgui'.
   */
  char sort_order_sub[30];

  double k_min, k_max;
  double kk_min, kk_max;
  double t_min, t_max;
  int c_min, c_max;
  int sv_min, sv_max;
  int m_min, m_max;
  int n_min, n_max;
  int r_min, r_max;
  int s_min, s_max;
  int ss_min, ss_max;
  double h_min, h_max;
  int g_min, g_max;
  int x_min, x_max;
  double q_min, q_max;
  /**
   * \brief highest range of @ref Chord::m_notes
   */
  int highest;
  /**
   * \brief lowest range of @ref Chord::m_notes
   */
  int lowest;
  /**
   * Unknown
   */
  int vl_min;
  /**
   * \brief Range of movement. Used in @ref Chord::set_new_chords().
   *
   * All mutation from the expanded chord would be considered,
   * as long as the interval between the original note and the new note
   * equals or less than vl_max (voice leading max).
   */
  int vl_max;
  int i_min, i_max, i_high, i_low; // range of interval
  double steady_min, steady_max;
  double ascending_min, ascending_max;
  double descending_min, descending_max;

  int k_reset_value, k_radius;
  int kk_reset_value, kk_radius;
  int t_reset_value, t_radius;
  int c_reset_value, c_radius;
  int sv_reset_value, sv_radius;
  int n_reset_value, n_radius;
  int r_reset_value, r_radius;
  int s_reset_value, s_radius;
  int ss_reset_value, ss_radius;
  int x_reset_value, x_radius;
  int p_reset_value, p_radius;
  int q_reset_value, q_radius;

  double k_min_sub, k_max_sub;
  double kk_min_sub, kk_max_sub;
  double t_min_sub, t_max_sub;
  double c_min_sub, c_max_sub;
  double sv_min_sub, sv_max_sub;
  double n_min_sub, n_max_sub;
  double r_min_sub, r_max_sub;
  double s_min_sub, s_max_sub;
  double ss_min_sub, ss_max_sub;
  double x_min_sub, x_max_sub;
  double p_min_sub, p_max_sub;
  double q_min_sub, q_max_sub;

  clock_t begin, begin_progr, end;
  clock_t begin_sub, begin_loop_sub, end_sub;
  /**
   * \brief expansion counter
   */
  int exp_count;
  /**
   * \brief progression counter
   */
  int m_progr_count;
  /**
   * \brief size of @ref Chord::m_new_chords
   */
  int c_size;
  /**
   * \brief size of record_ante / record_post
   */
  int sub_size;
  /**
   *\brief  an integer representing 'pitch_class_set'; unique for different 'pitch_class_set's
   */
  int set_id;
  /**
   * \brief an integer representing 'vec'; unique for different 'vec's
   */
  long long vec_id;
  /**
   * \brief total number of possible movement vectors
   */
  long long max_cnt;
  /**
   * \brief contains 'set_id' of all 12 transpositions of 'pitch_class_set'
   */
  vector<int> rec_id;
  /**
   * \brief contains the 'vec_id' of generated chords in a single progression
   */
  vector<long long> vec_ids;
  /**
   * \brief contains the generated chords in continual mode.
   */
  vector<ChordData> m_record;
  /**
   * \brief contains the generated chords in a single progression.
   */
  vector<ChordData> m_new_chords;
  /**
  * \brief contains antechords in substitutions
  */
  vector<ChordData> record_ante;
  /**
   * \brief contains postchords in substitutions
   */
  vector<ChordData> record_post;
  /**
   * \brief contains all possible chords for substitution.
   */
  vector<vector<int>> sub_library;

  void initialize_with_notes(const vector<int> &_notes, double _chroma_old);
  void set_max_count();
  void init(ChordData &);
  void set_param1();
  void get_progression();
  void expand(Chord &, const int &, const int &);
  void set_new_chords(Chord &);
  void next(vector<int> &);
  bool valid(Chord &);
  bool valid_alignment(Chord &);
  bool valid_exclusion(Chord &);
  bool include_pedal(Chord &);
  void _find_vec(Chord &);
  void set_param2(Chord &, bool, bool);
  int set_similarity(Chord &chord1, Chord &chord2, bool in_substitution, const int &period = 1);
  void set_span(Chord &, bool);
  void set_chroma_old();
  void set_chroma(Chord &);
  void set_name();
  void set_vec_id(Chord &);
  bool valid_vec(Chord &);
  bool valid_sim(Chord &);
  void sort_results(vector<ChordData> &, bool);
  void print_single();
  void print_continual();
  void print_stats();
  void print_end();
  void to_midi();
  bool set_notes_from_text(const std::string& str_notes);
  void check_initial();
  void choose_initial();

  int &get_set_id();
  void _set_vl_max(const int &);
  void Main();
  // Used in utilities. This function is needed because 'similarity' is related to 'vl_max'.
  void find_vec(Chord &new_chord, bool in_analyser = false, bool in_substitution = false);
  // interface of '_find_vec'

#ifdef QT_CORE_LIB
  protected:
      QProgressDialog* prgdialog;
      QProgressDialog* prgdialog_sub;
      QString labeltext;
      QString labeltext_sub;
      void set_est_time(const int&, bool);
      void abort(bool);

      void analyse();
      void substitute();
      void set_param_center();
      void set_param_range();
      void set_sub_library();
      bool valid_sub(Chord&, Chord&);
      bool valid_single_chord(Chord&);
      void print_sub();
      void print_stats_sub();
      void to_midi_sub();
#endif
};

extern const double _tension[ET_SIZE];
extern const int restriction[ET_SIZE];
extern vector<int> overall_scale;

vector<int> calculate_pitch_class_set(const vector<int> &notes);
float calculate_tension(const vector<int> &notes);

}
}
#endif
