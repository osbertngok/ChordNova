#ifndef CHORDNOVARW_SRC_INCLUDE_IO_FORMATTER_H_
#define CHORDNOVARW_SRC_INCLUDE_IO_FORMATTER_H_

#include "algorithm/sorting.h"
#include <ostream>
#include <string>
#include <vector>

namespace chordnovarw::io {

/**
 * \brief Format progression results to an output stream.
 *
 * Legacy: The print_single() / print_continual() output formatting in chord.cpp.
 *
 * \param out        Output stream.
 * \param candidates Sorted candidate entries.
 * \param index      1-based index prefix for each line (0 = no numbering).
 */
void format_candidates(
    std::ostream& out,
    const std::vector<algorithm::CandidateEntry>& candidates,
    int start_index = 1);

} // namespace chordnovarw::io

#endif // CHORDNOVARW_SRC_INCLUDE_IO_FORMATTER_H_
