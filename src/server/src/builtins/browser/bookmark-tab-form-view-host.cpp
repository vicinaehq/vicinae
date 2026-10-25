#include "builtins/browser/bookmark-tab-form-view-host.hpp"

#include <algorithm>

#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/action-panel/action.hpp"
#include "ui/action-panel/action-panel-state.hpp"
#include "ui/views/view-utils.hpp"

BookmarkTabFormViewHost::BookmarkTabFormViewHost(const BrowserExtensionService::BrowserTab &tab)
    : m_browserId(tab.browserId), m_tabId(tab.id), m_title(QString::fromStdString(tab.title)),
      m_url(QString::fromStdString(tab.url)) {}

QUrl BookmarkTabFormViewHost::qmlComponentUrl() const { return qml::componentUrl(u"BookmarkTabFormView"); }

QVariantMap BookmarkTabFormViewHost::qmlProperties() {
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

void BookmarkTabFormViewHost::initialize() {
  BaseView::initialize();
  setNavigationTitle(tr("Bookmark tab"));
  setNavigationIcon(BuiltinIcon::Bookmark);

  auto panel = std::make_unique<FormActionPanelState>();
  panel->createSection()->addAction(
      new StaticAction(tr("Save bookmark"), BuiltinIcon::Bookmark, [this]() { submit(); }));
  setActions(std::move(panel));

  auto *browser = context()->services->browserExtension();
  const auto fetchFolders = [this, browser]() {
    refreshFolders();
    const auto browsers = browser->browsers();
    const auto it =
        std::ranges::find_if(browsers, [this](const auto &info) { return info.id == m_browserId; });
    if (it != browsers.end() && it->hasPermission(BrowserExtensionService::BOOKMARKS_PERMISSION))
      browser->fetchBookmarks(m_browserId);
  };
  connect(browser, &BrowserExtensionService::bookmarksChanged, this,
          &BookmarkTabFormViewHost::refreshFolders);
  connect(browser, &BrowserExtensionService::browsersChanged, this, fetchFolders);
  fetchFolders();

  const auto browsers = browser->browsers();
  const auto it = std::ranges::find_if(browsers, [this](const auto &info) { return info.id == m_browserId; });
  if (it != browsers.end() && it->supportsPermissions() &&
      !it->hasPermission(BrowserExtensionService::BOOKMARKS_PERMISSION)) {
    browser->requestPermission(m_browserId, BrowserExtensionService::BOOKMARKS_PERMISSION);
    context()->navigation->showHud(tr("Check your browser to allow access"));
  }
}

void BookmarkTabFormViewHost::refreshFolders() {
  auto *service = context()->services->browserExtension();
  const auto browsers = service->browsers();
  const auto it = std::ranges::find_if(browsers, [this](const auto &info) { return info.id == m_browserId; });
  QVariantList items{
      QVariantMap{{QStringLiteral("id"), QString()}, {QStringLiteral("displayName"), tr("Browser default")}}};
  if (it != browsers.end()) {
    for (const auto &folder : it->folders) {
      QStringList parts;
      parts.reserve(folder.path.size());
      for (const auto &part : folder.path)
        parts.append(QString::fromStdString(part));
      items.append(QVariantMap{{QStringLiteral("id"), QString::fromStdString(folder.id)},
                               {QStringLiteral("displayName"), parts.join(QStringLiteral(" / "))}});
    }
  }
  m_folderModel.setItems(items);
  if (m_folderModel.itemDataById(m_folderId).isEmpty()) m_folderId.clear();
  emit folderChanged();
}

void BookmarkTabFormViewHost::selectFolder(const QVariantMap &item) {
  const auto id = item.value(QStringLiteral("id")).toString();
  if (m_folderModel.itemDataById(id).isEmpty()) return;
  m_folderId = id;
  emit folderChanged();
}

void BookmarkTabFormViewHost::submit() {
  const QString url = m_url.trimmed();
  const QUrl parsed(url);
  if (!parsed.isValid() || parsed.scheme().isEmpty()) {
    m_urlError = tr("Enter a valid URL");
    emit errorsChanged();
    return;
  }

  m_urlError.clear();
  emit errorsChanged();

  const auto browsers = context()->services->browserExtension()->browsers();
  const auto browser =
      std::ranges::find_if(browsers, [this](const auto &info) { return info.id == m_browserId; });
  if (browser != browsers.end() && browser->supportsPermissions() &&
      !browser->hasPermission(BrowserExtensionService::BOOKMARKS_PERMISSION)) {
    context()->services->toastService()->failure(tr("Allow bookmark access in your browser first"));
    return;
  }

  const std::optional<std::string> folderId =
      m_folderId.isEmpty() ? std::nullopt : std::optional(m_folderId.toStdString());
  auto result = context()->services->browserExtension()->bookmarkTab(
      m_browserId, m_tabId, m_title.toStdString(), url.toStdString(), folderId);
  if (!result) {
    context()->services->toastService()->failure(
        tr("Failed to bookmark tab: %1").arg(result.error().c_str()));
    return;
  }
  popSelf();
}
