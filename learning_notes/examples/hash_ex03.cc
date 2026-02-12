#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "absl/hash/hash.h"

// A wrapper that hashes a byte sequence. The exercise is to avoid the classic
// pitfall: hashing contents without also hashing the length.
struct BytesView {
  const unsigned char* data;
  std::size_t size;

  friend bool operator==(const BytesView& a, const BytesView& b) {
    if (a.size != b.size) return false;
    for (std::size_t i = 0; i < a.size; ++i) {
      if (a.data[i] != b.data[i]) return false;
    }
    return true;
  }

  template <typename H>
  friend H AbslHashValue(H h, const BytesView& v) {
    // TODO: Combine the contiguous bytes.
    // TODO: Also combine v.size (critical!)
    // return h;  // Placeholder until TODOs are filled.
    return H::combine_contiguous(std::move(h), v.data, v.size);
  }
};

int main() {
  std::vector<unsigned char> a = {'a', 'b'};
  std::vector<unsigned char> b = {'a', 'b', 'c'};

  BytesView va{a.data(), a.size()};
  BytesView vb{b.data(), b.size()};

  // Two different ranges should not be equal.
  assert(!(va == vb));

  absl::Hash<BytesView> h;

  // TODO: After you implement AbslHashValue correctly, add a check that
  // h(va) != h(vb).
  assert(!(h(va) == h(vb)));

  std::cout << "OK\n";
  return 0;
}
