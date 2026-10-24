#include "builtins/browser/bookmark-tab-form-view-host.hpp"

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

  auto result = context()->services->browserExtension()->bookmarkTab(
      m_browserId, m_tabId, m_title.toStdString(), url.toStdString());
  if (!result) {
    context()->services->toastService()->failure(
        tr("Failed to bookmark tab: %1").arg(result.error().c_str()));
    return;
  }
  popSelf();
}
