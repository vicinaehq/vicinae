#pragma once
#include <vector>

struct EmojiInfo {
  const char *emoji;
  const char *description;
  const char *aliases[5];
  const char *tags[5];
  const char *category;
};

extern const std::vector<EmojiInfo> EMOJI_LIST;

class EmojiDatabase {
public:
  const std::vector<EmojiInfo> &list() { return EMOJI_LIST; }
};
