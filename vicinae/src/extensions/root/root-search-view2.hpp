#pragma once
#include "layout.hpp"
#include "service-registry.hpp"
#include "services/app-service/abstract-app-db.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "services/calculator-service/calculator-service.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "services/app-service/app-service.hpp"
#include "ui/default-list-item-widget/default-list-item-widget.hpp"
#include "ui/transform-result/transform-result.hpp"
#include "ui/views/base-view.hpp"
#include "ui/vlist/vlist.hpp"
#include "ui/vlist/common/section-model.hpp"
#include "utils.hpp"
#include <qfuturewatcher.h>
#include <qlocale.h>
#include <qobjectdefs.h>
#include <variant>

// clang-format off
enum class SectionType { 
	Link, Calculator, Results, Files, Fallback,
	Favorites
};
// clang-format on

struct FallbackItem {
  const RootItem *item;
};

struct FavoriteItem {
  const RootItem *item;
};

struct LinkItem {
  std::shared_ptr<AbstractApplication> app;
  QString url;
};

using RootItemVariant = std::variant<AbstractCalculatorBackend::CalculatorResult, const RootItem *,
                                     std::filesystem::path, FallbackItem, FavoriteItem, LinkItem>;

class RootSearchModel : public vicinae::ui::SectionListModel<RootItemVariant, SectionType> {
public:
  RootSearchModel(RootItemManager *manager) : m_manager(manager) {
    m_files = {};
    regenerateFallback();
    connect(&m_calcWatcher, &QFutureWatcher<AbstractCalculatorBackend::ComputeResult>::finished, this,
            &RootSearchModel::handleCalculatorResult);
    connect(m_manager, &RootItemManager::fallbackDisabled, this, &RootSearchModel::regenerateFallback);
    connect(m_manager, &RootItemManager::fallbackEnabled, this, &RootSearchModel::regenerateFallback);
    connect(m_manager, &RootItemManager::itemsChanged, this, &RootSearchModel::reloadSearch);
    connect(m_manager, &RootItemManager::itemFavoriteChanged, this, &RootSearchModel::reloadSearch);
  }

  void reloadSearch() { setFilter(query); }

  void regenerateFallback() { m_fallbackItems = m_manager->fallbackItems(); }

  void handleCalculatorResult() {
    if (!m_calcWatcher.isFinished()) return;

    auto result = m_calcWatcher.result();

    if (result.has_value()) { m_calc = result.value(); }
    emit dataChanged();
  }

  std::span<const SectionType> sections() const {
    if (query.isEmpty()) return m_rootSections;
    return m_searchSections;
  }

  void setFilter(const QString &text) {
    auto calc = ServiceRegistry::instance()->calculatorService();
    auto appDb = ServiceRegistry::instance()->appDb();

    m_defaultOpener.reset();
    m_calc.reset();
    m_items = {};
    m_items = m_manager->search(text);

    if (text.isEmpty()) {
      m_favorites = m_manager->queryFavorites();
    } else {
      if (auto app = appDb->findDefaultOpener(text)) { m_defaultOpener = LinkItem{.app = app, .url = text}; }

      m_resultSectionTitle = std::format("Results ({})", m_items.size());
      m_fallbackSectionTitle = std::format("Use \"{}\" with...", text.toStdString());

      if (m_calcWatcher.isRunning()) m_calcWatcher.cancel();
      m_calcWatcher.setFuture(calc->backend()->asyncCompute(text));
    }

    query = text;
    emit dataChanged();
  }

  int sectionCount() const override { return sections().size(); }

  int sectionItemHeight(SectionType id) const override {
    switch (id) {
    case SectionType::Calculator:
      return CALCULATOR_HEIGHT;
    default:
      return ITEM_HEIGHT;
    }
  }

  SectionType sectionIdFromIndex(int idx) const override { return sections()[idx]; }

  int sectionItemCount(SectionType id) const override {
    switch (id) {
    case SectionType::Link:
      return m_defaultOpener.has_value();
    case SectionType::Calculator:
      return m_calc.has_value();
    case SectionType::Results:
      return m_items.size();
    case SectionType::Files:
      return m_files.size();
    case SectionType::Fallback:
      return m_fallbackItems.size();
    case SectionType::Favorites:
      return m_favorites.size();
    default:
      return 0;
    }
  }

  std::string_view sectionName(SectionType id) const override {
    switch (id) {
    case SectionType::Link:
      return "Link";
    case SectionType::Calculator:
      return "Calculator";
    case SectionType::Results:
      return query.isEmpty() ? std::string_view("Suggestions") : m_resultSectionTitle;
    case SectionType::Files:
      return "Files";
    case SectionType::Fallback:
      return m_fallbackSectionTitle;

    case SectionType::Favorites:
      return "Favorites";
    }
  }

  RootItemVariant sectionItemAt(SectionType id, int itemIdx) const override {
    switch (id) {
    case SectionType::Link:
      return m_defaultOpener.value();
    case SectionType::Calculator:
      return m_calc.value();
    case SectionType::Results:
      return m_items[itemIdx].item.get().get();
    case SectionType::Files:
      return m_files[itemIdx];
    case SectionType::Fallback:
      return FallbackItem{.item = m_fallbackItems[itemIdx].get()};
    case SectionType::Favorites:
      return FavoriteItem{.item = m_favorites[itemIdx].item.get()};
    }

    return {};
  }

