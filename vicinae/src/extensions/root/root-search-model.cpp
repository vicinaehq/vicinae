#include "root-search-model.hpp"
#include "services/app-service/app-service.hpp"
#include "services/calculator-service/calculator-service.hpp"
#include "ui/transform-result/transform-result.hpp"

RootSearchModel::RootSearchModel(RootItemManager *manager, FileService *fs, AppService *appDb,
                                 CalculatorService *calculator)
    : m_manager(manager), m_fs(fs), m_appDb(appDb), m_calculator(calculator) {
  using namespace std::chrono_literals;

  regenerateFallback();
  regenerateFavorites();
  m_fileSearchDebounce.setInterval(100ms);
  m_fileSearchDebounce.setSingleShot(true);
  m_calculatorDebounce.setInterval(100ms);
  m_calculatorDebounce.setSingleShot(true);

  connect(&m_fileSearchDebounce, &QTimer::timeout, this, &RootSearchModel::startFileSearch);
  connect(&m_calculatorDebounce, &QTimer::timeout, this, &RootSearchModel::startCalculator);
  connect(&m_calcWatcher, &QFutureWatcher<AbstractCalculatorBackend::ComputeResult>::finished, this,
          &RootSearchModel::handleCalculatorFinished);
  connect(m_manager, &RootItemManager::fallbackDisabled, this, &RootSearchModel::regenerateFallback);
  connect(m_manager, &RootItemManager::fallbackEnabled, this, &RootSearchModel::regenerateFallback);
  connect(m_manager, &RootItemManager::itemsChanged, this, &RootSearchModel::reloadSearch);
  connect(m_manager, &RootItemManager::itemFavoriteChanged, this, [this]() { regenerateFavorites(); });
  connect(&m_fileWatcher, &FileSearchWatcher::finished, this, &RootSearchModel::handleFileSearchFinished);
}

void RootSearchModel::setFileSearch(bool value) { m_isFileSearchEnabled = value; }

void RootSearchModel::setFilter(std::string_view text) {
  query = text;
  m_defaultOpener.reset();
  m_calc.reset();
  m_items = {};
  m_files.clear();

  if (text.empty()) {
    m_items = m_manager->search("", {.includeFavorites = false, .prioritizeAliased = false});
    emit dataChanged();
    return;
  }

  if (text.starts_with('/')) {
    std::error_code ec;
    if (std::filesystem::exists(text, ec)) {
      m_files = {{.path = text}};
      emit dataChanged();
      return;
    }
  }

  if (auto url = QUrl(QString::fromUtf8(text.data(), text.size()));
      url.isValid() && !url.scheme().isEmpty()) {
    if (auto app = m_appDb->findDefaultOpener(query.c_str())) {
      m_defaultOpener = LinkItem{.app = app, .url = query.c_str()};
      emit dataChanged();
      return;
    }
  }

  m_items = m_manager->search(query.c_str());

  m_resultSectionTitle = std::format("Results ({})", m_items.size());
  m_fallbackSectionTitle = std::format("Use \"{}\" with...", query);
  m_calculatorDebounce.start();
  m_fileSearchDebounce.start();

  emit dataChanged();
}

int RootSearchModel::sectionCount() const { return sections().size(); }

int RootSearchModel::sectionItemHeight(SectionType id) const {
  switch (id) {
  case SectionType::Calculator:
    return CALCULATOR_HEIGHT;
  default:
    return ITEM_HEIGHT;
  }
}

SectionType RootSearchModel::sectionIdFromIndex(int idx) const { return sections()[idx]; }

