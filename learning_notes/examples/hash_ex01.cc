#include <cassert>
#include <iostream>
#include <string>
#include <tuple>

#include "absl/hash/hash.h"

int main() {
  std::string s = "hello";
  std::tuple<int, int> t{4, 2};

  // TODO: Verify HashOf(t) matches Hash<tuple>{}(t)
  assert(absl::HashOf(t) == absl::Hash<decltype(t)>{}(t));

  // TODO: Verify HashOf(s) matches Hash<string>{}(s)
  assert(absl::HashOf(s) == absl::Hash<std::string>{}(s));

  // TODO: Confirm type sensitivity (2 vs 2.0)
  assert(absl::HashOf(2) != absl::HashOf(2.0));

  (void)s;
  (void)t;
  std::cout << "OK\n";
  return 0;
}
