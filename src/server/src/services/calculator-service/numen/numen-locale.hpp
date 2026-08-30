#pragma once
#include <array>
#include <exception>
#include <locale>
#include <optional>
#include <string>
#include <string_view>

namespace calculator {

// QLocale::name() returns "en_US". glibc locale names are typically "en_US.UTF-8",
// so constructing std::locale from the Qt name throws and every compute() fails.
inline bool isValidStdLocaleName(const std::string &name) {
  try {
    std::locale{name};
    return true;
  } catch (const std::exception &) { return false; }
}

inline std::optional<std::string> numenLocaleName(std::string_view qtName) {
  const std::string name{qtName};
  const auto candidates = std::to_array<std::string>({
      name + ".UTF-8",
      name + ".utf8",
      name,
  });

  for (const auto &candidate : candidates) {
    if (isValidStdLocaleName(candidate)) return candidate;
  }

  return std::nullopt;
}

} // namespace calculator
