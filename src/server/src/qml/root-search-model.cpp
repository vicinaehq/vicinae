#include "root-search-model.hpp"
#include "config/config.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "services/calculator-service/calculator-service.hpp"
#include "services/files-service/file-service.hpp"
#include "services/news/news-service.hpp"
#include "theme.hpp"
#include <filesystem>
#include <utility>

constexpr auto CALCULATOR_MIN_CHARS = 3;

RootSearchModel::RootSearchModel(const ViewScope &scope, QObject *parent)
    : SectionListModel(parent), m_manager(scope.services()->rootItemManager()),
      m_appDb(scope.services()->appDb()), m_newsService(scope.services()->newsService()),
      m_updateService(scope.services()->updateService()), m_calculator(scope.services()->calculatorService()),
      m_fileService(scope.services()->fileService()), m_config(scope.services()->config()),
      m_fileSearchEnabled(m_config->value().searchFilesInRoot) {

  setScope(scope);

  using namespace std::chrono_literals;

  m_fileSearchDebounce.setInterval(50ms);
  m_fileSearchDebounce.setSingleShot(true);

  connect(&m_fileSearchDebounce, &QTimer::timeout, this, &RootSearchModel::startFileSearch);
  connect(&m_fileWatcher, &FileSearchWatcher::finished, this, &RootSearchModel::handleFileSearchFinished);

  connect(m_config, &config::Manager::configChanged, this,
          [this](const config::ConfigValue &next, const config::ConfigValue &) {
            m_fileSearchEnabled = next.searchFilesInRoot;
          });

  connect(m_manager, &RootItemManager::metadataChanged, this, &RootSearchModel::refresh);
  connect(m_manager, &RootItemManager::itemsChanged, this, &RootSearchModel::refresh);
  connect(m_newsService, &NewsService::itemsChanged, this, &RootSearchModel::refresh);
  connect(m_updateService, &UpdateService::updateChanged, this, &RootSearchModel::refresh);

  connect(&ThemeService::instance(), &ThemeService::themeChanged, this, [this]() {
    if (rowCount() > 0) emit dataChanged(index(0), index(rowCount() - 1), {IconSource, AccessoryColor});
  });

  m_updateSource = new RootUpdateSection(m_updateService);
  m_linkSource = new RootLinkSection;
  m_calcSource = new RootCalculatorSection;
  m_newsSource = new RootNewsSection;
  m_favoritesSource = new RootFavoritesSection(m_manager);
  m_resultsSource = new RootResultsSection(m_manager);
  m_filesSource = new RootFilesSection(m_appDb);
  m_fallbackSource = new RootFallbackSection(m_manager);

  addSource(m_updateSource);
  addSource(m_linkSource);
  addSource(m_calcSource);
  addSource(m_newsSource);
  addSource(m_favoritesSource);
  addSource(m_resultsSource);
  addSource(m_filesSource);
  addSource(m_fallbackSource);

  setFilter({});
}

void RootSearchModel::setFilter(const QString &text) {
  auto query = text.toStdString();
  if (query == m_query) return;
  m_query = std::move(query);
  setSelectFirstOnReset(true);
  scope().clearActions();

  m_calcSource->setResult({});
  if (!fileSearchApplicable()) {
    m_filesSource->setFiles({});
    m_hasFileResults = false;
    scope().setLoading(false);
  }
  m_fileSearchDebounce.stop();

  bool const directMatch = rerunSearch();
}

void RootSearchModel::refresh() {
  auto saved = selectFirstOnReset();
  setSelectFirstOnReset(false);
  rerunSearch();
  setSelectFirstOnReset(saved);
  refreshActionPanel();
}

