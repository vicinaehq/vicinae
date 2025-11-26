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
#include "vicinae.hpp"
#include <catch2/internal/catch_result_type.hpp>
#include <qlocale.h>
#include <variant>

class RootSearchModel : public vicinae::ui::VListModel {
public:
  RootSearchModel(RootItemManager *manager) : m_manager(manager) {}

  void setFilter(const QString &text) {
    m_items = m_manager->search(text);
    /*
m_calc = AbstractCalculatorBackend::CalculatorResult{.question =
                                                         {
                                                             .text = "1 + 1",
                                                         },
                                                     .answer = {.text = "2"}};
                                                                                                             */
    qDebug() << "items" << m_items.size() << "for" << text;
    emit dataChanged();
  }

  struct SectionHeader {
    const char *name;
  };

protected:
  WidgetType *createWidget(Index idx) const override {
    const auto visitor =
        overloads{[](const AbstractCalculatorBackend::CalculatorResult &) -> WidgetType * {
                    return new TransformResult;
                  },
                  [](const SectionHeader &) -> WidgetType * { return new OmniListSectionHeader("", "", 0); },
                  [](const RootItem *) -> WidgetType * { return new DefaultListItemWidget; }};

    return std::visit(visitor, fromFlatIndex(idx));
  }

  size_t calculatorHeightOffset() const {
    if (!m_calc) return 0;
    return CALCULATOR_HEIGHT + HEADER_HEIGHT;
  }

  void refreshWidget(Index idx, WidgetType *widget) const override {
    const auto visitor = overloads{[&](const AbstractCalculatorBackend::CalculatorResult &calc) {
                                     auto w = static_cast<TransformResult *>(widget);
                                     w->setBase(calc.question.text, "Expression");
                                     w->setResult(calc.answer.text, "Answer");
                                   },
                                   [&](const SectionHeader &header) {
                                     auto w = static_cast<OmniListSectionHeader *>(widget);
                                     w->setTitle(header.name);
                                   },
                                   [&](const RootItem *item) {
                                     auto w = static_cast<DefaultListItemWidget *>(widget);

                                     w->setName(item->displayName());
                                     w->setIconUrl(item->iconUrl());
                                     w->setSubtitle(item->subtitle());
                                     w->setAccessories(item->accessories());
                                     w->setActive(item->isActive());
                                   }};

    std::visit(visitor, fromFlatIndex(idx));
  }

  size_t count() const override { return m_items.size() + 1 + (m_calc.has_value() ? 2 : 0); }

  size_t height(Index idx) const override {
    const auto visitor = overloads{
        [](const AbstractCalculatorBackend::CalculatorResult &) { return CALCULATOR_HEIGHT; },
        [](const SectionHeader &) { return HEADER_HEIGHT; }, [](const RootItem *) { return ITEM_HEIGHT; }};

    return std::visit(visitor, fromFlatIndex(idx));
  }

  std::variant<AbstractCalculatorBackend::CalculatorResult, const RootItem *, SectionHeader>
  fromFlatIndex(Index idx) const {
    if (m_calc) {
      switch (idx) {
      case 0:
        return SectionHeader{.name = "Calculator"};
        break;
      case 1:
        return m_calc.value();
        break;
      case 2:
        return SectionHeader{.name = "Results"};
        break;
      default: {
        qDebug() << "idx" << idx << "items" << m_items.size();
        return m_items[idx - 3].item.get().get();
      }
      }
    }

    if (idx == 0) return SectionHeader{.name = "Results"};
    return m_items[idx - 1].item.get().get();
  }

  size_t height() const override {
    return m_items.size() * ITEM_HEIGHT + HEADER_HEIGHT + calculatorHeightOffset();
  }

  bool isSelectable(Index idx) const override {
    return !std::holds_alternative<SectionHeader>(fromFlatIndex(idx));
  }

  size_t heightAtIndex(Index idx) const override {
    int height = 0;
    if (m_calc && idx > 0) { height += HEADER_HEIGHT; }
    if (m_calc && idx > 1) { height += CALCULATOR_HEIGHT; }
    if (m_calc && idx > 2) { height += HEADER_HEIGHT; }
    if (!m_calc && idx > 0) { height += HEADER_HEIGHT; }

    if (m_calc) { height += std::max(0, (int)idx - 3) * ITEM_HEIGHT; }
    if (!m_calc) { height += std::max(0, (int)idx - 1) * ITEM_HEIGHT; }

    return height;
  }

  Index indexAtHeight(int height) const override {
    if (m_calc) {
      if (height < HEADER_HEIGHT) return 0;
      if (height < HEADER_HEIGHT + CALCULATOR_HEIGHT) return 1;
      if (height < HEADER_HEIGHT + CALCULATOR_HEIGHT + HEADER_HEIGHT) return 2;
      int min = height - HEADER_HEIGHT - HEADER_HEIGHT - CALCULATOR_HEIGHT;
      return ceil(min / ITEM_HEIGHT) + 3;
    }

    if (height < HEADER_HEIGHT) return 0;
    int min = height - HEADER_HEIGHT;
    return min / ITEM_HEIGHT + 1;
  }

  WidgetTag widgetTag(Index idx) const override { return fromFlatIndex(idx).index(); }

  StableID stableId(Index idx) const override {
    static std::hash<QString> hasher = {};
    const auto visitor =
        overloads{[&](const AbstractCalculatorBackend::CalculatorResult &) { return hasher("calculator"); },
                  [&](const SectionHeader &header) { return hasher(header.name); },
                  [&](const RootItem *item) { return hasher(item->uniqueId()); }};

    return std::visit(visitor, fromFlatIndex(idx));
  }

private:
  static constexpr const size_t ITEM_HEIGHT = 40;
  static constexpr const size_t HEADER_HEIGHT = 40;
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
