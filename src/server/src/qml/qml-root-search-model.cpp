#include "qml-root-search-model.hpp"
#include "actions/app/app-actions.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include <format>

QmlRootSearchModel::QmlRootSearchModel(ApplicationContext &ctx, QObject *parent)
    : QAbstractListModel(parent), m_ctx(&ctx),
      m_manager(ctx.services->rootItemManager()), m_appDb(ctx.services->appDb()) {

  connect(m_manager, &RootItemManager::metadataChanged, this, [this]() {
    m_fallbackItems = m_manager->fallbackItems();
    m_favorites = m_manager->queryFavorites();
    setFilter(QString::fromStdString(m_query));
  });

  connect(m_manager, &RootItemManager::itemsChanged, this, [this]() {
    m_fallbackItems = m_manager->fallbackItems();
    m_favorites = m_manager->queryFavorites();
    setFilter(QString::fromStdString(m_query));
  });

  m_fallbackItems = m_manager->fallbackItems();
  m_favorites = m_manager->queryFavorites();

  // Initial empty search to populate suggestions
  setFilter({});
}

int QmlRootSearchModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) return 0;
  return static_cast<int>(m_flat.size());
}

QVariant QmlRootSearchModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(m_flat.size()))
    return {};

  const auto &flat = m_flat[index.row()];

  if (flat.kind == FlatItem::SectionHeader) {
    switch (role) {
    case IsSection: return true;
    case IsSelectable: return false;
    case SectionName: {
      switch (flat.section) {
      case SectionType::Link: return QStringLiteral("Link");
      case SectionType::Results:
        return m_query.empty()
            ? QStringLiteral("Suggestions")
            : QString::fromStdString(std::format("Results ({})", m_results.size()));
      case SectionType::Fallback:
        return QString::fromStdString(std::format("Use \"{}\" with...", m_query));
      case SectionType::Favorites: return QStringLiteral("Favorites");
      default: return {};
      }
    }
    // Return defaults for item roles so QML required properties are satisfied
    case ItemType: return QString();
    case Title: return QString();
    case Subtitle: return QString();
    case IconSource: return QString();
    case Alias: return QString();
    case IsActive: return false;
    default: return {};
    }
  }

  // Item roles
  switch (role) {
  case IsSection: return false;
  case IsSelectable: return true;
  case SectionName: return QString();
  case ItemType: return itemTypeString(flat.kind);
  }

  // Data roles that depend on the item kind
  switch (flat.kind) {
  case FlatItem::ResultItem: {
    if (flat.dataIndex < 0 || flat.dataIndex >= static_cast<int>(m_results.size()))
      return {};
    const auto &owned = m_results[flat.dataIndex];
    const auto &item = owned.item;
    if (!item) return {};
    switch (role) {
    case Title: return item->displayName();
    case Subtitle: return item->subtitle();
    case IconSource: return imageSourceFor(item->iconUrl());
    case Alias: return QString::fromStdString(owned.meta.alias.value_or(""));
    case IsActive: return item->isActive();
    default: return {};
    }
  }

  case FlatItem::FallbackItem: {
    if (flat.dataIndex < 0 || flat.dataIndex >= static_cast<int>(m_fallbackItems.size()))
      return {};
    const auto &item = m_fallbackItems[flat.dataIndex];
    if (!item) return {};
    switch (role) {
    case Title: return item->displayName();
    case Subtitle: return item->subtitle();
    case IconSource: return imageSourceFor(item->iconUrl());
    case Alias: return {};
    case IsActive: return item->isActive();
    default: return {};
    }
  }

  case FlatItem::FavoriteItem: {
    if (flat.dataIndex < 0 || flat.dataIndex >= static_cast<int>(m_favorites.size()))
      return {};
    const auto &item = m_favorites[flat.dataIndex];
    if (!item) return {};
    switch (role) {
    case Title: return item->displayName();
    case Subtitle: return item->subtitle();
    case IconSource: return imageSourceFor(item->iconUrl());
    case Alias: {
      auto meta = m_manager->itemMetadata(item->uniqueId());
      return QString::fromStdString(meta.alias.value_or(""));
    }
    case IsActive: return item->isActive();
    default: return {};
    }
  }

  case FlatItem::LinkItem: {
    if (!m_defaultOpener) return {};
    switch (role) {
    case Title: return m_defaultOpener->url;
    case Subtitle: return {};
    case IconSource: return imageSourceFor(m_defaultOpener->app->iconUrl());
    case Alias: return {};
    case IsActive: return false;
    default: return {};
    }
  }

  default: return {};
  }
}

