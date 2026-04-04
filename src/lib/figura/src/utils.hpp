#pragma once
#include <string>
#include <algorithm>
#include <cctype>

namespace detail {

inline std::string toCamelImpl(const std::string &s, bool capitalize_first) {
  std::string result;
  bool capitalize_next = capitalize_first;
  for (char c : s) {
    if (c == '_' || c == '-' || c == ' ') {
      capitalize_next = true;
    } else if (capitalize_next) {
      result += static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
      capitalize_next = false;
    } else {
      result += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
  }
  return result;
}

} // namespace detail

inline std::string toLower(const std::string &s) {
  std::string result = s;
  std::ranges::transform(result, result.begin(), [](unsigned char c) { return std::tolower(c); });
  return result;
}

inline std::string toSnakeCase(const std::string &s) {
  std::string result;
  for (std::size_t i = 0; i < s.size(); ++i) {
    char c = s[i];
    if (std::isupper(static_cast<unsigned char>(c))) {
      if (i > 0 && s[i - 1] != '_' && s[i - 1] != ' ' && s[i - 1] != '-') { result += '_'; }
      result += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    } else if (c == ' ' || c == '-') {
      if (!result.empty() && result.back() != '_') { result += '_'; }
    } else {
      result += c;
    }
  }
  return result;
}

inline std::string toCamelCase(const std::string &s) { return detail::toCamelImpl(s, false); }

inline std::string toPascalCase(const std::string &s) { return detail::toCamelImpl(s, true); }
