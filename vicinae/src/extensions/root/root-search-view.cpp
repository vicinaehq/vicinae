#include "root-search-view.hpp"
#include "misc/file-list-item.hpp"
#include "ui/views/base-view.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "service-registry.hpp"
#include "services/config/config-service.hpp"
#include "services/files-service/file-service.hpp"
#include "navigation-controller.hpp"
#include "services/app-service/app-service.hpp"
#include "actions/calculator/calculator-actions.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "service-registry.hpp"
#include "ui/calculator-list-item-widget.hpp"
#include "ui/omni-list/omni-list-item-widget.hpp"
#include "ui/omni-list/omni-list.hpp"
#include <QtConcurrent/QtConcurrent>
#include <chrono>
#include "utils/utils.hpp"
#include <memory>
#include <qbrush.h>
#include <qcoreevent.h>
#include <qdebug.h>
#include <qevent.h>
#include <qfuture.h>
#include <qfuturewatcher.h>
#include <qhash.h>
#include <qlabel.h>
#include <qlist.h>
#include <qlistwidget.h>
#include <qlogging.h>
#include <qmap.h>
#include <qmimedatabase.h>
#include <qnamespace.h>
#include <qnetworkcookiejar.h>
#include <qthreadpool.h>
#include <quuid.h>
#include <qwidget.h>
#include "ui/views/list-view.hpp"

/**
 * Common interface used by all root list items, whether they are attached to an actual root item or not.
 * For instance, the calculator item or the file results have no root item attached, they are generated
 * on the fly, from the search text directly.
 */
class AbstractRootListItem {
public:
  /**
   * Return the root item attached to the list item, if any.
   */
  virtual const RootItem *asRootItem() const = 0;
};

class RootFileListItem : public FileListItemBase, public AbstractRootListItem {
public:
  ItemData data() const override {
    return {.iconUrl = getIcon(), .name = m_path.filename().c_str(), .subtitle = compressPath(m_path)};
  }

  const RootItem *asRootItem() const override { return nullptr; }

  RootFileListItem(const std::filesystem::path &path) : FileListItemBase(path) {}
};

class RootSearchItem : public AbstractDefaultListItem,
                       public ListView::Actionnable,
                       public AbstractRootListItem {
protected:
  std::shared_ptr<RootItem> m_item;

  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx) const override {
    auto manager = ctx->services->rootItemManager();
    auto metadata = manager->itemMetadata(m_item->uniqueId());

    return m_item->newActionPanel(ctx, metadata);
  }

  ItemData data() const override {
    auto manager = ServiceRegistry::instance()->rootItemManager();
    auto metadata = manager->itemMetadata(m_item->uniqueId());
    auto accessories = m_item->accessories();

    return {
        .iconUrl = m_item->iconUrl(),
        .name = m_item->displayName(),
        .subtitle = m_item->subtitle(),
        .accessories = m_item->accessories(),
        .alias = QString::fromStdString(metadata.alias),
        .active = m_item->isActive(),
    };
  }

  std::unique_ptr<CompleterData> createCompleter() const override {
    return std::make_unique<CompleterData>(CompleterData{
        .iconUrl = m_item->iconUrl(),
        .arguments = m_item->arguments(),
    });
  }

  QString generateId() const override { return m_item->uniqueId(); }

public:
  const RootItem &item() const { return *m_item.get(); }
  const RootItem *asRootItem() const override { return m_item.get(); }

  RootSearchItem(const std::shared_ptr<RootItem> &item) : m_item(item) {}
};

class SuggestionRootSearchItem : public RootSearchItem {
  QString generateId() const override { return QString("suggestion.%1").arg(m_item->uniqueId()); }

public:
  SuggestionRootSearchItem(const std::shared_ptr<RootItem> &item) : RootSearchItem(item) {}
};

class FavoriteRootSearchItem : public RootSearchItem {
  QString generateId() const override { return QString("favorite.%1").arg(m_item->uniqueId()); }

public:
  FavoriteRootSearchItem(const std::shared_ptr<RootItem> &item) : RootSearchItem(item) {}
};

class FallbackRootSearchItem : public AbstractDefaultListItem,
                               public ListView::Actionnable,
                               public AbstractRootListItem {
  std::shared_ptr<RootItem> m_item;

  const RootItem *asRootItem() const override { return nullptr; }

  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx) const override {
    auto manager = ctx->services->rootItemManager();
    auto metadata = manager->itemMetadata(m_item->uniqueId());

    return m_item->fallbackActionPanel(ctx, metadata);
  }

  QString generateId() const override { return QString("fallback.%1").arg(m_item->uniqueId()); }

  ItemData data() const override {
    return {
        .iconUrl = m_item->iconUrl(),
        .name = m_item->displayName(),
        .subtitle = m_item->subtitle(),
        .accessories = m_item->accessories(),
    };
  }

