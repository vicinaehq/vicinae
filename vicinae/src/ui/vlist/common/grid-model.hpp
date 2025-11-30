#pragma once
#include "common.hpp"
#include "extend/grid-model.hpp"
#include "ui/list-section-header.hpp"
#include "ui/vlist/vlist.hpp"

namespace vicinae::ui {

/**
 * A VList model optimized to render data organized in sections.
 * This is the model used by most vertical lists in Vicinae.
 *
 * Currently, this model suffer from one notable limitation: for a given section, all items
 * are considered to have the same height. This restriction does not apply to the model as a whole,
 * but only individual sections.
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
  virtual void refreshItemWidget(const ItemType &type, WidgetType *widget) const = 0;

  virtual StableID stableId(const ItemType &item) const = 0;

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
    const auto visitor = overloads{[&](const SectionHeader &header) { return hasher(header.name); },
                                   [&](const SectionItem &item) { return stableId(item.data); }};
    return std::visit(visitor, fromFlatIndex(idx));
  }

  void refreshWidget(Index idx, WidgetType *widget) const final override {
    const auto visitor = overloads{[&](const SectionHeader &header) {
                                     static_cast<OmniListSectionHeader *>(widget)->setTitle(
                                         QString::fromUtf8(header.name.data(), header.name.size()));
                                   },
                                   [&](const SectionItem &item) { refreshItemWidget(item.data, widget); }};
    return std::visit(visitor, fromFlatIndex(idx));
  }

  QSize allocateSize(VListModel::Index idx, QSize viewport) const override {
    const auto visitor =
        overloads{[&](const SectionHeader &header) { return QSize(viewport.width(), HEADER_HEIGHT); },
                  [&](const SectionItem &item) {
                    auto id = sectionIdFromIndex(item.sectionIdx);
                    size_t cols = sectionColumns(id);
                    double ratio = sectionAspectRatio(id);
                    int width = cellWidth(cols, viewport.width());
                    int height = itemHeight(item.data, width, ratio);
                    return QSize(width, height);
                  }};
    return std::visit(visitor, fromFlatIndex(idx));
  }

  size_t spacing() const override { return 0; }

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

  size_t count() const final override {
    int c = 0;
    for (int i = 0; i != sectionCount(); ++i) {
      auto id = sectionIdFromIndex(i);
      int itemCount = sectionItemCount(id);
      bool withHeader = itemCount > 0 && !sectionName(id).empty();
      c = c + itemCount + withHeader * 1;
    }
    return c;
  }

  size_t height() const final override {
    int currentIndex = 0;
    int currentHeight = 0;

    for (int i = 0; i != sectionCount(); ++i) {
      auto id = sectionIdFromIndex(i);
      int itemCount = sectionItemCount(id);
      bool withHeader = itemCount > 0 && !sectionName(id).empty();
      int cols = sectionColumns(id);

      if (withHeader) {
        currentHeight += HEADER_HEIGHT;
        ++currentIndex;
      }

      int maxHeight = 0;

      for (int j = 0; j != itemCount; ++j) {
        if (j != 0 && j % cols == 0) {
          currentHeight += maxHeight + spacing();
          maxHeight = 0;
        }
        maxHeight = std::max(maxHeight, (int)height(currentIndex));
        ++currentIndex;
      }

      if (maxHeight > 0) { currentHeight += maxHeight; }
    }

    return currentHeight;
  }

  size_t heightAtIndex(Index idx) const final override {
    if (auto it = m_heightCache.find(idx); it != m_heightCache.end()) { return it->second; }

    int currentIndex = 0;
    int currentHeight = 0;

    for (int i = 0; i != sectionCount(); ++i) {
      auto id = sectionIdFromIndex(i);
      int itemCount = sectionItemCount(id);
      bool withHeader = itemCount > 0 && !sectionName(id).empty();
      int cols = sectionColumns(id);

      if (withHeader) {
        if (currentIndex == idx) {
          m_heightCache.insert({idx, currentHeight});
          return currentHeight;
        }
        currentHeight += HEADER_HEIGHT + spacing();
        ++currentIndex;
      }

      int maxHeight = 0;

      for (int j = 0; j != itemCount; ++j) {
        if (j != 0 && j % cols == 0) {
          currentHeight += maxHeight + spacing();
          maxHeight = 0;
        }
        if (currentIndex == idx) {
          m_heightCache.insert({idx, currentHeight});
          return currentHeight;
        }
        maxHeight = std::max(maxHeight, (int)height(currentIndex));
        ++currentIndex;
      }

      if (currentIndex == idx) return currentHeight;

      if (maxHeight > 0) { currentHeight += maxHeight; }
    }

    return currentHeight;
  }

  bool isAnchor(Index idx) const override {
    return std::holds_alternative<SectionHeader>(fromFlatIndex(idx));
  }

  bool isSelectable(Index idx) const final override {
    return !std::holds_alternative<SectionHeader>(fromFlatIndex(idx));
  }

  int xAtIndex(Index index) const override {
    const auto visitor = overloads{[&](const SectionHeader &header) -> size_t { return 0; },
                                   [&](const SectionItem &item) -> size_t {
                                     auto id = sectionIdFromIndex(item.sectionIdx);
                                     int cols = sectionColumns(id);
                                     int w = cellWidth(cols, m_viewport.width());
                                     int n = item.itemIdx % cols;

                                     return n * w + (n * spacing());
                                   }};
    return std::visit(visitor, fromFlatIndex(index));
  }

  Index indexAtHeight(int targetHeight) const final override {
    int currentIndex = 0;
    int currentHeight = 0;

    for (int i = 0; i != sectionCount(); ++i) {
      auto id = sectionIdFromIndex(i);
      int itemCount = sectionItemCount(id);
      bool withHeader = itemCount > 0 && !sectionName(id).empty();
      int cols = sectionColumns(id);

      if (withHeader) {
        if (targetHeight < currentHeight + HEADER_HEIGHT) return currentIndex;
        currentHeight += HEADER_HEIGHT;
        ++currentIndex;
      }

      int maxHeight = 0;
      int prevIdx = 0;

      for (int j = 0; j != itemCount; ++j) {
        if (j != 0 && j % cols == 0) {
          currentHeight += maxHeight + spacing();
          maxHeight = 0;
          if (targetHeight < currentHeight) return prevIdx;
          prevIdx = currentIndex;
        }

        maxHeight = std::max(maxHeight, (int)height(currentIndex));
        ++currentIndex;
      }

      if (maxHeight > 0) {
        if (targetHeight < currentHeight + maxHeight) return currentIndex;
        currentHeight += maxHeight;
      }
    }

    return InvalidIndex;
  }

  size_t height(Index idx) const final override {
    const auto visitor = overloads{[&](const SectionHeader &header) -> size_t { return HEADER_HEIGHT; },
                                   [&](const SectionItem &item) -> size_t {
                                     auto id = sectionIdFromIndex(item.sectionIdx);
                                     int w = cellWidth(sectionColumns(id), m_viewport.width());
                                     double ratio = sectionAspectRatio(id);
                                     return itemHeight(item.data, w, ratio);
                                   }};
    return std::visit(visitor, fromFlatIndex(idx));
  }

  void viewportChanged(QSize size) override {
    m_viewport = size;
    m_heightCache.clear();
  }

private:
  QSize m_viewport;
  mutable std::unordered_map<Index, size_t> m_heightCache;
  static const constexpr size_t HEADER_HEIGHT = 30;
};
}; // namespace vicinae::ui