bool RootSearchModel::rerunSearch() {
  auto text = QString::fromStdString(m_query);

  if (!text.isEmpty() && text.startsWith('/')) {
    std::error_code ec;
    if (std::filesystem::exists(m_query, ec)) {
      m_updateSource->setUpdate({});
      m_linkSource->setLink({});
      m_resultsSource->setItems({});
      m_resultsSource->setQueryEmpty(false);
      m_filesSource->setFiles({{std::filesystem::path(m_query), 1.0}});
      m_newsSource->setItems({});
      m_favoritesSource->setItems({});
      m_fallbackSource->setItems({});
      rebuild();
      return true;
    }
  }

  if (!text.isEmpty()) {
    if (auto url = QUrl(text); url.isValid() && !url.scheme().isEmpty()) {
      if (auto app = m_appDb->findDefaultOpener(text)) {
        m_updateSource->setUpdate({});
        m_linkSource->setLink(LinkItem{.app = app, .url = text});
        m_resultsSource->setItems({});
        m_resultsSource->setQueryEmpty(false);
        m_newsSource->setItems({});
        m_favoritesSource->setItems({});
        rebuild();
        return true;
      }
    }
  }

  m_linkSource->setLink({});
  m_resultsSource->setQueryEmpty(m_query.empty());
  m_fallbackSource->setQuery(m_query);

  std::vector<RootItemManager::ScoredItem> scored;
  if (m_query.empty()) {
    m_manager->search("", scored, {.includeFavorites = false, .prioritizeAliased = false});
    m_updateSource->setUpdate(m_updateService->available());
    m_newsSource->setItems(m_newsService->activeItems());
    m_favoritesSource->setItems(m_manager->queryFavorites());
    m_fallbackSource->setItems({});
  } else {
    m_manager->search(text, scored);
    m_updateSource->setUpdate({});
    m_newsSource->setItems({});
    m_favoritesSource->setItems({});

    bool const awaitingFiles = fileSearchApplicable() && !m_hasFileResults;
    m_fallbackSource->setItems(awaitingFiles ? std::vector<std::shared_ptr<RootItem>>{}
                                             : m_manager->fallbackItems());
  }

  std::vector<OwnedResult> results;
  results.reserve(scored.size());
  for (const auto &s : scored) {
    results.push_back({
        .item = s.item.get(),
        .meta = s.meta ? *s.meta : RootItemMetadata{},
    });
  }

  m_resultsSource->setItems(std::move(results));

  refreshCalculator();
  if (!text.isEmpty()) m_fileSearchDebounce.start();

  rebuild();
  return false;
}

void RootSearchModel::refreshCalculator() {
  m_calcSource->setResult({});

  if (m_query.empty()) return;

  if (m_query.starts_with('=')) {
    if (auto res = m_calculator->backend()->compute(QString::fromStdString(m_query.substr(1)), {})) {
      m_calcSource->setResult(res.value());
    }
    return;
  }

  if (m_resultsSource->count() > 0 || m_query.size() < CALCULATOR_MIN_CHARS) return;

  if (auto res = m_calculator->backend()->compute(QString::fromStdString(m_query), {})) {
    m_calcSource->setResult(res.value());
  }
}

void RootSearchModel::setSelectedIndex(int index) {
  QString oldId = m_lastCompleterItemId;
  SectionListModel::setSelectedIndex(index);

  int sourceIdx = -1;
  int itemIdx = -1;
  if (!dataItemAt(index, sourceIdx, itemIdx)) {
    if (!oldId.isEmpty()) scope().destroyCurrentCompletion();
    m_lastCompleterItemId.clear();

    return;
  }

  auto *src = sources()[sourceIdx];
  auto id = src->itemId(itemIdx);
  bool const sameItem = (!id.isEmpty() && id == oldId);
  m_lastCompleterItemId = id;

  if (!sameItem) {
    bool createdCompleter = false;

    auto tryCreateCompleter = [&](const RootItem *item) {
      if (!item) return;
      auto args = item->arguments();
      if (args.empty()) return;
      scope().createCompletion(args, item->iconUrl());
      createdCompleter = true;
    };

    if (src == m_resultsSource) {
      tryCreateCompleter(m_resultsSource->rootItem(itemIdx));
    } else if (src == m_favoritesSource) {
      tryCreateCompleter(m_favoritesSource->rootItem(itemIdx));
    }

    if (!createdCompleter) scope().destroyCurrentCompletion();
  }
}

const RootItem *RootSearchModel::selectedRootItem() const {
  int sourceIdx = -1;
  int itemIdx = -1;
  if (!dataItemAt(selectedIndex(), sourceIdx, itemIdx)) return nullptr;

  auto *section = dynamic_cast<const RootItemSection *>(sources()[sourceIdx]);
  return section ? section->rootItem(itemIdx) : nullptr;
}

bool RootSearchModel::fileSearchApplicable() const {
  return m_fileSearchEnabled && m_query.size() >= MIN_FS_TEXT_LENGTH;
}

void RootSearchModel::startFileSearch() {
  if (!fileSearchApplicable()) return;
  if (m_fileWatcher.isRunning()) { m_fileWatcher.cancel(); }
  m_fileSearchQuery = m_query;
  scope().setLoading(true);
  m_fileWatcher.setFuture(m_fileService->queryAsync(m_query));
}

void RootSearchModel::handleFileSearchFinished() {
  if (!m_fileWatcher.isFinished() || m_fileSearchQuery != m_query) return;
  scope().setLoading(false);
  m_filesSource->setFiles(m_fileWatcher.result());
  m_fileSearchQuery.clear();
  m_hasFileResults = true;
  m_fallbackSource->setItems(m_manager->fallbackItems());

  bool const atDefault = selectedIndex() < 0 || selectedIndex() == nextSelectableIndex(-1, 1);

  auto saved = selectFirstOnReset();
  setSelectFirstOnReset(false);
  rebuild();
  setSelectFirstOnReset(saved);

  if (atDefault) {
    setSelectedIndex(nextSelectableIndex(-1, 1));
  } else {
    refreshActionPanel();
  }
}
