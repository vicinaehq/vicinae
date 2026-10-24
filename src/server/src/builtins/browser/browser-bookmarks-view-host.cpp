#include <QDateTime>
#include <QLocale>
#include <algorithm>
#include <ranges>
#include <set>
#include <utility>
#include "builtins/browser/browser-bookmarks-view-host.hpp"
#include "actions/app-actions.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "theme/colors.hpp"
#include "ui/action-panel/action.hpp"
#include "ui/views/view-utils.hpp"

namespace {

constexpr QStringView ALL_FOLDERS_ID = u"all";
constexpr QStringView FOLDER_ID_PREFIX = u"folder:";
constexpr QChar FOLDER_ID_SEPARATOR = u'\x1f';
constexpr QStringView FOLDER_STORAGE_KEY = u"bookmarkFolder";
constexpr QStringView DETAIL_STORAGE_KEY = u"showDetail";

QString folderId(std::span<const std::string> folder) {
  QStringList parts;
  parts.reserve(folder.size());
  for (const auto &name : folder) {
    parts.emplace_back(QString::fromStdString(name));
  }
  return FOLDER_ID_PREFIX + parts.join(FOLDER_ID_SEPARATOR);
}

std::optional<std::vector<std::string>> parseFolderId(const QString &id) {
  if (!id.startsWith(FOLDER_ID_PREFIX)) return std::nullopt;

  return id.mid(FOLDER_ID_PREFIX.size()).split(FOLDER_ID_SEPARATOR) |
         std::views::transform([](const QString &name) { return name.toStdString(); }) |
         std::ranges::to<std::vector>();
}

} // namespace

QUrl BrowserBookmarksViewHost::qmlComponentUrl() const { return qml::componentUrl(u"BrowserBookmarksView"); }

QUrl BrowserBookmarksViewHost::qmlSearchAccessoryUrl() const {
  return qml::componentUrl(u"BookmarkFolderFilterAccessory");
}