int RootSearchModel::sectionItemCount(SectionType id) const {
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

std::string_view RootSearchModel::sectionName(SectionType id) const {
  switch (id) {
  case SectionType::Link:
    return "Link";
  case SectionType::Calculator:
    return "Calculator";
  case SectionType::Results:
    return query.empty() ? std::string_view("Suggestions") : m_resultSectionTitle;
  case SectionType::Files:
    return "Files";
  case SectionType::Fallback:
    return m_fallbackSectionTitle;
  case SectionType::Favorites:
    return "Favorites";
  default:
    return "Unknown";
  }
}

RootItemVariant RootSearchModel::sectionItemAt(SectionType id, int itemIdx) const {
  switch (id) {
  case SectionType::Link:
    return m_defaultOpener.value();
  case SectionType::Calculator:
    return m_calc.value();
  case SectionType::Results:
    return m_items[itemIdx].item.get().get();
  case SectionType::Files:
    return m_files[itemIdx].path;
  case SectionType::Fallback:
    return FallbackItem{.item = m_fallbackItems[itemIdx].get()};
  case SectionType::Favorites:
    return FavoriteItem{.item = m_favorites[itemIdx].item.get()};
  }

  return {};
}

RootSearchModel::StableID RootSearchModel::stableId(const RootItemVariant &item) const {
  static std::hash<QString> hasher = {};
  const auto visitor =
      overloads{[&](const AbstractCalculatorBackend::CalculatorResult &) { return randomId(); },
                [](const RootItem *item) { return hasher(item->uniqueId()); },
                [](const LinkItem &item) { return hasher(item.url + ".url"); },
                [](const std::filesystem::path &path) { return hasher(QString(path.c_str()) + ".files"); },
                [](const FallbackItem &item) { return hasher(item.item->uniqueId() + ".fallback"); },
                [](const FavoriteItem &item) { return hasher(item.item->uniqueId() + ".favorite"); }};
  return std::visit(visitor, item);
}

RootSearchModel::WidgetTag RootSearchModel::widgetTag(const RootItemVariant &item) const {
  if (std::holds_alternative<AbstractCalculatorBackend::CalculatorResult>(item)) { return InvalidTag; }
  return item.index();
}

RootSearchModel::WidgetType *RootSearchModel::createItemWidget(const RootItemVariant &type) const {
  const auto visitor = overloads{
      [](const AbstractCalculatorBackend::CalculatorResult &) -> WidgetType * { return new TransformResult; },
      [](const RootItem *) -> WidgetType * { return new DefaultListItemWidget; },
      [](const std::filesystem::path &path) -> WidgetType * { return new DefaultListItemWidget; },
      [](const LinkItem &item) -> WidgetType * { return new DefaultListItemWidget; },
      [](const FallbackItem &item) -> WidgetType * { return new DefaultListItemWidget; },
      [](const FavoriteItem &item) -> WidgetType * { return new DefaultListItemWidget; }};

  return std::visit(visitor, type);
}

void RootSearchModel::refreshItemWidget(const RootItemVariant &type, WidgetType *widget) const {
  auto refreshRootItem = [&](const RootItem *item, bool showAlias = true) {
    auto w = static_cast<DefaultListItemWidget *>(widget);
    w->setAlias(showAlias ? m_manager->itemMetadata(item->uniqueId()).alias.c_str() : "");
    w->setName(item->displayName());
    w->setIconUrl(item->iconUrl());
    w->setSubtitle(item->subtitle());
    w->setAccessories(item->accessories());
    w->setActive(item->isActive());
  };
  const auto visitor =
      overloads{[&](const AbstractCalculatorBackend::CalculatorResult &calc) {
                  auto w = static_cast<TransformResult *>(widget);
                  const auto toDp = [](auto &&u) { return u.displayName; };
                  w->setBase(calc.question.text, calc.question.unit.transform(toDp).value_or("Expression"));
                  w->setResult(calc.answer.text, calc.answer.unit.transform(toDp).value_or("Answer"));
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
                [&](const FallbackItem &item) { refreshRootItem(item.item, false); },
                [&](const FavoriteItem &item) { refreshRootItem(item.item); }};

  std::visit(visitor, type);
}

void RootSearchModel::startCalculator() {
  if (m_calcWatcher.isRunning()) { m_calcWatcher.cancel(); }

  auto expression = QString::fromStdString(query);

  m_calculatorSearchQuery = query;

  if (expression.startsWith("=") && expression.size() > 1) {
    auto stripped = expression.mid(1);
    m_calcWatcher.setFuture(m_calculator->backend()->asyncCompute(stripped));
    return;
  }

  bool containsNonAlnum = std::ranges::any_of(query, [](QChar ch) { return !ch.isLetterOrNumber(); });
  const auto isAllowedLeadingChar = [](QChar c) { return c == '(' || c == ')' || c.isLetterOrNumber(); };
  bool isComputable = expression.size() > 1 && isAllowedLeadingChar(expression.at(0)) && containsNonAlnum;

  auto e = []() {};

  if (!isComputable || !m_calculator->backend()) { return; }

  m_calcWatcher.setFuture(m_calculator->backend()->asyncCompute(expression));
}

void RootSearchModel::handleCalculatorFinished() {
  if (!m_calcWatcher.isFinished() || m_calculatorSearchQuery != query) return;
  if (auto res = m_calcWatcher.result()) {
    m_calc = res.value();
    emit dataChanged();
  }
}

void RootSearchModel::startFileSearch() {
  if (m_isFileSearchEnabled && query.size() >= MIN_FS_TEXT_LENGTH) {
    if (m_fileWatcher.isRunning()) { m_fileWatcher.cancel(); }
    m_fileSearchQuery = query;
    m_fileWatcher.setFuture(m_fs->queryAsync(query));
  }
}

void RootSearchModel::handleFileSearchFinished() {
  if (!m_fileWatcher.isFinished() || m_fileSearchQuery != query) return;
  m_files = m_fileWatcher.result();
  m_fileSearchQuery.clear();
  emit dataChanged();
}
