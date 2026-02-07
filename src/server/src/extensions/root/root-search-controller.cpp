#include "root-search-controller.hpp"
#include "root-search-model.hpp"
#include "service-registry.hpp"

RootSearchController::RootSearchController(RootItemManager *manager, FileService *fs, AppService *appDb,
                                           CalculatorService *calculator, RootSearchModel *model,
                                           QObject *parent)
    : QObject(parent), m_model(model), m_manager(manager), m_fs(fs), m_appDb(appDb),
      m_calculator(calculator) {
  using namespace std::chrono_literals;

  m_fileSearchDebounce.setInterval(200ms);
  m_fileSearchDebounce.setSingleShot(true);
  m_calculatorDebounce.setInterval(200ms);
  m_calculatorDebounce.setSingleShot(true);

  connect(&m_fileSearchDebounce, &QTimer::timeout, this, &RootSearchController::startFileSearch);
  connect(&m_calculatorDebounce, &QTimer::timeout, this, &RootSearchController::startCalculator);
  connect(&m_calcWatcher, &CalculatorWatcher::finished, this,
          &RootSearchController::handleCalculatorFinished);
  connect(&m_fileWatcher, &FileSearchWatcher::finished, this,
          &RootSearchController::handleFileSearchFinished);

  connect(m_manager, &RootItemManager::metadataChanged, this, [this]() {
    regenerateFallback();
    regenerateFavorites();
    reloadSearch();
  });
  connect(m_manager, &RootItemManager::itemsChanged, this, &RootSearchController::handleItemsChanged);

  regenerateFallback();
  regenerateFavorites();
}

void RootSearchController::setFileSearch(bool value) { m_isFileSearchEnabled = value; }

void RootSearchController::setFilter(std::string_view text) {
  m_query = text;

  if (text.empty()) {
    auto items = m_manager->search("", {.includeFavorites = false, .prioritizeAliased = false});
    m_model->setSearchResults({.query = std::string(text), .items = items});
    return;
  }

  if (text.starts_with('/')) {
    std::error_code ec;
    if (std::filesystem::exists(text, ec)) {
      m_model->setSearchResults({
          .query = std::string(text),
          .files = {{text}},
      });
      return;
    }
  }

  if (auto url = QUrl(QString::fromUtf8(text.data(), text.size()));
      url.isValid() && !url.scheme().isEmpty()) {
    if (auto app = m_appDb->findDefaultOpener(m_query.c_str())) {
      m_model->setSearchResults(
          {.query = std::string(text), .defaultOpener = LinkItem{.app = app, .url = m_query.c_str()}});
      return;
    }
  }

  auto items = m_manager->search(m_query.c_str());

  m_model->setSearchResults({
      .query = std::string(text),
      .items = items,
      .files = {},
  });
  m_calculatorDebounce.start();
  m_fileSearchDebounce.start();
}

void RootSearchController::reloadSearch() { setFilter(m_query); }

void RootSearchController::regenerateFallback() {
  auto items = m_manager->fallbackItems();
  m_model->setFallbackItems(items);
}

void RootSearchController::regenerateFavorites() {
  auto favorites = m_manager->queryFavorites();
  m_model->setFavorites(favorites);
}

void RootSearchController::startCalculator() {
  if (m_calcWatcher.isRunning()) { m_calcWatcher.cancel(); }

  auto expression = QString::fromStdString(m_query);
  m_calculatorSearchQuery = m_query;

  if (expression.startsWith("=") && expression.size() > 1) {
    auto stripped = expression.mid(1);
    m_calcWatcher.setFuture(m_calculator->backend()->asyncCompute(stripped));
    return;
  }

  bool containsNonAlnum = std::ranges::any_of(m_query, [](QChar ch) { return !ch.isLetterOrNumber(); }) ||
                          m_query.starts_with("0x") || m_query.starts_with("0b") || m_query.starts_with("0o");
  const auto isAllowedLeadingChar = [&](QChar c) {
    return c == '-' || c == '(' || c == ')' || c.isLetterOrNumber() || c.category() == QChar::Symbol_Currency;
  };
  bool isComputable = expression.size() > 1 && isAllowedLeadingChar(expression.at(0)) && containsNonAlnum;

  if (!isComputable || !m_calculator->backend()) { return; }

  m_calcWatcher.setFuture(m_calculator->backend()->asyncCompute(expression));
}

void RootSearchController::handleCalculatorFinished() {
  if (!m_calcWatcher.isFinished() || m_calculatorSearchQuery != m_query) return;
  if (auto res = m_calcWatcher.result()) { m_model->setCalculatorResult(res.value()); }
}

void RootSearchController::startFileSearch() {
  if (m_isFileSearchEnabled && m_query.size() >= MIN_FS_TEXT_LENGTH) {
    if (m_fileWatcher.isRunning()) { m_fileWatcher.cancel(); }
    m_fileSearchQuery = m_query;
    m_fileWatcher.setFuture(m_fs->queryAsync(m_query));
  }
}

void RootSearchController::handleFileSearchFinished() {
  if (!m_fileWatcher.isFinished() || m_fileSearchQuery != m_query) return;
  m_model->setFileResults(m_fileWatcher.result());
  m_fileSearchQuery.clear();
}

void RootSearchController::handleItemsChanged() {
  regenerateFallback();
  regenerateFavorites();
  reloadSearch();
}

void RootSearchController::handleFallbackChanged() { regenerateFallback(); }

void RootSearchController::handleFavoriteChanged() { regenerateFavorites(); }
