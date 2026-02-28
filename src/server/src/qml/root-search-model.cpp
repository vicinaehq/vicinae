#include "root-search-model.hpp"
#include "actions/app/app-actions.hpp"
#include "actions/calculator/calculator-actions.hpp"
#include "config/config.hpp"
#include "misc/file-list-item.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "theme.hpp"
#include "theme/theme-file.hpp"
#include <filesystem>
#include <format>
#include <utility>

RootSearchModel::RootSearchModel(const ViewScope &scope, QObject *parent)
    : QAbstractListModel(parent), m_scope(scope), m_manager(scope.services()->rootItemManager()),
      m_appDb(scope.services()->appDb()), m_calculator(scope.services()->calculatorService()),
      m_fileService(scope.services()->fileService()), m_config(scope.services()->config()),
      m_fileSearchEnabled(m_config->value().searchFilesInRoot) {

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

  m_fallbackItems = m_manager->fallbackItems();
  m_favorites = m_manager->queryFavorites();

  connect(&ThemeService::instance(), &ThemeService::themeChanged, this, [this]() {
    if (rowCount() > 0) emit dataChanged(index(0), index(rowCount() - 1), {IconSource, AccessoryColor});
  });

  setFilter({});
}

int RootSearchModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) return 0;
  return static_cast<int>(m_flat.size());
}

