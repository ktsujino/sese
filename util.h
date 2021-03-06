#ifndef _SESE_UTIL_H_
#define _SESE_UTIL_H_

#include <vector>
#include <string>

namespace sese {

template <class T>
std::vector<T> mergeSortedVectors(const std::vector<T>& first,
				  const std::vector<T>& second) {
  std::vector<T> merged;
  auto first_it = first.begin();
  auto second_it = second.begin();
  while (first_it != first.end() && second_it != second.end()) {
    if ((*first_it)  == (*second_it)) {
      // matched
      merged.push_back(*first_it);
      first_it = std::next(first_it);
      second_it = std::next(second_it);
    } else if ((*first_it) < (*second_it)) {
      first_it = std::next(first_it);
    } else {
      second_it = std::next(second_it);
    }
  }
  return merged;
}

std::vector<std::string> split(const std::string &s, char delim);

} // namespace sese

#endif // #ifndef _SESE_UTIL_H_
