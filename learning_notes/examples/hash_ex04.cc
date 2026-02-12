#include <cassert>
#include <iostream>
#include <string>
#include <typeindex>

#include "absl/hash/hash.h"

class Interface {
 public:
  virtual ~Interface() = default;

  friend bool operator==(const Interface& a, const Interface& b) {
    // NOTE: This is intentionally simplistic for an exercise.
    // In real code you'd likely have a virtual Equals() or use type + fields.
    return std::type_index(typeid(a)) == std::type_index(typeid(b)) &&
           a.DebugString() == b.DebugString();
  }

  template <typename H>
  friend H AbslHashValue(H state, const Interface& value) {
    // Mix dynamic type to avoid collisions between different derived types.
    state = H::combine(std::move(state), std::type_index(typeid(value)));

    // Type erase the hash state so we can forward it to a virtual function.
    absl::HashState erased = absl::HashState::Create(&state);

    // TODO: Forward the erased state into value.HashValue(...).

    return state;
  }

 private:
  virtual void HashValue(absl::HashState state) const = 0;
  virtual std::string DebugString() const = 0;
};

class Impl : public Interface {
 public:
  Impl(int a, std::string b) : a_(a), b_(std::move(b)) {}

 private:
  void HashValue(absl::HashState state) const override {
    // TODO: Combine fields into the erased state.
    (void)state;
  }

  std::string DebugString() const override {
    return std::to_string(a_) + ":" + b_;
  }

  int a_;
  std::string b_;
};

int main() {
  absl::Hash<Interface> h;

  Impl x1{1, "hi"};
  Impl x2{1, "hi"};
  Impl y{2, "hi"};

  // TODO: Once the TODOs are filled, add checks for equality/inequality + hash
  // consistency.

  std::cout << "OK\n";
  return 0;
}