QVariant RootSearchModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(m_flat.size())) return {};

  const auto &flat = m_flat[index.row()];

  if (flat.kind == FlatItem::SectionHeader) {
    switch (role) {
    case IsSection:
      return true;
    case IsSelectable:
      return false;
    case SectionName: {
      switch (flat.section) {
      case SectionType::Link:
        return QStringLiteral("Link");
      case SectionType::Calculator:
        return QStringLiteral("Calculator");
      case SectionType::Results:
        return m_query.empty() ? QStringLiteral("Suggestions")
                               : QString::fromStdString(std::format("Results ({})", m_results.size()));
      case SectionType::Files:
        return QStringLiteral("Files");
      case SectionType::Fallback:
        return QString::fromStdString(std::format("Use \"{}\" with...", m_query));
      case SectionType::Favorites:
        return QStringLiteral("Favorites");
      default:
        return {};
      }
    }
    case ItemType:
      return QString();
    case Title:
      return QString();
    case Subtitle:
      return QString();
    case IconSource:
      return QString();
    case Alias:
      return QString();
    case IsActive:
      return false;
    case AccessoryText:
      return QString();
    case AccessoryColor:
      return QString();
    case IsCalculator:
      return false;
    case CalcQuestion:
      return QString();
    case CalcQuestionUnit:
      return QString();
    case CalcAnswer:
      return QString();
    case CalcAnswerUnit:
      return QString();
    case IsFile:
      return false;
    default:
      return {};
    }
  }

  switch (role) {
  case IsSection:
    return false;
  case IsSelectable:
    return true;
  case SectionName:
    return QString();
  case ItemType:
    return itemTypeString(flat.kind);
  case IsCalculator:
    return flat.kind == FlatItem::CalculatorItem;
  case IsFile:
    return flat.kind == FlatItem::FileItem;
  default:
    break;
  }

  if (flat.kind == FlatItem::CalculatorItem) {
    if (!m_calc) return {};
    switch (role) {
    case Title:
      return m_calc->question.text + QStringLiteral(" = ") + m_calc->answer.text;
    case Subtitle:
      return {};
    case IconSource:
      return imageSourceFor(ImageURL::builtin("calculator"));
    case Alias:
      return {};
    case IsActive:
      return false;
    case AccessoryText:
      return {};
    case AccessoryColor:
      return {};
    case CalcQuestion:
      return m_calc->question.text;
    case CalcQuestionUnit:
      return m_calc->question.unit ? m_calc->question.unit->displayName : QString();
    case CalcAnswer:
      return m_calc->answer.text;
    case CalcAnswerUnit:
      return m_calc->answer.unit ? m_calc->answer.unit->displayName : QString();
    default:
      return {};
    }
  }

  if (flat.kind == FlatItem::FileItem) {
    if (flat.dataIndex < 0 || std::cmp_greater_equal(flat.dataIndex, m_files.size())) return {};
    const auto &file = m_files[flat.dataIndex];
    switch (role) {
    case Title:
      return QString::fromStdString(file.path.filename().string());
    case Subtitle:
      return QString::fromStdString(file.path.parent_path().string());
    case IconSource:
      return imageSourceFor(ImageURL::fileIcon(file.path));
    case Alias:
      return {};
    case IsActive:
      return false;
    case AccessoryText:
      return {};
    case AccessoryColor:
      return {};
    case CalcQuestion:
      return {};
    case CalcQuestionUnit:
      return {};
    case CalcAnswer:
      return {};
    case CalcAnswerUnit:
      return {};
    default:
      return {};
    }
  }

  const auto accessoryData = [&](const RootItem *item, int r) -> QVariant {
    if (!item) return {};
    if (r == AccessoryText) {
      auto acc = item->accessories();
      return acc.empty() ? QString() : acc.front().text;
    }
    if (r == AccessoryColor) {
      auto acc = item->accessories();
      if (acc.empty() || !acc.front().color) return QString();
      return resolveAccessoryColor(acc.front().color);
    }
    return {};
  };

  switch (flat.kind) {
  case FlatItem::ResultItem: {
    if (flat.dataIndex < 0 || std::cmp_greater_equal(flat.dataIndex, m_results.size())) return {};
    const auto &owned = m_results[flat.dataIndex];
    const auto &item = owned.item;
    if (!item) return {};
    switch (role) {
    case Title:
      return item->displayName();
    case Subtitle:
      return item->subtitle();
    case IconSource:
      return imageSourceFor(item->iconUrl());
    case Alias:
      return QString::fromStdString(owned.meta.alias.value_or(""));
    case IsActive:
      return item->isActive();
    case AccessoryText:
    case AccessoryColor:
      return accessoryData(item.get(), role);
    case CalcQuestion:
    case CalcQuestionUnit:
    case CalcAnswer:
    case CalcAnswerUnit:
      return {};
    default:
      return {};
    }
  }

  case FlatItem::FallbackItem: {
    if (flat.dataIndex < 0 || std::cmp_greater_equal(flat.dataIndex, m_fallbackItems.size())) return {};
    const auto &item = m_fallbackItems[flat.dataIndex];
    if (!item) return {};
    switch (role) {
    case Title:
      return item->displayName();
    case Subtitle:
      return item->subtitle();
    case IconSource:
      return imageSourceFor(item->iconUrl());
    case Alias:
      return {};
    case IsActive:
      return item->isActive();
    case AccessoryText:
    case AccessoryColor:
      return accessoryData(item.get(), role);
    case CalcQuestion:
    case CalcQuestionUnit:
    case CalcAnswer:
    case CalcAnswerUnit:
      return {};
    default:
      return {};
    }
  }

  case FlatItem::FavoriteItem: {
    if (flat.dataIndex < 0 || std::cmp_greater_equal(flat.dataIndex, m_favorites.size())) return {};
    const auto &item = m_favorites[flat.dataIndex];
    if (!item) return {};
    switch (role) {
    case Title:
      return item->displayName();
    case Subtitle:
      return item->subtitle();
    case IconSource:
      return imageSourceFor(item->iconUrl());
    case Alias: {
      auto meta = m_manager->itemMetadata(item->uniqueId());
      return QString::fromStdString(meta.alias.value_or(""));
    }
    case IsActive:
      return item->isActive();
    case AccessoryText:
    case AccessoryColor:
      return accessoryData(item.get(), role);
    case CalcQuestion:
    case CalcQuestionUnit:
    case CalcAnswer:
    case CalcAnswerUnit:
      return {};
    default:
      return {};
    }
  }

  case FlatItem::LinkItem: {
    if (!m_defaultOpener) return {};
    switch (role) {
    case Title:
      return m_defaultOpener->url;
    case Subtitle:
      return {};
    case IconSource:
      return imageSourceFor(m_defaultOpener->app->iconUrl());
    case Alias:
      return {};
    case IsActive:
      return false;
    case AccessoryText:
      return {};
    case AccessoryColor:
      return {};
    case CalcQuestion:
    case CalcQuestionUnit:
    case CalcAnswer:
    case CalcAnswerUnit:
      return {};
    default:
      return {};
    }
  }

  default:
    return {};
  }
}

