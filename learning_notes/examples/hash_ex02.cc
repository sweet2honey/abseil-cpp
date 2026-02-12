#include <cassert>
#include <iostream>
#include <utility>

#include "absl/hash/hash.h"

struct Point {
  int x;
  int y;

  friend bool operator==(const Point& a, const Point& b) {
    return a.x == b.x && a.y == b.y;
  }

  // TODO: Add AbslHashValue customization (friend, templated, uses H::combine)
  template <typename H>
  friend H AbslHashValue(H h, const Point& p) {
    return H::combine(std::move(h), p.x, p.y);
  }
};

int main() {
// #if !defined(HASH_EX02_DONE)
#if 0
  std::cout << "TODO: implement AbslHashValue for Point, then recompile with "
            << "-DHASH_EX02_DONE=1 to enable the checks.\n";
  return 0;
#else
  absl::Hash<Point> h;

  Point a{1, 2};
  Point b{1, 2};
  Point c{2, 1};

  // These must hold if AbslHashValue matches operator== fields.
  assert(a == b);
  assert(h(a) == h(b));

  // Different values should generally hash differently (not guaranteed, but
  // extremely likely for small examples).
  assert(!(a == c));
  assert(h(a) != h(c));

  std::cout << "OK\n";
  return 0;
#endif
}
