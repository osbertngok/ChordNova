#include "algorithm/validation.h"
#include "model/chordstatistics.h"
#include "model/bigramchordstatistics.h"
#include "service/voiceleading.h"
#include "constant.h"
#include "utility.h"
#include <algorithm>
#include <cmath>
#include <bitset>
#include <set>

namespace chordnovarw::algorithm {

using namespace chordnovarw::model;
using namespace chordnovarw::service;

// Helper: ensure candidate_stats is computed
static const OrderedChordStatistics& ensure_stats(
    ValidationContext& ctx, OrderedChord& chord) {
  if (!ctx.candidate_stats.has_value())
    ctx.candidate_stats.emplace(calculate_statistics(chord));
  return *ctx.candidate_stats;
}

// ── Individual Validators ───────────────────────────────────────────

bool validate_monotonicity(
    ValidationContext& /*ctx*/, OrderedChord& chord) {
  auto pitches = chord.get_pitches();
  for (size_t i = 1; i < pitches.size(); ++i) {
    if (pitches[i - 1] > pitches[i])
      return false;
  }
  return true;
}

bool validate_range(
    ValidationContext& ctx, OrderedChord& chord) {
  auto pitches = chord.get_pitches();
  if (pitches.empty()) return false;
  if (pitches.front().get_number() < ctx.config.range.lowest)
    return false;
  if (pitches.back().get_number() > ctx.config.range.highest)
    return false;
  return true;
}

bool validate_alignment(
    ValidationContext& ctx, OrderedChord& chord) {
  const auto& align = ctx.config.alignment;
  if (align.align_mode == AlignMode::Unlimited)
    return true;

  const auto& stats = ensure_stats(ctx, chord);

  if (align.align_mode == AlignMode::List) {
    for (const auto& allowed : align.alignment_list) {
      if (allowed == stats.alignment)
        return true;
    }
    return false;
  }

  // Interval mode
  auto pitches = chord.get_pitches();
  const int size = static_cast<int>(pitches.size());
  if (size < 2) return true;

  if (pitches[1].get_number() - pitches[0].get_number() < align.i_low)
    return false;
  if (pitches[size - 1].get_number() - pitches[size - 2].get_number() > align.i_high)
    return false;
  for (int i = 2; i <= size - 2; ++i) {
    int interval = pitches[i].get_number() - pitches[i - 1].get_number();
    if (interval > align.i_max || interval < align.i_min)
      return false;
  }
  return true;
}

bool validate_exclusion(
    ValidationContext& ctx, OrderedChord& chord) {
  const auto& excl = ctx.config.exclusion;
  if (!excl.enabled) return true;

  auto pitches = chord.get_pitches();

  // 1. Forbidden notes
  for (const auto& p : pitches) {
    for (int en : excl.exclusion_notes) {
      if (p.get_number() == en) return false;
    }
  }

  // 2. Forbidden roots
  const auto& stats = ensure_stats(ctx, chord);
  if (stats.root.has_value()) {
    int root_val = stats.root->value();
    for (int er : excl.exclusion_roots) {
      if (root_val == er) return false;
    }
  }

  // 3. Forbidden interval patterns
  if (!excl.exclusion_intervals.empty()) {
    const int m = static_cast<int>(pitches.size());
    std::vector<int> diffs;
    for (int i = 0; i < m; ++i) {
      for (int j = i + 1; j < m; ++j) {
        diffs.push_back(pitches[j].get_number() - pitches[i].get_number());
      }
    }

    for (const auto& ei : excl.exclusion_intervals) {
      int count = 0;
      for (int d : diffs) {
        int temp1 = d - ei.interval;
        int temp2 = d + ei.interval - ET_SIZE;
        if (temp1 % ET_SIZE == 0) {
          int octave = temp1 / ET_SIZE;
          if (octave >= ei.octave_min && octave <= ei.octave_max)
            ++count;
        } else if (temp2 % ET_SIZE == 0) {
          int octave = temp2 / ET_SIZE;
          if (octave >= ei.octave_min && octave <= ei.octave_max)
            ++count;
        }
      }
      if (count >= ei.num_min && count <= ei.num_max)
        return false;
    }
  }
  return true;
}

bool validate_pedal(
    ValidationContext& ctx, OrderedChord& chord) {
  const auto& pedal = ctx.config.pedal;
  if (!pedal.enabled || !ctx.config.continual)
    return true;

  auto pitches = chord.get_pitches();

  if (pedal.in_bass) {
    for (size_t i = 0; i < pedal.pedal_notes.size(); ++i) {
      if (i >= pitches.size()) return false;
      if (pitches[i].get_number() != pedal.pedal_notes[i])
        return false;
    }
    return true;
  }

  const size_t record_size = ctx.record.size();
  if (record_size % pedal.period == 0) {
    std::bitset<chordnovarw::ET_SIZE> chord_pcs;
    for (const auto& p : pitches) {
      chord_pcs.set(p.get_pitch_class().value());
    }
    for (int pc : pedal.pedal_notes_set) {
      if (!chord_pcs.test(pc))
        return false;
    }
    if (pedal.realign && record_size != 0) {
      std::vector<int> chord_midi;
      for (const auto& p : pitches) chord_midi.push_back(p.get_number());
      std::sort(chord_midi.begin(), chord_midi.end());
      bool all_same = true;
      for (int pn : pedal.pedal_notes) {
        if (!std::binary_search(chord_midi.begin(), chord_midi.end(), pn)) {
          all_same = false;
          break;
        }
      }
      if (all_same) return false;
    }
    return true;
  }

  std::vector<int> chord_midi;
  for (const auto& p : pitches) chord_midi.push_back(p.get_number());
  std::sort(chord_midi.begin(), chord_midi.end());
  for (int pn : pedal.pedal_notes) {
    if (!std::binary_search(chord_midi.begin(), chord_midi.end(), pn))
      return false;
  }
  return true;
}

bool validate_cardinality(
    ValidationContext& ctx, OrderedChord& chord) {
  const auto& range = ctx.config.range;
  const auto& stats = ensure_stats(ctx, chord);

  int m = static_cast<int>(stats.num_of_pitches);
  int n = static_cast<int>(stats.num_of_unique_pitch_classes);
  if (m < range.m_min || m > range.m_max)
    return false;
  if (n < range.n_min || n > range.n_max)
    return false;
  return true;
}

bool validate_single_chord_stats(
    ValidationContext& ctx, OrderedChord& chord) {
  const auto& range = ctx.config.range;
  const auto& stats = ensure_stats(ctx, chord);

  if (stats.thickness < range.h_min || stats.thickness > range.h_max)
    return false;

  if (stats.root.has_value()) {
    int r = stats.root->value();
    if (r < range.r_min || r > range.r_max)
      return false;
  }

  double g = stats.geometrical_center;
  if (g < range.g_min || g > range.g_max)
    return false;

  return true;
}

bool validate_scale_membership(
    ValidationContext& ctx, OrderedChord& chord) {
  const auto& scale = ctx.config.scale.overall_scale;
  if (scale.size() >= 12) return true;

  auto pitches = chord.get_pitches();
  std::bitset<chordnovarw::ET_SIZE> scale_bits;
  for (int s : scale) scale_bits.set(s);
  for (const auto& p : pitches) {
    if (!scale_bits.test(p.get_pitch_class().value()))
      return false;
  }
  return true;
}

bool validate_bass_and_library(
    ValidationContext& ctx, OrderedChord& chord) {
  const auto& stats = ensure_stats(ctx, chord);

  // Bass availability
  if (!stats.alignment.empty() && !ctx.config.bass.bass_avail.empty()) {
    int bass_align = stats.alignment[0];
    bool found = false;
    for (int ba : ctx.config.bass.bass_avail) {
      if (ba == bass_align) { found = true; break; }
    }
    if (!found) return false;
  }

  // Chord library
  const auto& library = ctx.config.chord_library.chord_library;
  if (!library.empty()) {
    auto pitches = chord.get_pitches();
    int set_id = 0;
    std::set<int> seen;
    for (const auto& p : pitches) {
      int pc = p.get_pitch_class().value();
      if (seen.insert(pc).second)
        set_id += (1 << pc);
    }
    bool found = false;
    for (int lib_id : library) {
      if (lib_id == set_id) { found = true; break; }
    }
    if (!found) return false;
  }

  return true;
}

bool validate_uniqueness(
    ValidationContext& ctx, OrderedChord& chord) {
  const auto& unique = ctx.config.uniqueness;
  if (unique.unique_mode != UniqueMode::RemoveDupType)
    return true;

  auto pitches = chord.get_pitches();
  int set_id = 0;
  std::set<int> seen;
  for (const auto& p : pitches) {
    int pc = p.get_pitch_class().value();
    if (seen.insert(pc).second)
      set_id += (1 << pc);
  }

  if (!ctx.rec_ids.insert(set_id).second)
    return false;

  return true;
}

bool validate_voice_leading(
    ValidationContext& ctx, OrderedChord& chord) {
  ctx.vl_result = find_voice_leading(ctx.prev_chord, chord);

  const auto& vl = ctx.config.voice_leading;
  const auto& vec = ctx.vl_result.vec;
  const int v_size = static_cast<int>(vec.size());

  for (int i = 0; i < v_size; ++i) {
    int abs_v = std::abs(vec[i]);
    if (abs_v < vl.vl_min || abs_v > vl.vl_max)
      return false;
  }

  int ascending_count = 0;
  int steady_count = 0;
  int descending_count = 0;
  for (int v : vec) {
    if (v > 0) ++ascending_count;
    else if (v == 0) ++steady_count;
    else ++descending_count;
  }

  switch (vl.vl_setting) {
    case VLSetting::Default: {
      if (v_size < 2) break;
      bool all_same_dir = true;
      int first_sign = 0;
      for (int v : vec) {
        if (v != 0) {
          int s = (v > 0) ? 1 : -1;
          if (first_sign == 0) first_sign = s;
          else if (s != first_sign) { all_same_dir = false; break; }
        } else {
          all_same_dir = false;
          break;
        }
      }
      if (all_same_dir && first_sign != 0)
        return false;
      break;
    }
    case VLSetting::Number:
      if (steady_count < vl.steady_min || steady_count > vl.steady_max)
        return false;
      if (ascending_count < vl.ascending_min || ascending_count > vl.ascending_max)
        return false;
      if (descending_count < vl.descending_min || descending_count > vl.descending_max)
        return false;
      break;
    case VLSetting::Percentage:
      if (steady_count < vl.steady_min * v_size || steady_count > vl.steady_max * v_size)
        return false;
      if (ascending_count < vl.ascending_min * v_size || ascending_count > vl.ascending_max * v_size)
        return false;
      if (descending_count < vl.descending_min * v_size || descending_count > vl.descending_max * v_size)
        return false;
      break;
  }

  // Common note check
  const auto& harmonic = ctx.config.harmonic;
  auto prev_pitches = ctx.prev_chord.get_pitches();
  auto curr_pitches = chord.get_pitches();
  std::vector<int> prev_midi, curr_midi;
  for (const auto& p : prev_pitches) prev_midi.push_back(p.get_number());
  for (const auto& p : curr_pitches) curr_midi.push_back(p.get_number());
  std::sort(prev_midi.begin(), prev_midi.end());
  std::sort(curr_midi.begin(), curr_midi.end());
  int common = static_cast<int>(set_intersect(prev_midi, curr_midi).size());
  if (common < harmonic.c_min || common > harmonic.c_max)
    return false;

  // sv check
  if (ctx.vl_result.sv < harmonic.sv_min || ctx.vl_result.sv > harmonic.sv_max)
    return false;

  // Root movement check
  if (ctx.config.root_movement.enabled) {
    const auto& stats = ensure_stats(ctx, chord);
    if (ctx.prev_stats.root.has_value() && stats.root.has_value()) {
      int rm = (stats.root->value() - ctx.prev_stats.root->value() + ET_SIZE) % ET_SIZE;
      if (rm > 6) rm = ET_SIZE - rm;
      const auto& priority = ctx.config.root_movement.rm_priority;
      if (rm < static_cast<int>(priority.size()) && priority[rm] == -1)
        return false;
    }
  }

  return true;
}

bool validate_similarity(
    ValidationContext& ctx, OrderedChord& chord) {
  const auto& harmonic = ctx.config.harmonic;
  const auto& stats = ensure_stats(ctx, chord);

  const int vl_max = ctx.config.voice_leading.vl_max;
  const double max_sv = vl_max * 1.0 *
      std::max(ctx.prev_stats.num_of_pitches, stats.num_of_pitches);

  double sim_temp = 0.0;
  if (max_sv > 0)
    sim_temp = std::pow(1.0 - ctx.vl_result.sv / max_sv, 1);

  if (ctx.prev_stats.root.has_value() && stats.root.has_value()
      && ctx.prev_stats.root.value() == stats.root.value()) {
    sim_temp = std::sqrt(sim_temp);
  }

  int similarity = static_cast<int>(std::round(100.0 * sim_temp));

  if (similarity < harmonic.x_min || similarity > harmonic.x_max)
    return false;

  // Extended similarity check
  if (ctx.config.similarity.enabled) {
    const auto& sim_cfg = ctx.config.similarity;
    for (size_t i = 0; i < sim_cfg.sim_period.size(); ++i) {
      int period = sim_cfg.sim_period[i];
      if (static_cast<int>(ctx.record.size()) >= period) {
        const auto& lookback_chord = ctx.record[ctx.record.size() - period];
        auto lookback_stats = calculate_statistics(lookback_chord);

        auto vl = find_voice_leading(lookback_chord, chord);
        double lb_max_sv = vl_max * 1.0 *
            std::max(lookback_stats.num_of_pitches, stats.num_of_pitches);
        double lb_sim = 0.0;
        if (lb_max_sv > 0)
          lb_sim = std::pow(1.0 - vl.sv / lb_max_sv, 1);
        if (lookback_stats.root.has_value() && stats.root.has_value()
            && lookback_stats.root.value() == stats.root.value()) {
          lb_sim = std::sqrt(lb_sim);
        }
        int lb_similarity = static_cast<int>(std::round(100.0 * lb_sim));

        if (lb_similarity < sim_cfg.sim_min[i] || lb_similarity > sim_cfg.sim_max[i])
          return false;
      }
    }
  }

  return true;
}

bool validate_span(
    ValidationContext& ctx, OrderedChord& chord) {
  const auto& harmonic = ctx.config.harmonic;

  auto pitches = chord.get_pitches();
  std::vector<int> curr_single_chroma;
  for (const auto& p : pitches) {
    int midi = p.get_number();
    curr_single_chroma.push_back(6 - (5 * (midi % ET_SIZE) + 6) % ET_SIZE);
  }

  std::vector<int> sorted_chroma(curr_single_chroma);
  std::sort(sorted_chroma.begin(), sorted_chroma.end());
  sorted_chroma.erase(
      std::unique(sorted_chroma.begin(), sorted_chroma.end()),
      sorted_chroma.end());

  int n = static_cast<int>(sorted_chroma.size());
  if (n <= 1) {
    if (0 < harmonic.s_min) return false;
    return true;
  }

  int min_span = sorted_chroma[n - 1] - sorted_chroma[0];
  for (int i = 1; i < n; ++i) {
    int rotated_span = sorted_chroma[i - 1] + ET_SIZE - sorted_chroma[i];
    if (rotated_span < min_span)
      min_span = rotated_span;
  }

  if (min_span < harmonic.s_min || min_span > harmonic.s_max)
    return false;

  if (!ctx.prev_single_chroma.empty()) {
    std::vector<int> merged = set_union(ctx.prev_single_chroma, sorted_chroma);
    int sspan = merged.back() - merged.front();
    int merged_n = static_cast<int>(merged.size());
    for (int i = 1; i < merged_n; ++i) {
      int rotated = merged[i - 1] + ET_SIZE - merged[i];
      if (rotated < sspan)
        sspan = rotated;
    }
    if (sspan < harmonic.ss_min || sspan > harmonic.ss_max)
      return false;
  }

  return true;
}

bool validate_q_indicator(
    ValidationContext& ctx, OrderedChord& chord) {
  const auto& harmonic = ctx.config.harmonic;
  const auto& stats = ensure_stats(ctx, chord);

  auto pitches = chord.get_pitches();
  std::vector<int> curr_midi;
  for (const auto& p : pitches) curr_midi.push_back(p.get_number());

  std::vector<int> curr_single_chroma;
  for (int m : curr_midi)
    curr_single_chroma.push_back(6 - (5 * (m % ET_SIZE) + 6) % ET_SIZE);

  std::vector<int> sorted_unique(curr_single_chroma);
  std::sort(sorted_unique.begin(), sorted_unique.end());
  sorted_unique.erase(
      std::unique(sorted_unique.begin(), sorted_unique.end()),
      sorted_unique.end());

  double curr_chroma_old = 0.0;
  for (int sc : sorted_unique) curr_chroma_old += sc;
  curr_chroma_old /= static_cast<double>(sorted_unique.size());
  curr_chroma_old = std::floor(curr_chroma_old * 100) / 100.0;

  if (curr_chroma_old - ctx.prev_chroma_old < -6.0)
    curr_chroma_old += ET_SIZE;
  else if (curr_chroma_old - ctx.prev_chroma_old > 6.0)
    curr_chroma_old -= ET_SIZE;

  double chroma = 0.0;
  if (!ctx.prev_single_chroma.empty()) {
    std::vector<int> prev_unique(ctx.prev_single_chroma);
    std::sort(prev_unique.begin(), prev_unique.end());
    prev_unique.erase(
        std::unique(prev_unique.begin(), prev_unique.end()),
        prev_unique.end());

    std::vector<int> A_only = set_complement(prev_unique, sorted_unique);
    std::vector<int> B_only = set_complement(sorted_unique, prev_unique);

    int val = 0;
    for (int a : A_only)
      for (int b : B_only)
        val += std::abs(a - b);

    int s = sign(curr_chroma_old - ctx.prev_chroma_old);
    chroma = s * 2.0 / 3.1416 * std::atan(val / 54.0) * 100.0;
  }

  double Q = chroma * (ctx.prev_stats.tension + stats.tension)
      / 2.0 / static_cast<double>(
          std::max(ctx.prev_stats.num_of_pitches, stats.num_of_pitches));

  if (Q < harmonic.q_min || Q > harmonic.q_max)
    return false;

  return true;
}

bool validate_vec_uniqueness(
    ValidationContext& ctx, OrderedChord& /*chord*/) {
  const auto& vec = ctx.vl_result.vec;
  long long vec_id = 0;
  long long base = 1;
  for (int v : vec) {
    vec_id += (v + 100) * base;
    base *= 200;
  }

  if (!ctx.vec_ids.insert(vec_id).second)
    return false;

  return true;
}

// ── Validation Pipeline ─────────────────────────────────────────────

ChordValidationPipeline::ChordValidationPipeline() {
  _validators = {
    validate_monotonicity,
    validate_range,
    validate_alignment,
    validate_exclusion,
    validate_pedal,
    validate_cardinality,
    validate_single_chord_stats,
    validate_scale_membership,
    validate_bass_and_library,
    validate_uniqueness,
    validate_voice_leading,
    validate_similarity,
    validate_span,
    validate_q_indicator,
    validate_vec_uniqueness
  };
}

bool ChordValidationPipeline::validate(
    ValidationContext& ctx, OrderedChord& chord) const {
  for (const auto& validator : _validators) {
    if (!validator(ctx, chord))
      return false;
  }
  return true;
}

} // namespace chordnovarw::algorithm