QHash<int, QByteArray> RootSearchModel::roleNames() const {
  return {
      {IsSection, "isSection"},
      {IsSelectable, "isSelectable"},
      {SectionName, "sectionName"},
      {ItemType, "itemType"},
      {Title, "title"},
      {Subtitle, "subtitle"},
      {IconSource, "iconSource"},
      {Alias, "alias"},
      {IsActive, "isActive"},
      {AccessoryText, "accessoryText"},
      {AccessoryColor, "accessoryColor"},
      {IsCalculator, "isCalculator"},
      {CalcQuestion, "calcQuestion"},
      {CalcQuestionUnit, "calcQuestionUnit"},
      {CalcAnswer, "calcAnswer"},
      {CalcAnswerUnit, "calcAnswerUnit"},
      {IsFile, "isFile"},
  };
}

void RootSearchModel::setFilter(const QString &text) {
  auto query = text.toStdString();
  if (query == m_query) return;
  m_query = std::move(query);
  m_selectedIndex = -1;
  m_selectFirstOnReset = true;
  emit selectFirstOnResetChanged();
  m_scope.clearActions();
  m_calc.reset();
  m_files.clear();

  m_calculatorDebounce.stop();
  m_fileSearchDebounce.stop();

  rerunSearch();

  if (!text.isEmpty()) {
    m_calculatorDebounce.start();
    m_fileSearchDebounce.start();
  }
}

void RootSearchModel::refresh() {
  m_fallbackItems = m_manager->fallbackItems();
  m_favorites = m_manager->queryFavorites();

  m_selectFirstOnReset = false;
  emit selectFirstOnResetChanged();

  rerunSearch();

  m_selectFirstOnReset = true;
  emit selectFirstOnResetChanged();
}

void RootSearchModel::rerunSearch() {
  auto text = QString::fromStdString(m_query);

  if (!text.isEmpty() && text.startsWith('/')) {
    std::error_code ec;
    if (std::filesystem::exists(m_query, ec)) {
      m_defaultOpener.reset();
      m_results.clear();
      m_files = {{std::filesystem::path(m_query), 1.0}};
      beginResetModel();
      rebuildFlatList();
      endResetModel();
      return;
    }
  }

  if (!text.isEmpty()) {
    if (auto url = QUrl(text); url.isValid() && !url.scheme().isEmpty()) {
      if (auto app = m_appDb->findDefaultOpener(text)) {
        m_defaultOpener = ::LinkItem{.app = app, .url = text};
        m_results.clear();
        beginResetModel();
        rebuildFlatList();
        endResetModel();
        return;
      }
    }
  }

  m_defaultOpener.reset();

  std::vector<RootItemManager::ScoredItem> scored;
  if (m_query.empty()) {
    m_manager->search("", scored, {.includeFavorites = false, .prioritizeAliased = false});
  } else {
    m_manager->search(text, scored);
  }

  m_results.clear();
  m_results.reserve(scored.size());
  for (const auto &s : scored) {
    m_results.push_back({
        .item = s.item.get(),
        .meta = s.meta ? *s.meta : RootItemMetadata{},
    });
  }

  beginResetModel();
  rebuildFlatList();
  endResetModel();
}

void RootSearchModel::rebuildFlatList() {
  m_flat.clear();

  if (m_query.empty()) {
    addSection(SectionType::Favorites, "Favorites", static_cast<int>(m_favorites.size()));
    addSection(SectionType::Results, "Suggestions", static_cast<int>(m_results.size()));
  } else {
    if (m_defaultOpener) { addSection(SectionType::Link, "Link", 1); }
    if (m_calc) { addSection(SectionType::Calculator, "Calculator", 1); }
    addSection(SectionType::Results, "", static_cast<int>(m_results.size()));
    addSection(SectionType::Files, "Files", static_cast<int>(m_files.size()));
    addSection(SectionType::Fallback, "", static_cast<int>(m_fallbackItems.size()));
  }
}

