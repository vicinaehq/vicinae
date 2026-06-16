#pragma once
#include <algorithm>
#include <cctype>
#include <string>
#include <string_view>
#include <vector>

namespace file_indexer::vocab {

inline constexpr size_t MIN_TOKEN_LENGTH = 3;
inline constexpr size_t MAX_TOKEN_LENGTH = 24;
inline constexpr size_t MIN_HEX_JUNK_LENGTH = 12;

// machine-generated tokens (hashes, ids...) make poor correction targets;
// expects an already-lowercased token
inline bool isJunkToken(std::string_view token) {
  if (token.size() > MAX_TOKEN_LENGTH) return true;

  auto isHexChar = [](unsigned char c) { return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'); };

  return token.size() >= MIN_HEX_JUNK_LENGTH && std::ranges::all_of(token, isHexChar);
}

inline bool isSkeletonVowel(unsigned char c) {
  return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
}

inline std::string skeletonizeToken(std::string_view token) {
  std::string skeleton;
  char last = 0;

  skeleton.reserve(token.size());

  for (unsigned char c : token) {
    c = static_cast<unsigned char>(std::tolower(c));

    if (!skeleton.empty()) {
      if (isSkeletonVowel(c)) continue;
      if (c == last) continue;
    }

    skeleton.push_back(static_cast<char>(c));
    last = static_cast<char>(c);
  }

  return skeleton;
}

inline std::string_view basenameView(std::string_view path) {
  if (auto const pos = path.rfind('/'); pos != std::string_view::npos) return path.substr(pos + 1);
  return path;
}

inline std::string_view fileExtensionView(std::string_view path) {
  if (auto const pos = path.rfind('.'); pos != std::string_view::npos) return path.substr(pos + 1);
  return path;
}

inline std::string_view dirnameView(std::string_view path) {
  if (auto const pos = path.rfind('/'); pos != std::string_view::npos) return path.substr(0, pos);
  return path;
}

/**
 * Splits a filename into vocabulary words: non-alphanumeric characters and camelCase
 * boundaries delimit tokens, which are then lowercased. Tokens shorter than
 * MIN_TOKEN_LENGTH, made of digits only, or junk (see isJunkToken) are dropped, as
 * they make poor spellfix correction targets.
 */
inline std::vector<std::string> tokenizeFilename(std::string_view name) {
  std::vector<std::string> tokens;
  std::string current;

  tokens.reserve(4);

  auto isTokenChar = [](unsigned char c) { return std::isalnum(c) != 0 || c >= 0x80; };
  auto flush = [&]() {
    bool const allDigits = std::ranges::all_of(current, [](unsigned char c) { return std::isdigit(c) != 0; });
    if (current.size() >= MIN_TOKEN_LENGTH && !allDigits && !isJunkToken(current)) {
      tokens.emplace_back(std::move(current));
    }
    current.clear();
  };

  for (size_t i = 0; i < name.size(); ++i) {
    unsigned char const c = name[i];

    if (!isTokenChar(c)) {
      flush();
      continue;
    }

    if (std::isupper(c) != 0 && !current.empty()) {
      unsigned char const prev = name[i - 1];
      bool const camelBoundary = std::islower(prev) != 0 || std::isdigit(prev) != 0;
      bool const acronymEnd = std::isupper(prev) != 0 && i + 1 < name.size() &&
                              std::islower(static_cast<unsigned char>(name[i + 1])) != 0;

      if (camelBoundary || acronymEnd) { flush(); }
    }

    current.push_back(static_cast<char>(std::tolower(c)));
  }

  flush();

  return tokens;
}

}; // namespace file_indexer::vocab
