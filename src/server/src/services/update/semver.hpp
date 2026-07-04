#pragma once
#include <algorithm>
#include <compare>
#include <cstddef>
#include <optional>
#include <string_view>
#include <vector>

namespace vicinae {

struct Semver {
  std::vector<unsigned> components;

  static constexpr std::optional<Semver> parse(std::string_view text) {
    if (text.starts_with('v')) text.remove_prefix(1);
    if (text.empty()) return std::nullopt;

    Semver version;
    version.components.reserve(3);
    unsigned current = 0;
    bool hasDigit = false;

    for (char c : text) {
      if (c >= '0' && c <= '9') {
        current = current * 10 + static_cast<unsigned>(c - '0');
        hasDigit = true;
      } else if (c == '.') {
        if (!hasDigit) return std::nullopt;
        version.components.emplace_back(current);
        current = 0;
        hasDigit = false;
      } else {
        return std::nullopt;
      }
    }

    if (!hasDigit) return std::nullopt;
    version.components.emplace_back(current);

    return version;
  }

  constexpr std::strong_ordering operator<=>(const Semver &other) const {
    const size_t count = std::max(components.size(), other.components.size());

    for (size_t i = 0; i < count; ++i) {
      const unsigned lhs = i < components.size() ? components[i] : 0;
      const unsigned rhs = i < other.components.size() ? other.components[i] : 0;
      if (auto cmp = lhs <=> rhs; cmp != 0) return cmp;
    }

    return std::strong_ordering::equal;
  }

  constexpr bool operator==(const Semver &other) const { return (*this <=> other) == 0; }
};

} // namespace vicinae
