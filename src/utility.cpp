#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include "constant.h"

template<typename Out>
void split(const std::string &s, char delim, Out result) {
  std::istringstream iss(s);
  std::string item;
  while (std::getline(iss, item, delim)) {
    *result++ = item;
  }
}

std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  split(s, delim, std::back_inserter(elems));
  return elems;
}

std::vector<int> normal_form(const std::vector<int> &set) {
  const int len = set.size();
  if (len == 0) return {};
  int i_rec = 0;
  std::vector<int> best;
  for (int i = 0; i < len; ++i) {
    std::vector<int> intervals;
    for (int j = len - 1; j > 0; --j) {
      int interval = set[(i + j) % len] - set[i];
      if (interval < 0) interval += chordnovarw::ET_SIZE;
      intervals.push_back(interval);
    }
    if (i == 0 || intervals < best) {
      best = intervals;
      i_rec = i;
    }
  }
  std::vector<int> result;
  const int copy = set[i_rec];
  for (int j = 0; j < len; ++j) {
    int val = set[(i_rec + j) % len] - copy;
    if (val < 0) val += chordnovarw::ET_SIZE;
    result.push_back(val);
  }
  return result;
}

std::vector<int> set_intersect(const std::vector<int> &A, const std::vector<int> &B) {
  std::vector<int> result;
  std::set_intersection(A.begin(), A.end(), B.begin(), B.end(),
                        std::back_inserter(result));
  return result;
}

std::vector<int> set_union(const std::vector<int> &A, const std::vector<int> &B) {
  std::vector<int> result;
  std::set_union(A.begin(), A.end(), B.begin(), B.end(),
                 std::back_inserter(result));
  return result;
}

std::vector<int> set_complement(const std::vector<int> &A, const std::vector<int> &B) {
  std::vector<int> result;
  std::set_difference(A.begin(), A.end(), B.begin(), B.end(),
                      std::back_inserter(result));
  return result;
}

int sign(double x) {
  if (x > 0.0) return 1;
  if (x < 0.0) return -1;
  return 0;
}
