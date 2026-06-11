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

/**
 * Machine-generated tokens (hashes, ids, minified asset names...) that no human would
 * ever type an approximation of. They make poor correction targets and needlessly
 * bloat the vocabulary. Expects an already-lowercased token.
 */
inline bool isJunkToken(std::string_view token) {
  if (token.size() > MAX_TOKEN_LENGTH) return true;

  auto isHexChar = [](unsigned char c) { return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'); };

  return token.size() >= MIN_HEX_JUNK_LENGTH && std::ranges::all_of(token, isHexChar);
}

inline std::string_view basenameView(std::string_view path) {
  if (auto const pos = path.rfind('/'); pos != std::string_view::npos) return path.substr(pos + 1);
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
