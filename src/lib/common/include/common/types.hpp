#pragma once
#include <variant>

// should only contain types, no other includes

template <class... Ts> struct overloads : Ts... {
  using Ts::operator()...;
};

template <typename V, typename... Fs> auto match(V &&v, Fs &&...fs) {
  return std::visit(overloads{std::forward<Fs>(fs)...}, std::forward<V>(v));
}

class NonCopyable {
public:
  NonCopyable(const NonCopyable &) = delete;
  NonCopyable &operator=(const NonCopyable &) = delete;
  NonCopyable() = default;
};
