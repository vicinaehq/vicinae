#pragma once
#include "common.hpp"
#include "ui/list-section-header.hpp"
#include "ui/vlist/vlist.hpp"
#include <qobject.h>

namespace vicinae::ui {

/**
 * A VList model optimized to render data organized in sections.
 * This is the model used by most vertical lists in Vicinae.
 *
 * Currently, this model suffer from one notable limitation: for a given section, all items
 * are considered to have the same height. This restriction does not apply to the model as a whole,
 * but only individual sections.
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
  virtual WidgetTag widgetTag(const ItemType &item) const = 0;

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
    int height = 0;
    for (int i = 0; i != sectionCount(); ++i) {
      auto id = sectionIdFromIndex(i);
      int itemCount = sectionItemCount(id);
      int itemHeight = sectionItemHeight(id);
      bool withHeader = itemCount > 0 && !sectionName(id).empty();
      height = height + itemCount * itemHeight + withHeader * HEADER_HEIGHT;
    }
    return height;
  }

  size_t heightAtIndex(Index idx) const final override {
    int currentIndex = 0;
    int height = 0;

    for (int i = 0; i != sectionCount(); ++i) {
      auto id = sectionIdFromIndex(i);
      int itemCount = sectionItemCount(id);
      int itemHeight = sectionItemHeight(id);
      bool withHeader = itemCount > 0 && !sectionName(id).empty();

      if (withHeader) {
        if (currentIndex == idx) return height;
        height += HEADER_HEIGHT;
        ++currentIndex;
      }

      int sectionHeight = itemCount * itemHeight;

      if (itemCount > 0 && idx >= currentIndex && idx < currentIndex + itemCount) {
        int itemIdx = idx - currentIndex;
        return height + itemIdx * itemHeight;
      }

      currentIndex += itemCount;
      height += sectionHeight;
    }

    return height;
  }

  bool isAnchor(Index idx) const override {
    return std::holds_alternative<SectionHeader>(fromFlatIndex(idx));
  }

  bool isSelectable(Index idx) const final override {
    return !std::holds_alternative<SectionHeader>(fromFlatIndex(idx));
  }

  Index indexAtHeight(int targetHeight) const final override {
    int currentIndex = 0;
    int height = 0;

    for (int i = 0; i != sectionCount(); ++i) {
      auto id = sectionIdFromIndex(i);
      int itemCount = sectionItemCount(id);
      int itemHeight = sectionItemHeight(id);
      bool withHeader = itemCount > 0 && !sectionName(id).empty();

      if (withHeader) {
        if (targetHeight < height + HEADER_HEIGHT) return currentIndex;
        height += HEADER_HEIGHT;
        ++currentIndex;
      }

      int sectionHeight = itemHeight * itemCount;

      if (itemCount > 0) {
        if (targetHeight < height + sectionHeight) {
          return currentIndex + (targetHeight - height) / itemHeight;
        }
      }

      height += sectionHeight;
      currentIndex += itemCount;
    }

    return InvalidIndex;
  }

  size_t height(Index idx) const final override {
    const auto visitor = overloads{[&](const SectionHeader &header) -> size_t { return HEADER_HEIGHT; },
                                   [&](const SectionItem &item) -> size_t {
                                     return sectionItemHeight(sectionIdFromIndex(item.sectionIdx));
                                   }};
    return std::visit(visitor, fromFlatIndex(idx));
  }

private:
  static const constexpr size_t HEADER_HEIGHT = 30;
};
}; // namespace vicinae::ui
