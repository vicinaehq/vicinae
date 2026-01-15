#pragma once
#include "actions/app/app-actions.hpp"
#include "actions/calculator/calculator-actions.hpp"
#include "config/config.hpp"
#include "misc/file-list-item.hpp"
#include "root-search-model.hpp"
#include "root-search-controller.hpp"
#include "common.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "ui/views/typed-list-view.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "ui/search-bar/search-bar.hpp"
#include "ui/views/base-view.hpp"
#include <qfuturewatcher.h>
#include <qlocale.h>
#include <qnamespace.h>
#include <qobjectdefs.h>
#include <variant>

class RootSearchView : public TypedListView<RootSearchModel> {
public:
  QString initialSearchPlaceholderText() const override { return "Search for anything..."; }

  std::unique_ptr<ActionPanelState> createActionPanel(const RootItemVariant &item) const override {
    const auto visitor = overloads{
        [&](const RootSearchResult &item) {
          auto rootItem = item.scored->item.get();
          return rootItem->newActionPanel(context(), m_manager->itemMetadata(rootItem->uniqueId()));
        },
        [](const AbstractCalculatorBackend::CalculatorResult &item) -> std::unique_ptr<ActionPanelState> {
          auto panel = std::make_unique<ListActionPanelState>();
          auto copyAnswer = new CopyCalculatorAnswerAction(item);
          auto copyQA = new CopyCalculatorQuestionAndAnswerAction(item);
          auto putAnswerInSearchBar = new PutCalculatorAnswerInSearchBar(item);
          auto openHistory = new OpenCalculatorHistoryAction();
          auto main = panel->createSection();

          copyAnswer->setPrimary(true);
          main->addAction(copyAnswer);
          main->addAction(copyQA);
          main->addAction(putAnswerInSearchBar);
          main->addAction(openHistory);

          return panel;
        },
        [&](const FallbackItem &item) {
          return item.item->fallbackActionPanel(context(), m_manager->itemMetadata(item.item->uniqueId()));
        },
        [&](const std::filesystem::path &path) {
          return FileListItemBase::actionPanel(path, context()->services->appDb());
        },
        [&](const FavoriteItem &item) {
          return item.item->newActionPanel(context(), m_manager->itemMetadata(item.item->uniqueId()));
        },
        [&](const LinkItem &item) {
          auto panel = std::make_unique<ActionPanelState>();
          auto section = panel->createSection();
          auto open =
              new OpenAppAction(item.app, QString("Open in %1").arg(item.app->displayName()), {item.url});
          open->setClearSearch(true);
          section->addAction(open);
          return panel;
        }};

    return std::visit(visitor, item);
  }

  std::unique_ptr<CompleterData> createCompleter(const RootItemVariant &item) const override {
    auto rootItemCompleter = [](const RootItem *item) -> std::unique_ptr<CompleterData> {
      ArgumentList args = item->arguments();

      if (args.empty()) return nullptr;

      auto cmpl = std::make_unique<CompleterData>();
      cmpl->arguments = args;
      cmpl->iconUrl = item->iconUrl();

      return cmpl;
    };

    const auto visitor = overloads{
        [&](const RootSearchResult &result) { return rootItemCompleter(result.scored->item.get().get()); },
        [&](const FavoriteItem &item) { return rootItemCompleter(item.item); },
        [](auto &&a) { return std::unique_ptr<CompleterData>(); }};

    return std::visit(visitor, item);
  }

  bool tryAliasFastTrack() {
    auto selection = m_list->currentSelection();

    if (!selection) return false;

    if (m_model->shouldFastTrack(m_model->fromIndex(*selection).value())) {
      executePrimaryAction();
      return true;
    }

    return false;
  }

  bool inputFilter(QKeyEvent *event) override {
    if (event->modifiers().toInt() == 0) {
      if (event->key() == Qt::Key_Space) { return tryAliasFastTrack(); }
    }
    return TypedListView::inputFilter(event);
  }

  void initialize() override {
    TypedListView::initialize();

    auto config = context()->services->config();
    m_manager = context()->services->rootItemManager();
    m_model = new RootSearchModel(m_manager);
    setModel(m_model);

    m_controller =
        new RootSearchController(m_manager, context()->services->fileService(), context()->services->appDb(),
                                 context()->services->calculatorService(), m_model, this);

    m_list->setModel(m_model);
    m_controller->setFilter("");
    m_controller->setFileSearch(config->value().searchFilesInRoot);

    connect(config, &config::Manager::configChanged, this,
            [&](const config::ConfigValue &next, const config::ConfigValue &prev) {
              m_controller->setFileSearch(next.searchFilesInRoot);
            });
    connect(m_manager, &RootItemManager::subtitleChanged, this, [this]() { m_list->refreshAll(); });
  }

  void textChanged(const QString &text) override {
    m_controller->setFilter(text.simplified().toStdString());
    m_list->selectFirst();
  }

private:
  RootItemManager *m_manager = nullptr;
  RootSearchModel *m_model = nullptr;
  RootSearchController *m_controller = nullptr;
};
