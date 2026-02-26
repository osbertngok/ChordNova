#ifndef CHORDNOVARW_SRC_INCLUDE_UTILITY_H_
#define CHORDNOVARW_SRC_INCLUDE_UTILITY_H_
#include <string>
#include <vector>

template<typename Out>
void split(const std::string &s, char delim, Out result);

std::vector<std::string> split(const std::string &s, char delim);

std::vector<int> normal_form(const std::vector<int> &set);

[[nodiscard]] std::vector<int> set_intersect(const std::vector<int> &A, const std::vector<int> &B);
[[nodiscard]] std::vector<int> set_union(const std::vector<int> &A, const std::vector<int> &B);
[[nodiscard]] std::vector<int> set_complement(const std::vector<int> &A, const std::vector<int> &B);
[[nodiscard]] int sign(double x);

#endif //CHORDNOVARW_SRC_INCLUDE_UTILITY_H_