public:
  FallbackRootSearchItem(const std::shared_ptr<RootItem> &item) : m_item(item) {}
};

class BaseCalculatorListItem : public OmniList::AbstractVirtualItem,
                               public ListView::Actionnable,
                               public AbstractRootListItem {
protected:
  const AbstractCalculatorBackend::CalculatorResult item;

  OmniListItemWidget *createWidget() const override {
    return new CalculatorListItemWidget(CalculatorItem{.expression = item.question, .result = item.answer});
  }

  int calculateHeight(int width) const override {
    static CalculatorListItemWidget ruler({});

    return ruler.sizeHint().height();
  }

  const RootItem *asRootItem() const override { return nullptr; }

  QString generateId() const override { return item.question; }

  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx) const override {
    auto panel = std::make_unique<ActionPanelState>();
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
  }

public:
  BaseCalculatorListItem(const AbstractCalculatorBackend::CalculatorResult &item) : item(item) {}
};

void RootSearchView::handleFileResults() {
  if (!m_pendingFileSearchResults.isFinished()) return;

  auto results = m_pendingFileSearchResults.result();

  if (m_lastFileSearchQuery != m_searchText) return;
  m_fileResults = results;
  render(m_searchText);
}

void RootSearchView::handleFileSearchTimeout() {
  auto filesService = ServiceRegistry::instance()->fileService();
  if (m_searchText.size() >= 3) {
    QString currentQuery = m_searchText;

    m_pendingFileSearchResults.setFuture(
        filesService->indexer()->queryAsync(currentQuery.toStdString(), {.pagination = {.limit = 8}}));
    m_lastFileSearchQuery = currentQuery;
  }
}

bool RootSearchView::inputFilter(QKeyEvent *event) {
  // activate current selection if its alias begins with the search text
  if (event->key() == Qt::Key_Space) {
    if (searchText().isEmpty()) return true;

    if (auto item = dynamic_cast<const AbstractRootListItem *>(m_list->selected())) {
      if (auto rootItem = item->asRootItem()) {
        if (rootItem->supportsAliasSpaceShortcut()) {
          auto manager = context()->services->rootItemManager();
          auto metadata = manager->itemMetadata(rootItem->uniqueId());

          if (metadata.alias.starts_with(searchText().toStdString())) {
            m_list->activateCurrentSelection();
            return true;
          }
        }
      }
    }
  }

  return ListView::inputFilter(event);
}

void RootSearchView::textChanged(const QString &text) {
  m_searchText = text;
  QString query = text.trimmed();

  if (m_pendingCalculation.isRunning()) { m_pendingCalculation.cancel(); }
  if (m_pendingFileSearchResults.isRunning()) { m_pendingFileSearchResults.cancel(); }
  if (query.isEmpty()) return renderEmpty();

  m_calcDebounce->start();

  if (context()->services->config()->value().rootSearch.searchFiles) { m_fileSearchDebounce->start(); }

  if (text.size() < 3) { m_fileResults.clear(); }

  return render(text);
}

void RootSearchView::handleFavoriteChanged(const QString &itemId, bool value) {
  if (isVisible()) textChanged(searchText());
}

void RootSearchView::handleItemChange() {
  if (isVisible()) textChanged(searchText());
}

void RootSearchView::initialize() {
  auto manager = context()->services->rootItemManager();
  auto wm = context()->services->windowManager();

  m_calcDebounce->setInterval(100);
  m_calcDebounce->setSingleShot(true);
  m_fileSearchDebounce->setInterval(100);
  m_fileSearchDebounce->setSingleShot(true);

  setSearchPlaceholderText("Search for anything...");
  textChanged(searchText());

  connect(wm, &WindowManager::windowsChanged, this, [this]() {
    m_list->refresh();
    forceReselection();
  });
  connect(manager, &RootItemManager::itemsChanged, this, &RootSearchView::handleItemChange);
  connect(manager, &RootItemManager::itemFavoriteChanged, this, &RootSearchView::handleFavoriteChanged);
  connect(m_calcDebounce, &QTimer::timeout, this, &RootSearchView::handleCalculatorTimeout);
  connect(m_fileSearchDebounce, &QTimer::timeout, this, &RootSearchView::handleFileSearchTimeout);
  connect(&m_pendingFileSearchResults, &FileSearchWatcher::finished, this,
          &RootSearchView::handleFileResults);
  connect(&m_pendingCalculation, &CalculatorWatcher::finished, this,
          &RootSearchView::handleCalculationResult);
}

