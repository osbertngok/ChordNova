#include "io/formatter.h"
#include <iomanip>

namespace chordnovarw::io {

void format_candidates(
    std::ostream& out,
    const std::vector<algorithm::CandidateEntry>& candidates,
    int start_index) {

  for (size_t i = 0; i < candidates.size(); ++i) {
    const auto& entry = candidates[i];
    const auto& s = entry.stats;

    if (start_index > 0) {
      out << (start_index + static_cast<int>(i)) << ". ";
    }

    // Note names (with octave)
    out << s.name_with_octave;

    // Key statistics
    out << "  (";
    out << "sv=" << s.sv;
    out << " k=" << std::fixed << std::setprecision(1) << s.chroma;
    out << " s=" << s.span;
    out << " x=" << s.similarity;
    out << " Q=" << std::fixed << std::setprecision(1) << s.Q_indicator;
    out << ")";

    // Root and voice movement info
    if (!s.root_name.empty()) {
      out << " root=" << s.root_name;
    }

    out << "\n";
  }
}

} // namespace chordnovarw::io
