#include "root-search-model.hpp"
#include "config/config.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "services/calculator-service/calculator-service.hpp"
#include "services/files-service/file-service.hpp"
#include "services/news/news-service.hpp"
#include "theme.hpp"
#include <filesystem>
#include <utility>

RootSearchModel::RootSearchModel(const ViewScope &scope, QObject *parent)
    : SectionListModel(parent), m_manager(scope.services()->rootItemManager()),
      m_appDb(scope.services()->appDb()), m_newsService(scope.services()->newsService()),
      m_calculator(scope.services()->calculatorService()), m_fileService(scope.services()->fileService()),
      m_config(scope.services()->config()), m_fileSearchEnabled(m_config->value().searchFilesInRoot) {

  setScope(scope);

  using namespace std::chrono_literals;

  m_calculatorDebounce.setInterval(200ms);
  m_calculatorDebounce.setSingleShot(true);
  m_fileSearchDebounce.setInterval(200ms);
  m_fileSearchDebounce.setSingleShot(true);

  connect(&m_calculatorDebounce, &QTimer::timeout, this, &RootSearchModel::startCalculator);
  connect(&m_fileSearchDebounce, &QTimer::timeout, this, &RootSearchModel::startFileSearch);
  connect(&m_calcWatcher, &CalculatorWatcher::finished, this, &RootSearchModel::handleCalculatorFinished);
  connect(&m_fileWatcher, &FileSearchWatcher::finished, this, &RootSearchModel::handleFileSearchFinished);

  connect(m_config, &config::Manager::configChanged, this,
          [this](const config::ConfigValue &next, const config::ConfigValue &) {
            m_fileSearchEnabled = next.searchFilesInRoot;
          });

  connect(m_manager, &RootItemManager::metadataChanged, this, &RootSearchModel::refresh);
  connect(m_manager, &RootItemManager::itemsChanged, this, &RootSearchModel::refresh);
  connect(m_newsService, &NewsService::itemsChanged, this, &RootSearchModel::refresh);

  connect(&ThemeService::instance(), &ThemeService::themeChanged, this, [this]() {
    if (rowCount() > 0) emit dataChanged(index(0), index(rowCount() - 1), {IconSource, AccessoryColor});
  });

  m_linkSource = new RootLinkSection;
  m_calcSource = new RootCalculatorSection;
  m_newsSource = new RootNewsSection;
  m_favoritesSource = new RootFavoritesSection(m_manager);
  m_resultsSource = new RootResultsSection(m_manager);
  m_filesSource = new RootFilesSection(m_appDb);
  m_fallbackSource = new RootFallbackSection(m_manager);

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
  m_filesSource->setFiles({});

  m_calculatorDebounce.stop();
  m_fileSearchDebounce.stop();

  bool const directMatch = rerunSearch();

  if (!text.isEmpty() && !directMatch) {
    m_calculatorDebounce.start();
    m_fileSearchDebounce.start();
  }
}

void RootSearchModel::refresh() {
  auto saved = selectFirstOnReset();
  setSelectFirstOnReset(false);
  rerunSearch();
  setSelectFirstOnReset(saved);
}