void RootSearchModel::addSection(SectionType section, const std::string &name, int count) {
  if (count == 0) return;

  m_flat.push_back({.kind = FlatItem::SectionHeader, .dataIndex = -1, .section = section});

  FlatItem::Kind itemKind;
  switch (section) {
  case SectionType::Results:
    itemKind = FlatItem::ResultItem;
    break;
  case SectionType::Fallback:
    itemKind = FlatItem::FallbackItem;
    break;
  case SectionType::Favorites:
    itemKind = FlatItem::FavoriteItem;
    break;
  case SectionType::Link:
    itemKind = FlatItem::LinkItem;
    break;
  case SectionType::Calculator:
    itemKind = FlatItem::CalculatorItem;
    break;
  case SectionType::Files:
    itemKind = FlatItem::FileItem;
    break;
  default:
    itemKind = FlatItem::ResultItem;
    break;
  }

  for (int i = 0; i < count; ++i) {
    m_flat.push_back({.kind = itemKind, .dataIndex = i, .section = section});
  }
}

int RootSearchModel::nextSelectableIndex(int from, int direction) const {
  int const count = static_cast<int>(m_flat.size());
  if (count == 0) return from;

  int idx = from + direction;
  if (idx < 0)
    idx = count - 1;
  else if (idx >= count)
    idx = 0;

  while (idx != from) {
    if (m_flat[idx].kind != FlatItem::SectionHeader) return idx;
    idx += direction;
    if (idx < 0)
      idx = count - 1;
    else if (idx >= count)
      idx = 0;
  }

  return from;
}

QString RootSearchModel::itemTypeString(FlatItem::Kind kind) const {
  switch (kind) {
  case FlatItem::ResultItem:
    return QStringLiteral("result");
  case FlatItem::FallbackItem:
    return QStringLiteral("fallback");
  case FlatItem::FavoriteItem:
    return QStringLiteral("favorite");
  case FlatItem::LinkItem:
    return QStringLiteral("link");
  case FlatItem::CalculatorItem:
    return QStringLiteral("calculator");
  case FlatItem::FileItem:
    return QStringLiteral("file");
  default:
    return QStringLiteral("unknown");
  }
}

QString RootSearchModel::resolveAccessoryColor(const std::optional<ColorLike> &color) const {
  if (!color) return {};
  const auto &theme = ThemeService::instance().theme();
  const auto visitor = [&](auto &&c) -> QString {
    using T = std::decay_t<decltype(c)>;
    if constexpr (std::is_same_v<T, QColor>) {
      return c.name(QColor::HexArgb);
    } else if constexpr (std::is_same_v<T, QString>) {
      return c;
    } else if constexpr (std::is_same_v<T, SemanticColor>) {
      return theme.resolve(c).name(QColor::HexArgb);
    } else if constexpr (std::is_same_v<T, DynamicColor>) {
      if (theme.isLight()) return c.light;
      return c.dark;
    } else {
      return {};
    }
  };
  return std::visit(visitor, *color);
}

