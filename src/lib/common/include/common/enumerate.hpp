#pragma once
#include <cstddef>
#include <iterator>
#include <ranges>
#include <utility>

namespace vicinae {

template <std::ranges::input_range R>
  requires std::ranges::view<R>
class enumerate_view : public std::ranges::view_interface<enumerate_view<R>> {
  R m_base{};

public:
  enumerate_view() = default;
  constexpr explicit enumerate_view(R base) : m_base(std::move(base)) {}

  struct iterator {
    using inner_iter = std::ranges::iterator_t<R>;
    using value_type = std::pair<std::size_t, std::ranges::range_reference_t<R>>;
    using difference_type = std::ptrdiff_t;
    using iterator_concept = std::input_iterator_tag;

    std::size_t index{};
    inner_iter it{};

    constexpr value_type operator*() const { return value_type{index, *it}; }
    constexpr iterator &operator++() {
      ++index;
      ++it;
      return *this;
    }
    constexpr void operator++(int) { ++*this; }
    friend constexpr bool operator==(const iterator &a, const iterator &b) { return a.it == b.it; }
  };

  struct sentinel {
    std::ranges::sentinel_t<R> end;
    friend constexpr bool operator==(const iterator &it, const sentinel &s) { return it.it == s.end; }
  };

  constexpr auto begin() { return iterator{0, std::ranges::begin(m_base)}; }
  constexpr auto end() { return sentinel{std::ranges::end(m_base)}; }
};

template <typename R> enumerate_view(R &&) -> enumerate_view<std::views::all_t<R>>;

struct enumerate_fn {
  template <std::ranges::viewable_range R> constexpr auto operator()(R &&r) const {
    return enumerate_view{std::views::all(std::forward<R>(r))};
  }
};

inline constexpr enumerate_fn enumerate{};

template <std::ranges::viewable_range R> constexpr auto operator|(R &&r, enumerate_fn fn) {
  return fn(std::forward<R>(r));
}

} // namespace vicinae
