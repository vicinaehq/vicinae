#pragma once
#include <variant>

// should only contain types, no other includes

template <class... Ts> struct overloads : Ts... {
  using Ts::operator()...;
};

template <class... Ts> struct match : Ts... {
  using Ts::operator()...;
};

template <class V, class... Ts> auto operator|(V &&variant, match<Ts...> &&m) {
  return std::visit(std::move(m), std::forward<V>(variant));
}

class NonCopyable {
public:
  NonCopyable(const NonCopyable &) = delete;
  NonCopyable &operator=(const NonCopyable &) = delete;
  NonCopyable() = default;
};
