#pragma once
#include <vector>
#include <string_view>
#include <array>
#include <unordered_map>

struct EmojiData {
  std::string_view emoji;
  std::string_view name;
  std::string_view group;
  std::vector<std::string_view> keywords;
  bool skinToneSupport = false;
};

class StaticEmojiDatabase {
public:
  StaticEmojiDatabase() = delete;
  static const std::array<EmojiData, 1906> &orderedList();
  static const std::unordered_map<std::string_view, const EmojiData *> &mapping();
  static const std::array<std::string_view, 9> &groups();
};