bool RootSearchModel::rerunSearch() {
  auto text = QString::fromStdString(m_query);

  if (!text.isEmpty() && text.startsWith('/')) {
    std::error_code ec;
    if (std::filesystem::exists(m_query, ec)) {
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
    m_newsSource->setItems(m_newsService->activeItems());
    m_favoritesSource->setItems(m_manager->queryFavorites());
    m_fallbackSource->setItems({});
  } else {
    m_manager->search(text, scored);
    m_newsSource->setItems({});
    m_favoritesSource->setItems({});
    m_fallbackSource->setItems(m_manager->fallbackItems());
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

  rebuild();
  return false;
}

void RootSearchModel::setSelectedIndex(int index) {
  QString oldId = m_lastCompleterItemId;
  SectionListModel::setSelectedIndex(index);

  int sourceIdx = -1;
  int itemIdx = -1;
  if (!dataItemAt(index, sourceIdx, itemIdx)) {
    if (!oldId.isEmpty()) scope().destroyCurrentCompletion();
    m_lastCompleterItemId.clear();
    emit primaryActionChanged();
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

  emit primaryActionChanged();
}

bool RootSearchModel::tryAliasFastTrack() {
  int sourceIdx = -1;
  int itemIdx = -1;
  if (!dataItemAt(selectedIndex(), sourceIdx, itemIdx)) return false;

  auto *src = sources()[sourceIdx];
  const RootItem *item = nullptr;

  if (src == m_resultsSource) {
    item = m_resultsSource->rootItem(itemIdx);
  } else if (src == m_favoritesSource) {
    item = m_favoritesSource->rootItem(itemIdx);
  }

  if (!item || !item->supportsAliasSpaceShortcut()) return false;
  auto meta = m_manager->itemMetadata(item->uniqueId());
  if (!meta.alias || !meta.alias->starts_with(m_query)) return false;

  activateSelected();
  return true;
}

QString RootSearchModel::primaryActionTitle() const {
  auto *state = scope().topState();
  if (!state || !state->actionPanelState) return {};
  auto *action = state->actionPanelState->primaryAction();
  return action ? action->title() : QString();
}

QString RootSearchModel::primaryActionIcon() const {
  auto *state = scope().topState();
  if (!state || !state->actionPanelState) return {};
  auto *action = state->actionPanelState->primaryAction();
  if (!action) return {};
  auto icon = action->icon();
  return icon ? qml::imageSourceFor(*icon) : QString();
}

QVariantList RootSearchModel::primaryActionShortcutTokens() const {
  auto *state = scope().topState();
  if (!state || !state->actionPanelState) return {};
  auto *action = state->actionPanelState->primaryAction();
  if (!action) return {};
  auto shortcut = action->shortcut().value_or(Keyboard::Shortcut::enter());
  return shortcut.toDisplayTokens();
}

void RootSearchModel::startCalculator() {
  if (m_calcWatcher.isRunning()) {
    m_calculator->backend()->abort();
    m_calcWatcher.waitForFinished();
  }

  m_calculatorSearchQuery = m_query;

  if (!m_calculator->backend()) return;

  auto expression = QString::fromStdString(m_query);
  if (expression.startsWith("=") && expression.size() > 1) {
    m_calcWatcher.setFuture(m_calculator->backend()->asyncCompute(expression.mid(1)));
    return;
  }

  bool const containsNonAlnum =
      std::ranges::any_of(m_query, [](QChar ch) { return !ch.isLetterOrNumber(); }) ||
      m_query.starts_with("0x") || m_query.starts_with("0b") || m_query.starts_with("0o");
  const auto isAllowedLeadingChar = [&](QChar c) {
    return c == '-' || c == '(' || c == ')' || c.isLetterOrNumber() || c.category() == QChar::Symbol_Currency;
  };
  bool const isComputable =
      expression.size() > 1 && isAllowedLeadingChar(expression.at(0)) && containsNonAlnum;
  if (!isComputable) return;

  m_calcWatcher.setFuture(m_calculator->backend()->asyncCompute(expression));
}

void RootSearchModel::handleCalculatorFinished() {
  if (!m_calcWatcher.isFinished() || m_calculatorSearchQuery != m_query) return;
  auto res = m_calcWatcher.result();
  if (!res) return;

  m_calcSource->setResult(res.value());

  auto saved = selectFirstOnReset();
  setSelectFirstOnReset(false);
  rebuild();
  setSelectFirstOnReset(saved);
}

void RootSearchModel::startFileSearch() {
  if (!m_fileSearchEnabled || m_query.size() < MIN_FS_TEXT_LENGTH) return;
  if (m_fileWatcher.isRunning()) { m_fileWatcher.cancel(); }
  m_fileSearchQuery = m_query;
  m_fileWatcher.setFuture(m_fileService->queryAsync(m_query));
}

void RootSearchModel::handleFileSearchFinished() {
  if (!m_fileWatcher.isFinished() || m_fileSearchQuery != m_query) return;
  m_filesSource->setFiles(m_fileWatcher.result());
  m_fileSearchQuery.clear();

  auto saved = selectFirstOnReset();
  setSelectFirstOnReset(false);
  rebuild();
  setSelectFirstOnReset(saved);
}
