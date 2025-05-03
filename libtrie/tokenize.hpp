#pragma once
#include <cctype>
#include <string_view>
#include <vector>

std::vector<std::string_view> tokenizeWordBoundaries(std::string_view view) {
  std::vector<std::string_view> words;
  size_t idx = 0;
  size_t lastIdx = 0;
  bool lowerFlag = false;

  while (idx < view.size()) {
    char ch = view[idx];

    if ((std::isspace(ch) || (isupper(ch) && lowerFlag)) && idx - lastIdx > 0) {
      words.emplace_back(std::string_view{view.begin() + lastIdx, view.begin() + lastIdx + idx});
      lastIdx = idx + 1;
      lowerFlag = false;
      continue;
    }

    lowerFlag = islower(ch);
  }

  if (idx - lastIdx > 0) {
    words.emplace_back(std::string_view{view.begin() + lastIdx, view.begin() + lastIdx + idx});
  }

  return words;
}
