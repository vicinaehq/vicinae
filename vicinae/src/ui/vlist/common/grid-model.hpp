#pragma once
#include "common.hpp"
#include "ui/list-section-header.hpp"
#include "ui/vlist/vlist.hpp"
#include <algorithm>
#include <bits/ranges_algo.h>
#include <qdnslookup.h>

namespace vicinae::ui {

/**
 * A VList model optimized to render grid data, organized in sections.
 */
template <typename ItemType, typename SectionId = int> class GridModel : public VListModel {
public:
  using Item = ItemType;

  GridModel(QObject *parent = nullptr) : VListModel(parent) {}

  virtual int sectionCount() const = 0;
  virtual int sectionItemCount(SectionId id) const = 0;

  virtual int itemHeight(const ItemType &item, int width, double ratio) const = 0;

  virtual ItemType sectionItemAt(SectionId id, int itemIdx) const = 0;

  virtual double sectionAspectRatio(SectionId id) const = 0;
  virtual int sectionColumns(SectionId id) const = 0;

  virtual SectionId sectionIdFromIndex(int idx) const = 0;
  virtual std::string_view sectionName(SectionId id) const = 0;

  virtual WidgetType *createItemWidget(const ItemType &type) const = 0;
  virtual void refreshItemWidget(const ItemType &type, WidgetType *widget, SectionId sectionId) const = 0;

  virtual StableID stableId(const ItemType &item, SectionId id) const = 0;

  virtual WidgetTag widgetTag(const ItemType &item) const { return InvalidTag; }

  struct SectionHeader {
    std::string_view name;
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
  void rebuildMap() {
    int currentIndex = 0;
    int currentHeight = 0;

    m_cache.resize(count());

    for (int i = 0; i != sectionCount(); ++i) {
      auto id = sectionIdFromIndex(i);
      int itemCount = sectionItemCount(id);
      bool withHeader = itemCount > 0 && !sectionName(id).empty();
      int cols = sectionColumns(id);

      if (withHeader) {
        auto &item = m_cache[currentIndex];
        item.y = currentHeight;
        item.height = HEADER_HEIGHT;
        item.rowStart = true;
        item.isSection = true;
        item.sectionIdx = i;
        currentHeight += HEADER_HEIGHT;
        ++currentIndex;
      }

      int maxHeight = 0;

      for (int j = 0; j != itemCount; ++j) {
        auto &item = m_cache[currentIndex];

        if (j != 0 && j % cols == 0) {
          currentHeight += maxHeight + spacing();
          maxHeight = 0;
        }

        item.y = currentHeight;
        item.height = height(currentIndex);
        item.rowStart = j % cols == 0;
        item.isSection = false;
        item.sectionIdx = i;
        item.itemIdx = j;
        maxHeight = std::max(maxHeight, item.height);
        ++currentIndex;
      }

      if (maxHeight > 0) { currentHeight += maxHeight; }
    }

    m_height = currentHeight;
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
    const auto visitor = overloads{
        [&](const SectionHeader &header) { return randomId(); },
        [&](const SectionItem &item) { return stableId(item.data, sectionIdFromIndex(item.sectionIdx)); }};
    return std::visit(visitor, fromFlatIndex(idx));
  }

  void refreshWidget(Index idx, WidgetType *widget) const final override {
    const auto visitor =
        overloads{[&](const SectionHeader &header) {
                    static_cast<OmniListSectionHeader *>(widget)->setTitle(
                        QString::fromUtf8(header.name.data(), header.name.size()));
                  },
                  [&](const SectionItem &item) {
                    refreshItemWidget(item.data, widget, sectionIdFromIndex(item.sectionIdx));
                  }};
    return std::visit(visitor, fromFlatIndex(idx));
  }

  QSize allocateSize(VListModel::Index idx, QSize viewport) const override {
    const auto visitor =
        overloads{[&](const SectionHeader &header) { return QSize(viewport.width(), HEADER_HEIGHT); },
                  [&](const SectionItem &item) {
                    auto id = sectionIdFromIndex(item.sectionIdx);
                    int cols = sectionColumns(id);
                    double ratio = sectionAspectRatio(id);
                    int width = cellWidth(cols, viewport.width());
                    int height = itemHeight(item.data, width, ratio);
                    return QSize(width, height);
                  }};
    return std::visit(visitor, fromFlatIndex(idx));
  }

  int spacing() const override { return 10; }

  int cellWidth(int cols, int viewportWidth) const {
    int availableWidth = viewportWidth - spacing() * (cols - 1);
    return availableWidth / cols;
  }

  FlattenedItem fromFlatIndex(Index idx) const {
    int currentIndex = 0;

    for (int i = 0; i != sectionCount(); ++i) {
      auto id = sectionIdFromIndex(i);
      int itemCount = sectionItemCount(id);
      bool withHeader = itemCount > 0 && !sectionName(id).empty();

      if (withHeader) {
        if (currentIndex == idx) return SectionHeader{.name = sectionName(id)};
        ++currentIndex;
      }

      if (itemCount > 0 && idx >= currentIndex && idx < currentIndex + itemCount) {
        int j = idx - currentIndex;
        return SectionItem{.data = sectionItemAt(id, j), .sectionIdx = i, .itemIdx = j};
      }

      currentIndex += itemCount;
    }

    throw std::runtime_error("Invalid index, this should not happen");
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

  int height() const final override { return m_height; }

  int heightAtIndex(Index idx) const final override { return m_cache[idx].y; }

  bool isAnchor(Index idx) const override {
    return std::holds_alternative<SectionHeader>(fromFlatIndex(idx));
  }

  bool isSelectable(Index idx) const final override {
    return !std::holds_alternative<SectionHeader>(fromFlatIndex(idx));
  }

  int xAtIndex(Index index) const override {
    const auto visitor = overloads{[&](const SectionHeader &header) -> int { return 0; },
                                   [&](const SectionItem &item) -> int {
                                     auto id = sectionIdFromIndex(item.sectionIdx);
                                     int cols = sectionColumns(id);
                                     int w = cellWidth(cols, m_viewport.width());
                                     int n = item.itemIdx % cols;

                                     return n * w + (n * spacing());
                                   }};
    return std::visit(visitor, fromFlatIndex(index));
  }

  Index indexAtHeight(int targetHeight) const final override {
    int low = 0;
    int high = m_cache.size() - 1;

    while (low <= high) {
      int mid = low + (high - low) / 2;
      auto &item = m_cache[mid];

      if (targetHeight >= item.y && targetHeight <= item.y + item.height + spacing()) {
        for (int i = mid; i >= 0; --i) {
          if (m_cache[i].rowStart) return i;
        }
      }

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
                                     auto id = sectionIdFromIndex(item.sectionIdx);
                                     int w = cellWidth(sectionColumns(id), m_viewport.width());
                                     double ratio = sectionAspectRatio(id);
                                     return itemHeight(item.data, w, ratio);
                                   }};
    return std::visit(visitor, fromFlatIndex(idx));
  }

  void viewportChanged(QSize size) override {
    m_viewport = size;
    rebuildMap();
  }

private:
  struct CachedItem {
    int y;
    int height;
    bool rowStart = false;
    bool isSection = 0;
    int sectionIdx;
    int itemIdx;
  };

  QSize m_viewport;
  int m_height = 0;
  std::vector<CachedItem> m_cache;
  static const constexpr int HEADER_HEIGHT = 30;
};
}; // namespace vicinae::ui
