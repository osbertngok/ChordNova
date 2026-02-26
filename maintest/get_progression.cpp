#include "gtest/gtest.h"
#include "chord.h"
#include "functions.h"
using namespace std;

using namespace chordnova::chord;

bool preset_1(Chord& chord, const char* notes_text) {
  bool ret = chord.set_notes_from_text(string(notes_text));
  if (!ret) {
    return false;
  }
  // This is mandatory because m_max is used in Chord::get_progression to determine maximum number of parts.
  chord.initialize_with_notes(chord.m_notes, 0);
  // For testing purpose, let's start with something small:
  chord.m_max = 4;
  // This is mandatory. otherwise m_notes_size would not be initialized, and calling find_vec would likely crash.
  chord.set_param1();

  // The following are mimicking Chord::Main(), without using global variables like chordnova::functions::fout, if possible.
  chord.vl_min = 0; // disable vl_min
  chord.vl_max = 4; // traverse [-4, 4]
  // To set max_count correctly, we also need to set vl_min and vl_max.
  chord.set_max_count();
  chord.h_min = 0;
  chord.h_max = 50; // thickness_max does not have a good initial value
  chord.r_min = 0;
  chord.r_max = 11; // Same for root
  chord.g_min = 0;
  chord.g_max = 70; // Same for geometrical center
  chord.bass_avail = {1, 3, 5, 7, 9, 11, 13}; // bass_avail is a bitflag that indicate which note can be served as bass pitch

  chord.align_mode = AlignMode::Unlimited;
  chord.vl_setting = VLSetting::Default;
  chord.unique_mode = UniqueMode::RemoveDup;
  chord.continual = false;

  chord.t_min = 0;
  chord.t_max = 60;
  chord.k_min = 30;
  chord.k_max = 70;
  chord.c_min = 0;
  chord.c_max = 2;
  chord.s_min = 0;
  chord.s_max = 12;
  chord.ss_min = 0;
  chord.ss_max = 12;
  chord.sv_min = 4;
  chord.sv_max = 12;
  chord.q_min = -500;
  chord.q_max = 500;
  chord.x_min = 0;
  chord.x_max = 100;
  chord.kk_min = 0;
  chord.kk_max = 100;

  chord.enable_rm = false;
  chord.enable_ex = false;
  chord.enable_sim = false;
  return true;
}


void setup_minor_minor_chord_library(vector<int>& chord_library) {
  chordnova::functions::note_set_to_id({0, 4, 7}, chord_library); // Major Triad
  chordnova::functions::note_set_to_id({0, 3, 7}, chord_library); // Minor Triad
}

TEST(Chord, get_progression_1) {
  // initialize chord library
  setup_minor_minor_chord_library(chordnova::functions::chord_library);

  // Initialize set_expansion_indexes. This is important, otherwise @ref Chord::find_vec would yield incorrect result.
  chordnova::functions::set_expansion_indexes();

  Chord chord1;
  EXPECT_TRUE(preset_1(chord1, "C4 E4 G4"));

  chord1.init(static_cast<ChordData &>(chord1));
  EXPECT_EQ(chord1.m_new_chords.size(), 0);
  chord1.get_progression();
  EXPECT_EQ(chord1.m_new_chords.size(), 10);

  // E/G#
  EXPECT_EQ(chord1.m_new_chords[0].m_notes[0], 56); // G#3
  EXPECT_EQ(chord1.m_new_chords[0].m_notes[1], 59); // B3
  EXPECT_EQ(chord1.m_new_chords[0].m_notes[2], 64); // E4
  EXPECT_EQ(chord1.m_new_chords[0].m_notes[3], 68); // G#4

  // G/B
  EXPECT_EQ(chord1.m_new_chords[1].m_notes[0], 59); // B3
  EXPECT_EQ(chord1.m_new_chords[1].m_notes[1], 62); // D4
  EXPECT_EQ(chord1.m_new_chords[1].m_notes[2], 67); // G4
  EXPECT_EQ(chord1.m_new_chords[1].m_notes[3], 71); // B4

  // A
  EXPECT_EQ(chord1.m_new_chords[2].m_notes[0], 57); // A3
  EXPECT_EQ(chord1.m_new_chords[2].m_notes[1], 60); // C4
  EXPECT_EQ(chord1.m_new_chords[2].m_notes[2], 65); // F4
  EXPECT_EQ(chord1.m_new_chords[2].m_notes[3], 69); // A4

  // Gm/B-
  EXPECT_EQ(chord1.m_new_chords[3].m_notes[0], 58); // B-3
  EXPECT_EQ(chord1.m_new_chords[3].m_notes[1], 62); // D4
  EXPECT_EQ(chord1.m_new_chords[3].m_notes[2], 67); // G4
  EXPECT_EQ(chord1.m_new_chords[3].m_notes[3], 70); // B-4

  // E/B
  EXPECT_EQ(chord1.m_new_chords[4].m_notes[0], 59); // B3
  EXPECT_EQ(chord1.m_new_chords[4].m_notes[1], 64); // E4
  EXPECT_EQ(chord1.m_new_chords[4].m_notes[2], 68); // G#4
  EXPECT_EQ(chord1.m_new_chords[4].m_notes[3], 71); // B4
}