QVariantMap BrowserBookmarksViewHost::qmlProperties() {
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

void BrowserBookmarksViewHost::initialize() {
  BaseView::initialize();
  initModel();

  model()->addSource(&m_section);
  setSearchPlaceholderText(tr("Search bookmarks..."));

  m_detailVisible = command()->storage().getItem(DETAIL_STORAGE_KEY.toString()).toBool(false);
  m_section.setDetailVisible(m_detailVisible);
  m_section.setOnToggleDetail([this]() { toggleDetail(); });
  m_section.setOnSelectionChanged([this](const BrowserBookmark *bookmark) {
    m_selected = bookmark ? std::optional(*bookmark) : std::nullopt;
    emit detailChanged();
  });

  if (const auto saved = command()->storage().getItem(FOLDER_STORAGE_KEY.toString()); saved.isString()) {
    m_folderFilter = parseFolderId(saved.toString());
  }

  rebuildFolderFilter();

  auto browser = context()->services->browserExtension();
  connect(browser, &BrowserExtensionService::bookmarksChanged, this, &BrowserBookmarksViewHost::reload);
  connect(browser, &BrowserExtensionService::browsersChanged, this,
          &BrowserBookmarksViewHost::handleBrowsersChanged);
}

void BrowserBookmarksViewHost::loadInitialData() { handleBrowsersChanged(); }

BrowserBookmarksViewHost::Access BrowserBookmarksViewHost::computeAccess() const {
  const auto browsers = context()->services->browserExtension()->browsers();

  if (browsers.empty()) return Access::NoBrowser;

  if (std::ranges::any_of(browsers, [](auto &&b) {
        return b.hasPermission(BrowserExtensionService::BOOKMARKS_PERMISSION);
      })) {
    return Access::Granted;
  }

  if (std::ranges::any_of(browsers, [](auto &&b) { return b.supportsPermissions(); })) {
    return Access::NeedsPermission;
  }

  return Access::NeedsUpdate;
}

void BrowserBookmarksViewHost::handleBrowsersChanged() {
  if (const auto access = computeAccess(); access != m_access) {
    m_access = access;
    emit accessChanged();
  }

  if (m_access == Access::Granted) context()->services->browserExtension()->fetchBookmarks();

  reload();
}

void BrowserBookmarksViewHost::reload() {
  const auto browser = context()->services->browserExtension();
  const bool fetching = browser->isFetchingBookmarks();

  setLoading(fetching);

  if (m_access == Access::Granted) {
    auto bookmarks = browser->bookmarks();

    // keep the list in its loading state instead of flashing the empty view
    if (fetching && bookmarks.empty()) return;

    m_bookmarks = std::move(bookmarks);
  } else {
    m_bookmarks.clear();
  }

  rebuildFolderFilter();
  applyFolderFilter();

  if (m_access != Access::Granted) setActions(emptyActionPanel());
}

void BrowserBookmarksViewHost::rebuildFolderFilter() {
  std::vector<std::vector<std::string>> folders;
  std::set<std::vector<std::string>> seen;

  for (const auto &bookmark : m_bookmarks) {
    for (size_t depth = 1; depth <= bookmark.folder.size(); ++depth) {
      std::vector<std::string> prefix(bookmark.folder.begin(), bookmark.folder.begin() + depth);
      if (seen.insert(prefix).second) folders.emplace_back(std::move(prefix));
    }
  }

  QVariantList items;
  items.reserve(folders.size() + 1);
  items.emplace_back(QVariantMap{
      {QStringLiteral("id"), ALL_FOLDERS_ID.toString()},
      {QStringLiteral("displayName"), tr("All Bookmarks")},
      {QStringLiteral("iconSource"), qml::imageSourceFor(ImageURL(BuiltinIcon::Bookmark))},
  });

  const auto folderIcon = qml::imageSourceFor(ImageURL(BuiltinIcon::Folder));

  for (const auto &folder : folders) {
    items.emplace_back(QVariantMap{
        {QStringLiteral("id"), folderId(folder)},
        {QStringLiteral("displayName"),
         QString::fromStdString(folder | std::views::join_with(std::string_view{" / "}) |
                                std::ranges::to<std::string>())},
        {QStringLiteral("iconSource"), folderIcon},
    });
  }

  m_folderFilterModel.setItems(items);

  // the saved folder may be gone, but only trust that once bookmarks are actually loaded
  if (m_folderFilter && !m_bookmarks.empty() && !seen.contains(*m_folderFilter)) m_folderFilter.reset();

  emit currentFolderFilterChanged();
}

void BrowserBookmarksViewHost::applyFolderFilter() {
  if (!m_folderFilter) {
    m_section.setItems(m_bookmarks);
    return;
  }

  m_section.setItems(m_bookmarks | std::views::filter([&](const BrowserBookmark &b) {
                       return b.isInFolder(*m_folderFilter);
                     }) |
                     std::ranges::to<std::vector>());
}

void BrowserBookmarksViewHost::toggleDetail() {
  m_detailVisible = !m_detailVisible;
  m_section.setDetailVisible(m_detailVisible);
  command()->storage().setItem(DETAIL_STORAGE_KEY.toString(), m_detailVisible);
  model()->refreshActionPanel();
  emit detailChanged();
}

QVariantList BrowserBookmarksViewHost::detailMetadata() const {
  if (!m_selected) return {};

  QVariantList metadata;
  const QString url = QString::fromStdString(m_selected->url);

  metadata.emplace_back(QVariantMap{{QStringLiteral("label"), tr("Title")},
                                    {QStringLiteral("value"), QString::fromStdString(m_selected->title)}});
  metadata.emplace_back(QVariantMap{{QStringLiteral("type"), QStringLiteral("link")},
                                    {QStringLiteral("label"), tr("URL")},
                                    {QStringLiteral("value"), url},
                                    {QStringLiteral("url"), url}});

  if (!m_selected->folder.empty()) {
    metadata.emplace_back(QVariantMap{{QStringLiteral("label"), tr("Folder")},
                                      {QStringLiteral("value"), m_selected->folderPath()}});
  }

  if (m_selected->dateAdded > 0) {
    const auto added = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(m_selected->dateAdded));
    metadata.emplace_back(
        QVariantMap{{QStringLiteral("label"), tr("Added")},
                    {QStringLiteral("value"), QLocale().toString(added, QLocale::ShortFormat)}});
  }

  return metadata;
}