QHash<int, QByteArray> QmlRootSearchModel::roleNames() const {
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
  };
}

void QmlRootSearchModel::setFilter(const QString &text) {
  m_query = text.toStdString();
  m_selectedIndex = -1;
  m_actionPanel.reset();

  // Check for URL patterns
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
  if (text.isEmpty()) {
    m_manager->search("", scored, {.includeFavorites = false, .prioritizeAliased = false});
  } else {
    m_manager->search(text, scored);
  }

  // Snapshot results: copy shared_ptr and metadata so we don't hold dangling reference_wrappers
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

void QmlRootSearchModel::rebuildFlatList() {
  m_flat.clear();

  if (m_query.empty()) {
    // Root sections: Favorites, then Results (suggestions)
    addSection(SectionType::Favorites, "Favorites", static_cast<int>(m_favorites.size()));
    addSection(SectionType::Results, "Suggestions", static_cast<int>(m_results.size()));
  } else {
    // Search sections: Link, Results, Fallback
    if (m_defaultOpener) {
      addSection(SectionType::Link, "Link", 1);
    }
    addSection(SectionType::Results, "", static_cast<int>(m_results.size()));
    addSection(SectionType::Fallback, "", static_cast<int>(m_fallbackItems.size()));
  }
}

void QmlRootSearchModel::addSection(SectionType section, const std::string &name, int count) {
  if (count == 0) return;

  m_flat.push_back({.kind = FlatItem::SectionHeader, .dataIndex = -1, .section = section});

  FlatItem::Kind itemKind;
  switch (section) {
  case SectionType::Results: itemKind = FlatItem::ResultItem; break;
  case SectionType::Fallback: itemKind = FlatItem::FallbackItem; break;
  case SectionType::Favorites: itemKind = FlatItem::FavoriteItem; break;
  case SectionType::Link: itemKind = FlatItem::LinkItem; break;
  default: itemKind = FlatItem::ResultItem; break;
  }

  for (int i = 0; i < count; ++i) {
    m_flat.push_back({.kind = itemKind, .dataIndex = i, .section = section});
  }
}

int QmlRootSearchModel::nextSelectableIndex(int from, int direction) const {
  int idx = from + direction;
  int count = static_cast<int>(m_flat.size());

  while (idx >= 0 && idx < count) {
    if (m_flat[idx].kind != FlatItem::SectionHeader) return idx;
    idx += direction;
  }

  return from; // stay in place if nothing found
}

QString QmlRootSearchModel::imageSourceFor(const ImageURL &url) const {
  switch (url.type()) {
  case ImageURLType::Builtin: {
    QString base = QString("image://vicinae/builtin:%1").arg(url.name());
    if (auto bg = url.backgroundTint())
      base += QString("?bg=%1").arg(static_cast<int>(*bg));
    if (auto fg = url.foregroundTint())
      base += QString(base.contains('?') ? "&fg=%1" : "?fg=%1").arg(static_cast<int>(*fg));
    return base;
  }
  case ImageURLType::System:
    return QString("image://vicinae/system:%1").arg(url.name());
  case ImageURLType::Local:
    return QString("image://vicinae/local:%1").arg(url.name());
  case ImageURLType::Emoji:
    return QString("image://vicinae/emoji:%1").arg(url.name());
  case ImageURLType::Http:
  case ImageURLType::Https:
    // HTTP icons fall back to placeholder for POC
    return QStringLiteral("image://vicinae/builtin:globe-01");
  case ImageURLType::Favicon:
    return QStringLiteral("image://vicinae/builtin:globe-01");
  default:
    return QStringLiteral("image://vicinae/builtin:question-mark-circle");
  }
}

QString QmlRootSearchModel::itemTypeString(FlatItem::Kind kind) const {
  switch (kind) {
  case FlatItem::ResultItem: return QStringLiteral("result");
  case FlatItem::FallbackItem: return QStringLiteral("fallback");
  case FlatItem::FavoriteItem: return QStringLiteral("favorite");
  case FlatItem::LinkItem: return QStringLiteral("link");
  default: return QStringLiteral("unknown");
  }
}

void QmlRootSearchModel::setSelectedIndex(int index) {
  if (index == m_selectedIndex) return;
  m_selectedIndex = index;

  if (index < 0 || index >= static_cast<int>(m_flat.size())) {
    m_actionPanel.reset();
    emit primaryActionChanged();
    return;
  }

  const auto &flat = m_flat[index];

  switch (flat.kind) {
  case FlatItem::ResultItem: {
    if (flat.dataIndex >= 0 && flat.dataIndex < static_cast<int>(m_results.size())) {
      m_actionPanel = m_results[flat.dataIndex].item->newActionPanel(m_ctx, m_results[flat.dataIndex].meta);
    }
    break;
  }
  case FlatItem::FallbackItem: {
    if (flat.dataIndex >= 0 && flat.dataIndex < static_cast<int>(m_fallbackItems.size())) {
      auto &item = m_fallbackItems[flat.dataIndex];
      m_actionPanel = item->fallbackActionPanel(m_ctx, m_manager->itemMetadata(item->uniqueId()));
    }
    break;
  }
  case FlatItem::FavoriteItem: {
    if (flat.dataIndex >= 0 && flat.dataIndex < static_cast<int>(m_favorites.size())) {
      auto &item = m_favorites[flat.dataIndex];
      m_actionPanel = item->newActionPanel(m_ctx, m_manager->itemMetadata(item->uniqueId()));
    }
    break;
  }
  case FlatItem::LinkItem: {
    if (m_defaultOpener) {
      auto panel = std::make_unique<ActionPanelState>();
      auto section = panel->createSection();
      auto open = new OpenAppAction(
          m_defaultOpener->app,
          QString("Open in %1").arg(m_defaultOpener->app->displayName()),
          {m_defaultOpener->url});
      open->setClearSearch(true);
      section->addAction(open);
      m_actionPanel = std::move(panel);
    }
    break;
  }
  default:
    m_actionPanel.reset();
    break;
  }

  if (m_actionPanel) {
    m_actionPanel->finalize();
  }

  emit primaryActionChanged();
}

void QmlRootSearchModel::activateSelected() {
  if (!m_actionPanel) return;

  auto *action = m_actionPanel->primaryAction();
  if (!action) return;

  // Keep the panel alive: executeAction may re-enter setFilter (via
  // activateView → searchTextTampered → QML handler) which resets
  // m_actionPanel, destroying the action we're still using.
  auto keepAlive = std::move(m_actionPanel);
  m_ctx->navigation->executeAction(action);
}

QString QmlRootSearchModel::primaryActionTitle() const {
  if (!m_actionPanel) return {};
  auto *action = m_actionPanel->primaryAction();
  return action ? action->title() : QString();
}

QString QmlRootSearchModel::primaryActionIcon() const {
  if (!m_actionPanel) return {};
  auto *action = m_actionPanel->primaryAction();
  if (!action) return {};
  auto icon = action->icon();
  return icon ? imageSourceFor(*icon) : QString();
}
