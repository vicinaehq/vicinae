#pragma once
// should only contain types, no other includes

template <class... Ts> struct overloads : Ts... {
  using Ts::operator()...;
};

class NonCopyable {
public:
  NonCopyable(const NonCopyable &) = delete;
  NonCopyable &operator=(const NonCopyable &) = delete;
  NonCopyable() = default;
};
