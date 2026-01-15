#pragma once
#include "emoji/emoji.hpp"
#include "emoji/generated/db.hpp"
#include "ui/vlist/common/simple-grid-model.hpp"
#include "services/emoji-service/emoji-service.hpp"
#include "utils.hpp"
#include <cstdint>

class EmojiBrowserModel : public vicinae::ui::SimpleGridModel<EmojiData, int> {
public:
  enum class DisplayMode : std::uint8_t { Root, Search };

  void setSkinTone(std::optional<emoji::SkinTone> tone) { m_skinTone = tone; }

  void setGroupedEmojis(EmojiService::GroupedEmojis emojis) { m_grouped = emojis; }

  void setSearchResults(std::span<Scored<const EmojiData *>> results) { m_results = results; }

  void setPinnedEmojis(std::vector<const EmojiData *> pinned) { m_pinned = pinned; }

  void setRecentEmojis(std::vector<const EmojiData *> recent) { m_recent = recent; }

  void requestUpdate() const { emit dataChanged(); }

  void setDisplayMode(DisplayMode mode) { m_root = mode == DisplayMode::Root; }

  GridData createItemData(const EmojiData &item) const override {
    if (item.skinToneSupport) {
      return {.icon = ImageURL::emoji(getSkinTonedEmoji(item).c_str()),
              .tooltip = qStringFromStdView(item.name)};
    }

    return {.icon = ImageURL::emoji(qStringFromStdView(item.emoji)),
            .tooltip = qStringFromStdView(item.name)};
  }

  VListModel::StableID stableId(const EmojiData &item, int sectionId) const override {
    if (m_root) {
      switch (sectionId) {
      case 0:
        return hash(std::format("pinned.{}", item.emoji));
      case 1:
        return hash(std::format("recent.{}", item.emoji));
      default:
        return hash(item.emoji);
      }
    }

    return hash(item.emoji);
  }

  EmojiData sectionItemAt(int id, int itemIdx) const override {
    if (m_root) {
      switch (id) {
      case 0:
        return *m_pinned[itemIdx];
      case 1:
        return *m_recent[itemIdx];
      default:
        return *groupFromIdx(id).second[itemIdx];
      }
    }

    return *m_results[itemIdx].data;
  }

  int sectionCount() const override {
    if (m_root) return m_grouped.size() + 2;
    return 1;
  }

  double sectionAspectRatio(int id) const override { return 1; }
  int sectionColumns(int id) const override { return 8; }

  int sectionItemCount(int id) const override {
    if (m_root) {
      if (id == 0) return m_pinned.size();
      if (id == 1) return m_recent.size();
      return groupFromIdx(id).second.size();
    }

    return m_results.size();
  }

  int sectionIdFromIndex(int idx) const override { return idx; }

  std::string_view sectionName(int id) const override {
    if (m_root) {
      switch (id) {
      case 0:
        return "Pinned";
      case 1:
        return "Recently used";
      default:
        return groupFromIdx(id).first;
      }
    }
    return "Results";
  }

private:
  std::string getSkinTonedEmoji(const EmojiData &info) const {
    if (!info.skinToneSupport || !m_skinTone) return std::string(info.emoji);
    return emoji::applySkinTone(info.emoji, m_skinTone.value());
  }

  const std::pair<std::string_view, std::vector<const EmojiData *>> &groupFromIdx(int idx) const {
    return m_grouped[idx - 2];
  }

  bool m_root = true;
  std::span<Scored<const EmojiData *>> m_results;
  std::vector<const EmojiData *> m_pinned;
  std::vector<const EmojiData *> m_recent;
  EmojiService::GroupedEmojis m_grouped;
  std::optional<emoji::SkinTone> m_skinTone;
};
