#include "emoji-service.hpp"
#include "services/emoji-service/emoji.hpp"

void EmojiService::buildIndex() {
  for (const auto &emoji : StaticEmojiDatabase::orderedList()) {
    for (const auto &keyword : emoji.keywords) {
      m_index.indexLatinText(keyword, &emoji);
    }
  }
}

std::vector<const EmojiData *> EmojiService::search(std::string_view query) const {
  return m_index.prefixSearch(query);
}

EmojiService::EmojiService() { buildIndex(); }