void RootSearchModel::setSelectedIndex(int index) {
  if (index == m_selectedIndex) return;
  m_selectedIndex = index;

  if (index < 0 || std::cmp_greater_equal(index, m_flat.size())) {
    m_lastSelectedItemId.clear();
    m_scope.clearActions();
    emit primaryActionChanged();
    return;
  }

  const auto &flat = m_flat[index];

  // Compute a stable identity for the selected item
  QString itemId;
  switch (flat.kind) {
  case FlatItem::ResultItem:
    if (flat.dataIndex >= 0 && std::cmp_less(flat.dataIndex, m_results.size()))
      itemId = QString::fromStdString(m_results[flat.dataIndex].item->uniqueId());
    break;
  case FlatItem::FallbackItem:
    if (flat.dataIndex >= 0 && std::cmp_less(flat.dataIndex, m_fallbackItems.size()))
      itemId = QString::fromStdString(m_fallbackItems[flat.dataIndex]->uniqueId());
    break;
  case FlatItem::FavoriteItem:
    if (flat.dataIndex >= 0 && std::cmp_less(flat.dataIndex, m_favorites.size()))
      itemId = QString::fromStdString(m_favorites[flat.dataIndex]->uniqueId());
    break;
  case FlatItem::LinkItem:
    if (m_defaultOpener) itemId = m_defaultOpener->url;
    break;
  case FlatItem::CalculatorItem:
    if (m_calc) itemId = QStringLiteral("calc:") + m_calc->question.text;
    break;
  case FlatItem::FileItem:
    if (flat.dataIndex >= 0 && std::cmp_less(flat.dataIndex, m_files.size()))
      itemId = QString::fromStdString(m_files[flat.dataIndex].path.string());
    break;
  default:
    break;
  }

  bool const sameItem = (!itemId.isEmpty() && itemId == m_lastSelectedItemId);
  m_lastSelectedItemId = itemId;

  std::unique_ptr<ActionPanelState> actionPanel;

  switch (flat.kind) {
  case FlatItem::ResultItem: {
    if (flat.dataIndex >= 0 && std::cmp_less(flat.dataIndex, m_results.size())) {
      actionPanel = m_results[flat.dataIndex].item->newActionPanel(m_scope.appContext(),
                                                                   m_results[flat.dataIndex].meta);
    }
    break;
  }
  case FlatItem::FallbackItem: {
    if (flat.dataIndex >= 0 && std::cmp_less(flat.dataIndex, m_fallbackItems.size())) {
      auto &item = m_fallbackItems[flat.dataIndex];
      actionPanel =
          item->fallbackActionPanel(m_scope.appContext(), m_manager->itemMetadata(item->uniqueId()));
    }
    break;
  }
  case FlatItem::FavoriteItem: {
    if (flat.dataIndex >= 0 && std::cmp_less(flat.dataIndex, m_favorites.size())) {
      auto &item = m_favorites[flat.dataIndex];
      actionPanel = item->newActionPanel(m_scope.appContext(), m_manager->itemMetadata(item->uniqueId()));
    }
    break;
  }
  case FlatItem::LinkItem: {
    if (m_defaultOpener) {
      actionPanel = std::make_unique<ActionPanelState>();
      auto section = actionPanel->createSection();
      auto open = new OpenAppAction(m_defaultOpener->app,
                                    QString("Open in %1").arg(m_defaultOpener->app->displayName()),
                                    {m_defaultOpener->url});
      open->setClearSearch(true);
      section->addAction(open);
    }
    break;
  }
  case FlatItem::CalculatorItem: {
    if (m_calc) {
      auto panel = std::make_unique<ListActionPanelState>();
      auto main = panel->createSection();
      auto copyAnswer = new CopyCalculatorAnswerAction(*m_calc);
      auto copyQA = new CopyCalculatorQuestionAndAnswerAction(*m_calc);
      auto putInSearch = new PutCalculatorAnswerInSearchBar(*m_calc);
      auto openHistory = new OpenCalculatorHistoryAction();
      copyAnswer->setPrimary(true);
      main->addAction(copyAnswer);
      main->addAction(copyQA);
      main->addAction(putInSearch);
      main->addAction(openHistory);
      actionPanel = std::move(panel);
    }
    break;
  }
  case FlatItem::FileItem: {
    if (flat.dataIndex >= 0 && std::cmp_less(flat.dataIndex, m_files.size())) {
      actionPanel = FileActions::actionPanel(m_files[flat.dataIndex].path, m_appDb);
    }
    break;
  }
  default:
    break;
  }

  if (actionPanel) {
    actionPanel->finalize();
    m_scope.setActions(std::move(actionPanel));
  } else {
    m_scope.clearActions();
  }

  // Completer: create/destroy argument completion based on the selected item.
  // Skip if the same logical item is still selected (e.g. after search filter update).
  if (!sameItem) {
    bool createdCompleter = false;

    auto tryCreateCompleter = [&](const RootItem *item) {
      if (!item) return;
      ArgumentList const args = item->arguments();
      if (args.empty()) return;
      m_scope.createCompletion(args, item->iconUrl());
      createdCompleter = true;
    };

    switch (flat.kind) {
    case FlatItem::ResultItem:
      if (flat.dataIndex >= 0 && std::cmp_less(flat.dataIndex, m_results.size()))
        tryCreateCompleter(m_results[flat.dataIndex].item.get());
      break;
    case FlatItem::FavoriteItem:
      if (flat.dataIndex >= 0 && std::cmp_less(flat.dataIndex, m_favorites.size()))
        tryCreateCompleter(m_favorites[flat.dataIndex].get());
      break;
    default:
      break;
    }

    if (!createdCompleter) { m_scope.destroyCurrentCompletion(); }
  }

  emit primaryActionChanged();
}

