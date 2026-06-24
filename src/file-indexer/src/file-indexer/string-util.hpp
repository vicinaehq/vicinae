#pragma once
#include <algorithm>
#include <cctype>
#include <string>
#include <string_view>

namespace file_indexer {

inline char asciiLower(unsigned char c) { return static_cast<char>(std::tolower(c)); }

inline std::string asciiLowercase(std::string_view value) {
  std::string normalized{value};
  std::ranges::transform(normalized, normalized.begin(), asciiLower);
  return normalized;
}

} // namespace file_indexer
