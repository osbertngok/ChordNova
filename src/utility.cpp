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
  const int A_size = A.size(), B_size = B.size();
  std::vector<int> result;
  int i = 0, j = 0;
  while (i < A_size && j < B_size) {
    if (A[i] > B[j]) ++j;
    else if (A[i] < B[j]) ++i;
    else {
      result.push_back(A[i]);
      ++i;
      ++j;
    }
  }
  return result;
}

std::vector<int> set_union(const std::vector<int> &A, const std::vector<int> &B) {
  std::vector<int> result(A);
  result.insert(result.end(), B.begin(), B.end());
  std::sort(result.begin(), result.end());
  result.erase(std::unique(result.begin(), result.end()), result.end());
  return result;
}

std::vector<int> set_complement(const std::vector<int> &A, const std::vector<int> &B) {
  const int A_size = A.size(), B_size = B.size();
  std::vector<int> result;
  int i = 0, j = 0;
  while (i < A_size && j < B_size) {
    while (i < A_size && j < B_size && A[i] > B[j]) ++j;
    while (i < A_size && j < B_size && A[i] <= B[j]) {
      if (A[i] < B[j])
        result.push_back(A[i]);
      ++i;
    }
  }
  for (int k = i; k < A_size; ++k)
    result.push_back(A[k]);
  return result;
}

int sign(double x) {
  if (x > 0.0) return 1;
  if (x < 0.0) return -1;
  return 0;
}
