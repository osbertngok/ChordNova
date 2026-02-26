#ifndef CHORDNOVARW_SRC_INCLUDE_IO_DATABASE_H_
#define CHORDNOVARW_SRC_INCLUDE_IO_DATABASE_H_

#include <string>
#include <vector>

namespace chordnovarw::io {

/**
 * \brief Read a chord database file and return all set_id bitmask values.
 *
 * Each line in the file contains space-separated pitch class integers (0-11).
 * Lines beginning with '/' or 't' are skipped as comments.
 *
 * For each chord, all 12 transpositions are generated, and for chords with
 * 3-7 notes, omissible notes (those whose scale degree is not essential) are
 * removed in all combinations, generating additional set_ids.
 *
 * Legacy: `dbentry()` in `functions.cpp`.
 *
 * \param filename Path to the chord database file.
 * \return Sorted, deduplicated vector of set_id bitmask values.
 */
[[nodiscard]] std::vector<int> read_chord_database(const std::string& filename);

/**
 * \brief Read an alignment database file.
 *
 * Skips 5 header lines, then reads one alignment per line as space-separated
 * voice indices. All cyclic rotations are pre-expanded.
 *
 * Legacy: `read_alignment()` in `functions.cpp`.
 *
 * \param filename Path to the alignment database file.
 * \return Vector of alignment patterns (each a vector of integers).
 */
[[nodiscard]] std::vector<std::vector<int>> read_alignment_database(const std::string& filename);

} // namespace chordnovarw::io

#endif // CHORDNOVARW_SRC_INCLUDE_IO_DATABASE_H_
