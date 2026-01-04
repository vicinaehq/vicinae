#pragma once
#include "common.hpp"
#include "template-engine/template-engine.hpp"
#include "ui/list-section-header.hpp"
#include "ui/vlist/vlist.hpp"
#include <algorithm>
#include <numeric>
#include <ranges>

namespace vicinae::ui {

/**
 * A VList model optimized to render data organized in sections.
 * This is the model used by most vertical lists in Vicinae.
 */
template <typename ItemType, typename SectionId = int> class SectionListModel : public VListModel {
public:
  using Item = ItemType;

  SectionListModel(QObject *parent = nullptr) : VListModel(parent) {}

  virtual int sectionCount() const = 0;
  virtual int sectionItemCount(SectionId id) const = 0;
  virtual int sectionItemHeight(SectionId id) const = 0;

  virtual ItemType sectionItemAt(SectionId id, int itemIdx) const = 0;

  virtual SectionId sectionIdFromIndex(int idx) const = 0;
  virtual std::string_view sectionName(SectionId id) const = 0;

  virtual WidgetType *createItemWidget(const ItemType &type) const = 0;
  virtual void refreshItemWidget(const ItemType &type, WidgetType *widget) const = 0;

  virtual StableID stableId(const ItemType &item) const = 0;
  virtual WidgetTag widgetTag(const ItemType &item) const { return InvalidTag; }

  int xAtIndex(Index index) const override { return 0; }

  struct SectionHeader {
    std::string_view name;
    int count;
  };

  struct SectionItem {
    ItemType data;
    int sectionIdx = -1;
    int itemIdx = -1;
  };

  std::optional<ItemType> fromIndex(Index idx) {
    const auto visitor =
        overloads{[&](const SectionItem &item) -> std::optional<ItemType> { return item.data; },
                  [](const SectionHeader &header) -> std::optional<ItemType> { return std::nullopt; }};

    return std::visit(visitor, fromFlatIndex(idx));
  }

  using FlattenedItem = std::variant<SectionItem, SectionHeader>;

protected:
  void viewportChanged(QSize size) override { rebuildMap(); }

  void rebuildMap() {
    int currentIndex = 0;
    int y = 0;

    m_cache.resize(count());

    for (int i = 0; i != sectionCount(); ++i) {
      auto id = sectionIdFromIndex(i);
      int itemCount = sectionItemCount(id);
      int itemHeight = sectionItemHeight(id);
      bool withHeader = itemCount > 0 && !sectionName(id).empty();

      if (withHeader) {
        auto &item = m_cache[currentIndex];
        item.y = y;
        item.height = HEADER_HEIGHT;
        item.sectionIdx = i;
        item.isSection = true;
        y += HEADER_HEIGHT;
        ++currentIndex;
      }

      for (int j = 0; j != itemCount; ++j) {
        auto &item = m_cache[currentIndex];
        item.y = y;
        item.height = itemHeight;
        item.sectionIdx = i;
        item.isSection = false;
        item.itemIdx = j;
        y += itemHeight;
        ++currentIndex;
      }
    }
    m_height = y;
  }

  void onDataChanged() override { rebuildMap(); }

  WidgetType *createWidget(Index idx) const final override {
    const auto visitor = overloads{
        [&](const SectionHeader &header) -> WidgetType * { return new OmniListSectionHeader("", "", 0); },
        [&](const SectionItem &item) -> WidgetType * { return createItemWidget(item.data); }};
    return std::visit(visitor, fromFlatIndex(idx));
  }

  WidgetTag widgetTag(Index idx) const final override {
    const auto visitor =
        overloads{[&](const SectionHeader &header) { return typeid(OmniListSectionHeader).hash_code(); },
                  [&](const SectionItem &item) { return widgetTag(item.data); }};
    return std::visit(visitor, fromFlatIndex(idx));
  }

  StableID stableId(Index idx) const final override {
    static std::hash<std::string_view> hasher = {};
    const auto visitor = overloads{[&](const SectionHeader &header) { return randomId(); },
                                   [&](const SectionItem &item) { return stableId(item.data); }};
    return std::visit(visitor, fromFlatIndex(idx));
  }

  static QString parseSectionName(std::string_view tmpl, int count) {
    TemplateEngine engine;
    engine.setVar("count", QString::number(count));
    return engine.build(QString::fromUtf8(tmpl.data(), tmpl.size()));
  }

  void refreshWidget(Index idx, WidgetType *widget) const final override {
    const auto visitor = overloads{[&](const SectionHeader &header) {
                                     static_cast<OmniListSectionHeader *>(widget)->setTitle(
                                         parseSectionName(header.name, header.count));
                                   },
                                   [&](const SectionItem &item) { refreshItemWidget(item.data, widget); }};
    return std::visit(visitor, fromFlatIndex(idx));
  }

  FlattenedItem fromFlatIndex(Index idx) const {
    auto &item = m_cache[idx];
    auto id = sectionIdFromIndex(item.sectionIdx);

    if (item.isSection) { return SectionHeader{.name = sectionName(id), .count = sectionItemCount(id)}; }

    return SectionItem{
        .data = sectionItemAt(id, item.itemIdx), .sectionIdx = item.sectionIdx, .itemIdx = item.itemIdx};
  }

  int count() const final override {
    int c = 0;
    for (int i = 0; i != sectionCount(); ++i) {
      auto id = sectionIdFromIndex(i);
      int itemCount = sectionItemCount(id);
      bool withHeader = itemCount > 0 && !sectionName(id).empty();
      c = c + itemCount + withHeader * 1;
    }
    return c;
  }

  std::optional<VListModel::Index> findItemIf(std::function<bool(const Item &item)> pred) {
    int c = 0;
    for (int i = 0; i != sectionCount(); ++i) {
      auto id = sectionIdFromIndex(i);
      int itemCount = sectionItemCount(id);
      bool withHeader = itemCount > 0 && !sectionName(id).empty();

      if (withHeader) { ++c; }

      const auto items =
          std::views::iota(0, itemCount) | std::views::transform([&](auto j) { return sectionItemAt(i, j); });

      for (const auto &item : items) {
        if (pred(item)) { return c; }
        ++c;
      }
    }
    return {};
  }

  int height() const final override { return m_height; }

  int heightAtIndex(Index idx) const final override { return m_cache[idx].y; }

  bool isAnchor(Index idx) const override {
    return std::holds_alternative<SectionHeader>(fromFlatIndex(idx));
  }

  bool isSelectable(Index idx) const final override {
    return !std::holds_alternative<SectionHeader>(fromFlatIndex(idx));
  }

  Index indexAtHeight(int targetHeight) const final override {
    int low = 0;
    int high = m_cache.size() - 1;

    while (low <= high) {
      int mid = low + (high - low) / 2;
      auto &item = m_cache[mid];

      if (targetHeight >= item.y && targetHeight <= item.y + item.height) { return mid; }

      if (targetHeight > item.y + item.height) {
        low = mid + 1;
      } else {
        high = mid - 1;
      }
    }

    return InvalidIndex;
  }

  int height(Index idx) const final override {
    const auto visitor = overloads{[&](const SectionHeader &header) -> int { return HEADER_HEIGHT; },
                                   [&](const SectionItem &item) -> int {
                                     return sectionItemHeight(sectionIdFromIndex(item.sectionIdx));
                                   }};
    return std::visit(visitor, fromFlatIndex(idx));
  }

private:
  struct CachedItem {
    int y = 0;
    int height = 0;
    bool isSection = false;
    int sectionIdx = 0;
    int itemIdx = 0;
  };

  static const constexpr int HEADER_HEIGHT = 30;
  int m_height = 0;
  std::vector<CachedItem> m_cache;
};
}; // namespace vicinae::ui