void RootSearchView::handleCalculatorTimeout() {
  auto calculator = context()->services->calculatorService();
  QString expression = searchText().trimmed();
  bool isComputable = false;

  if (expression.isEmpty()) return;

  for (const auto &ch : expression) {
    if (!ch.isLetterOrNumber() || ch.isSpace()) {
      isComputable = true;
      break;
    }
  }

  if (!isComputable || !calculator->backend()) {
    m_currentCalculatorEntry.reset();
    render(searchText());
    return;
  }

  m_pendingCalculation.setFuture(calculator->backend()->asyncCompute(expression));
}

void RootSearchView::handleCalculationResult() {
  if (m_pendingCalculation.isCanceled()) return;

  auto result = m_pendingCalculation.result();

  if (result) {
    m_currentCalculatorEntry = *result;
  } else {
    m_currentCalculatorEntry.reset();
  }
  render(searchText());
}

void RootSearchView::render(const QString &text) {
  auto rootItemManager = ServiceRegistry::instance()->rootItemManager();
  auto appDb = ServiceRegistry::instance()->appDb();
  const auto &appEntries = appDb->list();

  m_list->beginResetModel();

  auto start = std::chrono::high_resolution_clock::now();

  if (m_currentCalculatorEntry) {
    m_list->addSection("Calculator")
        .addItem(std::make_unique<BaseCalculatorListItem>(*m_currentCalculatorEntry));
  }

  auto &results = m_list->addSection("Results");

  auto searchResults = rootItemManager->search(text.trimmed());

  for (const auto &item : searchResults) {
    results.addItem(std::make_unique<RootSearchItem>(item.item.get()));
  }

  if (!m_fileResults.empty()) {
    auto &section = m_list->addSection("Files");

    for (const auto &file : m_fileResults) {
      section.addItem(std::make_unique<RootFileListItem>(file.path));
    }
  }

  auto &fallbackSection = m_list->addSection(QString("Use \"%1\" with...").arg(text));

  for (const auto &fallback : rootItemManager->fallbackItems()) {
    fallbackSection.addItem(std::make_unique<FallbackRootSearchItem>(fallback));
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  m_list->endResetModel(OmniList::SelectFirst);
  // qDebug() << "root searched in " << duration << "ms";
}

void RootSearchView::renderEmpty() {
  m_fileResults.clear();
  m_currentCalculatorEntry.reset();

  m_list->beginResetModel();
  auto appDb = ServiceRegistry::instance()->appDb();
  const auto &appEntries = appDb->list();
  auto rootManager = ServiceRegistry::instance()->rootItemManager();

  {
    auto &favorites = m_list->addSection("Favorites");

    for (const auto &item : rootManager->queryFavorites()) {
      favorites.addItem(std::make_unique<FavoriteRootSearchItem>(item.item));
    }
  }

  {
    auto &suggestions = m_list->addSection("Suggestions");

    for (const auto &item : rootManager->querySuggestions()) {
      suggestions.addItem(std::make_unique<SuggestionRootSearchItem>(item.item));
    }
  }

  auto isEnabled = [&](const std::shared_ptr<RootItem> &item) {
    return rootManager->itemMetadata(item->uniqueId()).isEnabled;
  };

  if (auto provider = rootManager->provider("shortcuts")) {
    auto &section = m_list->addSection("Shortcuts");

    auto items =
        provider->loadItems() | std::views::filter(isEnabled) |
        std::views::transform([](const auto &item) { return std::make_unique<RootSearchItem>(item); });

    for (auto item : items)
      section.addItem(std::move(item));
  }

  auto commandItems =
      rootManager->providers() | std::views::filter([](const auto &provider) {
        return provider->type() == RootProvider::Type::ExtensionProvider;
      }) |
      std::views::transform([](const auto &provider) { return provider->loadItems(); }) | std::views::join |
      std::views::filter(isEnabled) |
      std::views::transform([](const auto &item) { return std::make_unique<RootSearchItem>(item); });

  auto &section = m_list->addSection("Commands");

  for (auto item : commandItems) {
    section.addItem(std::move(item));
  }

  if (auto provider = rootManager->provider("apps")) {
    auto &section = m_list->addSection("Apps");
    auto items =
        provider->loadItems() | std::views::filter(isEnabled) |
        std::views::transform([](const auto &item) { return std::make_unique<RootSearchItem>(item); });

    for (auto item : items)
      section.addItem(std::move(item));
  }

  m_list->endResetModel(OmniList::SelectFirst);
}