void RootSearchModel::activateSelected() { m_scope.executePrimaryAction(); }

bool RootSearchModel::tryAliasFastTrack() {
  if (m_selectedIndex < 0 || std::cmp_greater_equal(m_selectedIndex, m_flat.size())) return false;

  const auto &flat = m_flat[m_selectedIndex];

  const auto check = [&](const RootItem *item) -> bool {
    if (!item || !item->supportsAliasSpaceShortcut()) return false;
    auto meta = m_manager->itemMetadata(item->uniqueId());
    if (!meta.alias) return false;
    return meta.alias->starts_with(m_query);
  };

  bool shouldFastTrack = false;

  switch (flat.kind) {
  case FlatItem::ResultItem:
    if (flat.dataIndex >= 0 && std::cmp_less(flat.dataIndex, m_results.size()))
      shouldFastTrack = check(m_results[flat.dataIndex].item.get());
    break;
  case FlatItem::FavoriteItem:
    if (flat.dataIndex >= 0 && std::cmp_less(flat.dataIndex, m_favorites.size()))
      shouldFastTrack = check(m_favorites[flat.dataIndex].get());
    break;
  default:
    break;
  }

  if (shouldFastTrack) {
    activateSelected();
    return true;
  }

  return false;
}

QString RootSearchModel::primaryActionTitle() const {
  auto *state = m_scope.topState();
  if (!state || !state->actionPanelState) return {};
  auto *action = state->actionPanelState->primaryAction();
  return action ? action->title() : QString();
}

QString RootSearchModel::primaryActionIcon() const {
  auto *state = m_scope.topState();
  if (!state || !state->actionPanelState) return {};
  auto *action = state->actionPanelState->primaryAction();
  if (!action) return {};
  auto icon = action->icon();
  return icon ? imageSourceFor(*icon) : QString();
}

void RootSearchModel::startCalculator() {
  if (m_calcWatcher.isRunning()) { m_calcWatcher.cancel(); }

  auto expression = QString::fromStdString(m_query);
  m_calculatorSearchQuery = m_query;

  if (expression.startsWith("=") && expression.size() > 1) {
    auto stripped = expression.mid(1);
    m_calcWatcher.setFuture(m_calculator->backend()->asyncCompute(stripped));
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

  if (!isComputable || !m_calculator->backend()) { return; }

  m_calcWatcher.setFuture(m_calculator->backend()->asyncCompute(expression));
}

void RootSearchModel::handleCalculatorFinished() {
  if (!m_calcWatcher.isFinished() || m_calculatorSearchQuery != m_query) return;
  auto res = m_calcWatcher.result();
  if (!res) return;

  m_calc = res.value();
  m_selectedIndex = -1;
  beginResetModel();
  rebuildFlatList();
  endResetModel();
}

void RootSearchModel::startFileSearch() {
  if (!m_fileSearchEnabled || m_query.size() < MIN_FS_TEXT_LENGTH) return;
  if (m_fileWatcher.isRunning()) { m_fileWatcher.cancel(); }
  m_fileSearchQuery = m_query;
  m_fileWatcher.setFuture(m_fileService->queryAsync(m_query));
}

void RootSearchModel::handleFileSearchFinished() {
  if (!m_fileWatcher.isFinished() || m_fileSearchQuery != m_query) return;
  m_files = m_fileWatcher.result();
  m_fileSearchQuery.clear();

  m_selectFirstOnReset = false;
  emit selectFirstOnResetChanged();

  beginResetModel();
  rebuildFlatList();
  endResetModel();
}