QString BrowserBookmarksViewHost::currentFolderFilter() const {
  return m_folderFilter ? folderId(*m_folderFilter) : ALL_FOLDERS_ID.toString();
}

void BrowserBookmarksViewHost::setFolderFilter(const QString &id) {
  auto filter = parseFolderId(id);

  if (filter == m_folderFilter) return;

  m_folderFilter = std::move(filter);
  command()->storage().setItem(FOLDER_STORAGE_KEY.toString(), id);
  emit currentFolderFilterChanged();
  applyFolderFilter();
}

QString BrowserBookmarksViewHost::emptyTitle() const {
  switch (m_access) {
  case Access::NeedsPermission:
    return tr("Bookmark access needed");
  case Access::NeedsUpdate:
    return tr("Browser extension update needed");
  case Access::NoBrowser:
    return tr("No browser connected");
  case Access::Granted:
    break;
  }
  return {};
}

QString BrowserBookmarksViewHost::emptyDescription() const {
  switch (m_access) {
  case Access::NeedsPermission:
    return tr("Allow the Vicinae browser extension to read your bookmarks. Your browser will ask you to "
              "confirm.");
  case Access::NeedsUpdate:
    return tr("Update the Vicinae browser extension to search your bookmarks.");
  case Access::NoBrowser:
    return tr("You need to connect at least one browser to vicinae using the browser extension in order to "
              "use this command.");
  case Access::Granted:
    break;
  }
  return {};
}

ImageUrl BrowserBookmarksViewHost::emptyIcon() const {
  switch (m_access) {
  case Access::NeedsPermission:
    return ImageUrl(ImageURL(BuiltinIcon::Lock).setFill(SemanticColor::Orange));
  case Access::NeedsUpdate:
    return ImageUrl(ImageURL(BuiltinIcon::Bookmark).setFill(SemanticColor::Orange));
  case Access::NoBrowser:
    return ImageUrl(ImageURL(BuiltinIcon::Link).setFill(SemanticColor::Red));
  case Access::Granted:
    break;
  }
  return {};
}

std::unique_ptr<ActionPanelState> BrowserBookmarksViewHost::emptyActionPanel() {
  if (m_access == Access::Granted) return nullptr;

  auto panel = std::make_unique<ListActionPanelState>();
  auto section = panel->createSection();

  if (m_access == Access::NeedsPermission) {
    const auto browsers = context()->services->browserExtension()->browsers();
    bool first = true;

    for (const auto &browser : browsers) {
      if (!browser.supportsPermissions() ||
          browser.hasPermission(BrowserExtensionService::BOOKMARKS_PERMISSION))
        continue;

      auto allow = new StaticAction(tr("Allow in %1").arg(QString::fromStdString(browser.name)),
                                    BuiltinIcon::Lock, [id = browser.id](ApplicationContext *ctx) {
                                      ctx->services->browserExtension()->requestPermission(
                                          id, BrowserExtensionService::BOOKMARKS_PERMISSION);
                                      ctx->navigation->showHud(tr("Check your browser to allow access"));
                                    });

      if (std::exchange(first, false)) allow->setShortcut(Keyboard::Shortcut::enter());
      section->addAction(allow);
    }

    return panel;
  }

  auto docs =
      new OpenInBrowserAction(QUrl("https://docs.vicinae.com/browser-extension"), tr("Open documentation"));
  docs->setShortcut(Keyboard::Shortcut::enter());
  section->addAction(docs);

  return panel;
}
