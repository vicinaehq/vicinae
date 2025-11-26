#pragma once
#include "layout.hpp"
#include "service-registry.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "ui/default-list-item-widget/default-list-item-widget.hpp"
#include "ui/list-section-header.hpp"
#include "ui/transform-result/transform-result.hpp"
#include "ui/views/base-view.hpp"
#include "ui/vlist/vlist.hpp"
#include <catch2/internal/catch_result_type.hpp>
#include <qlocale.h>
#include <stdexcept>
#include <variant>

template <typename ItemType, typename SectionId> class SectionListModel : public vicinae::ui::VListModel {
public:
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

  std::variant<SectionItem, SectionHeader> fromFlatIndex(Index idx) const {
    // TODO: optimize, that's obviously slow
    int currentIndex = 0;

    for (int i = 0; i != sectionCount(); ++i) {
      auto id = sectionIdFromIndex(i);
      int itemCount = sectionItemCount(id);
      bool withHeader = itemCount > 0 && !sectionName(id).empty();

      if (withHeader) {
        if (currentIndex == idx) return SectionHeader{.name = sectionName(id)};
        ++currentIndex;
      }

      for (int j = 0; j != itemCount; ++j) {
        if (currentIndex == idx) {
          return SectionItem{.data = sectionItemAt(id, j), .sectionIdx = i, .itemIdx = j};
        }
        ++currentIndex;
      }
    }

    qDebug() << "idx" << idx;

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

      for (int j = 0; j != itemCount; ++j) {
        if (currentIndex == idx) { return height; }
        height += itemHeight;
        ++currentIndex;
      }
    }

    return height;
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

      for (int j = 0; j != itemCount; ++j) {
        if (targetHeight < height + itemHeight) { return currentIndex; }
        height += itemHeight;
        ++currentIndex;
      }
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
  static const constexpr size_t HEADER_HEIGHT = 40;
};

enum class SectionType { Calculator, Results, Files };

using RootItemVariant = std::variant<AbstractCalculatorBackend::CalculatorResult, const RootItem *>;

class RootSearchModel : public SectionListModel<RootItemVariant, SectionType> {
public:
  std::array<SectionType, 3> m_sections = {SectionType::Calculator, SectionType::Results, SectionType::Files};
  RootSearchModel(RootItemManager *manager) : m_manager(manager) {}

  void setFilter(const QString &text) {
    m_items = m_manager->search(text);
    qDebug() << "items" << m_items.size() << "for" << text;
    emit dataChanged();
  }

  int sectionCount() const override { return m_sections.size(); }

  int sectionItemHeight(SectionType id) const override {
    switch (id) {
    case SectionType::Calculator:
      return CALCULATOR_HEIGHT;
    default:
      return ITEM_HEIGHT;
    }
  }

  SectionType sectionIdFromIndex(int idx) const override { return m_sections[idx]; }

  int sectionItemCount(SectionType id) const override {
    switch (id) {
    case SectionType::Calculator:
      return m_calc.has_value() * 1;
    case SectionType::Results:
      return m_items.size();
    default:
      return 0;
    }
  }

  std::string_view sectionName(SectionType id) const override {
    static std::string resultName;

    switch (id) {
    case SectionType::Calculator:
      return "Calculator";
    case SectionType::Results: {
      resultName = std::format("Results ({})", m_items.size());
      return resultName;
    }
    case SectionType::Files:
      return "Files";
    }
    return "";
  }

  RootItemVariant sectionItemAt(SectionType id, int itemIdx) const override {
    switch (id) {
    case SectionType::Calculator:
      return m_calc.value();
    case SectionType::Results:
      return m_items[itemIdx].item.get().get();
      // FIXME: implement
    case SectionType::Files:
      return {};
    }

    return {};
  }

  StableID stableId(const RootItemVariant &item) const override {
    static std::hash<QString> hasher = {};
    const auto visitor =
        overloads{[](const AbstractCalculatorBackend::CalculatorResult &) { return hasher("calculator"); },
                  [](const RootItem *item) { return hasher(item->uniqueId()); }};
    return std::visit(visitor, item);
  }

  WidgetTag widgetTag(const RootItemVariant &item) const override { return item.index(); }

protected:
  WidgetType *createItemWidget(const RootItemVariant &type) const override {
    const auto visitor =
        overloads{[](const AbstractCalculatorBackend::CalculatorResult &) -> WidgetType * {
                    return new TransformResult;
                  },
                  [](const RootItem *) -> WidgetType * { return new DefaultListItemWidget; }};
    return std::visit(visitor, type);
  }

  void refreshItemWidget(const RootItemVariant &type, WidgetType *widget) const override {
    const auto visitor = overloads{[&](const AbstractCalculatorBackend::CalculatorResult &calc) {
                                     auto w = static_cast<TransformResult *>(widget);
                                     w->setBase(calc.question.text, "Expression");
                                     w->setResult(calc.answer.text, "Answer");
                                   },
                                   [&](const RootItem *item) {
                                     auto w = static_cast<DefaultListItemWidget *>(widget);
                                     w->setName(item->displayName());
                                     w->setIconUrl(item->iconUrl());
                                     w->setSubtitle(item->subtitle());
                                     w->setAccessories(item->accessories());
                                     w->setActive(item->isActive());
                                   }};

    std::visit(visitor, type);
  }

private:
  static constexpr const size_t ITEM_HEIGHT = 40;
  static constexpr const size_t CALCULATOR_HEIGHT = 80;
  std::span<RootItemManager::ScoredItem> m_items;
  std::optional<AbstractCalculatorBackend::CalculatorResult> m_calc;
  RootItemManager *m_manager;
};

class RootSearchView2 : public BaseView {
public:
  RootSearchView2() { VStack().add(m_list).imbue(this); }

  void initialize() override {
    auto manager = context()->services->rootItemManager();
    m_model = new RootSearchModel(manager);
    m_list->setModel(m_model);
    m_model->setFilter("");
  }

  void textChanged(const QString &text) override {
    m_model->setFilter(text);
    m_list->selectFirst();
  }

private:
  RootSearchModel *m_model = nullptr;
  vicinae::ui::VListWidget *m_list = new vicinae::ui::VListWidget;
};