  StableID stableId(const RootItemVariant &item) const override {
    static std::hash<QString> hasher = {};
    const auto visitor =
        overloads{[](const AbstractCalculatorBackend::CalculatorResult &) { return hasher("calculator"); },
                  [](const RootItem *item) { return hasher(item->uniqueId()); },
                  [](const LinkItem &item) { return hasher(item.url); },
                  [](const std::filesystem::path &path) { return hasher(path.c_str()); },
                  [](const FallbackItem &item) { return hasher(item.item->uniqueId() + ".fallback"); },
                  [](const FavoriteItem &item) { return hasher(item.item->uniqueId() + ".favorite"); }};
    return std::visit(visitor, item);
  }

  WidgetTag widgetTag(const RootItemVariant &item) const override { return item.index(); }

protected:
  WidgetType *createItemWidget(const RootItemVariant &type) const override {
    const auto visitor =
        overloads{[](const AbstractCalculatorBackend::CalculatorResult &) -> WidgetType * {
                    return new TransformResult;
                  },
                  [](const RootItem *) -> WidgetType * { return new DefaultListItemWidget; },
                  [](const std::filesystem::path &path) -> WidgetType * { return new DefaultListItemWidget; },
                  [](const LinkItem &item) -> WidgetType * { return new DefaultListItemWidget; },
                  [](const FallbackItem &item) -> WidgetType * { return new DefaultListItemWidget; },
                  [](const FavoriteItem &item) -> WidgetType * { return new DefaultListItemWidget; }};

    return std::visit(visitor, type);
  }

  void refreshItemWidget(const RootItemVariant &type, WidgetType *widget) const override {
    auto refreshRootItem = [&](const RootItem *item) {
      auto w = static_cast<DefaultListItemWidget *>(widget);
      w->setName(item->displayName());
      w->setIconUrl(item->iconUrl());
      w->setSubtitle(item->subtitle());
      w->setAccessories(item->accessories());
      w->setActive(item->isActive());
    };
    const auto visitor = overloads{[&](const AbstractCalculatorBackend::CalculatorResult &calc) {
                                     auto w = static_cast<TransformResult *>(widget);
                                     w->setBase(calc.question.text, "Expression");
                                     w->setResult(calc.answer.text, "Answer");
                                   },
                                   [&](const std::filesystem::path &path) {
                                     auto w = static_cast<DefaultListItemWidget *>(widget);
                                     w->setName(getLastPathComponent(path).c_str());
                                     w->setIconUrl(ImageURL::fileIcon(path));
                                     w->setSubtitle(path);
                                     w->setActive(false);
                                   },
                                   [&](const LinkItem &item) {
                                     auto w = static_cast<DefaultListItemWidget *>(widget);
                                     w->setName(item.url);
                                     w->setIconUrl(item.app->iconUrl());
                                     w->setActive(false);
                                   },
                                   refreshRootItem,
                                   [&](const FallbackItem &item) { refreshRootItem(item.item); },
                                   [&](const FavoriteItem &item) { refreshRootItem(item.item); }};

    std::visit(visitor, type);
  }

private:
  static constexpr const size_t ITEM_HEIGHT = 40;
  static constexpr const size_t CALCULATOR_HEIGHT = 80;

  static constexpr const std::array<SectionType, 2> m_rootSections = {SectionType::Favorites,
                                                                      SectionType::Results};
  static constexpr const std::array<SectionType, 6> m_searchSections = {
      SectionType::Link, SectionType::Calculator, SectionType::Results, SectionType::Files,
      SectionType::Fallback};

  QString query;
  std::span<RootItemManager::ScoredItem> m_items;
  std::optional<AbstractCalculatorBackend::CalculatorResult> m_calc;
  QFutureWatcher<AbstractCalculatorBackend::ComputeResult> m_calcWatcher;
  std::vector<std::filesystem::path> m_files;
  std::vector<std::shared_ptr<RootItem>> m_fallbackItems;

  // root
  std::vector<RootItemManager::SearchableRootItem> m_favorites;
  std::vector<RootItemManager::SearchableRootItem> m_suggestions;
  std::vector<std::shared_ptr<RootItem>> m_shortcuts;
  std::vector<std::shared_ptr<RootItem>> m_commands;
  std::vector<std::shared_ptr<RootItem>> m_apps;

  std::optional<LinkItem> m_defaultOpener;

  std::string m_fallbackSectionTitle;
  std::string m_resultSectionTitle;

  RootItemManager *m_manager;
};

class RootSearchView2 : public BaseView {
public:
  RootSearchView2() { VStack().add(m_list).imbue(this); }

  void handleSelection(std::optional<vicinae::ui::VListModel::Index> idx) {
    qDebug() << "selection changed";
    if (!idx) {
      qDebug() << "no index, clearing";
      clearActions();
      return;
    }

    const auto visitor =
        overloads{[this](const RootItem *item) {
                    setActions(item->newActionPanel(context(), m_manager->itemMetadata(item->uniqueId())));
                  },
                  [](auto &&a) {}};

    if (auto data = m_model->fromIndex(idx.value())) {
      qDebug() << "index" << idx;
      std::visit(visitor, data.value());
    } else {
      qDebug() << "no data";
    }
  }

  void initialize() override {
    m_manager = context()->services->rootItemManager();
    m_model = new RootSearchModel(m_manager);
    m_list->setModel(m_model);
    m_model->setFilter("");
    connect(m_list, &vicinae::ui::VListWidget::itemSelected, this, &RootSearchView2::handleSelection);
  }

  void textChanged(const QString &text) override {
    m_model->setFilter(text);
    m_list->selectFirst();
  }

private:
  RootItemManager *m_manager = nullptr;
  RootSearchModel *m_model = nullptr;
  vicinae::ui::VListWidget *m_list = new vicinae::ui::VListWidget;
};
