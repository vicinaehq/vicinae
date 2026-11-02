#include <QDateTime>
#include <QLocale>
#include <utility>
#include "builtins/browser/browser-history-view-host.hpp"
#include "actions/app-actions.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "theme/colors.hpp"
#include "ui/action-panel/action.hpp"
#include "ui/views/view-utils.hpp"

namespace {

constexpr QStringView DETAIL_STORAGE_KEY = u"showDetail";

}

QUrl BrowserHistoryViewHost::qmlComponentUrl() const { return qml::componentUrl(u"BrowserHistoryView"); }

QVariantMap BrowserHistoryViewHost::qmlProperties() {
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

void BrowserHistoryViewHost::initialize() {
  BaseView::initialize();
  initModel();

  model()->addSource(&m_section);
  setSearchPlaceholderText(tr("Search history..."));

  m_detailVisible = command()->storage().getItem(DETAIL_STORAGE_KEY.toString()).toBool(false);
  m_section.setDetailVisible(m_detailVisible);
  m_section.setOnToggleDetail([this]() { toggleDetail(); });
  m_section.setOnSelectionChanged([this](const BrowserHistoryItem *item) {
    m_selected = item ? std::optional(*item) : std::nullopt;
    emit detailChanged();
  });

  m_searchThrottle.setSingleShot(true);
  m_searchThrottle.setInterval(SEARCH_THROTTLE);
  connect(&m_searchThrottle, &QTimer::timeout, this, [this]() {
    if (std::exchange(m_searchPending, false)) search();
  });

  auto browser = context()->services->browserExtension();
  connect(browser, &BrowserExtensionService::historyChanged, this, &BrowserHistoryViewHost::reload);
  connect(browser, &BrowserExtensionService::browsersChanged, this,
          &BrowserHistoryViewHost::handleBrowsersChanged);
}

void BrowserHistoryViewHost::loadInitialData() { handleBrowsersChanged(); }

void BrowserHistoryViewHost::textChanged(const QString &text) {
  m_query = text.toStdString();

  if (m_searchThrottle.isActive()) {
    m_searchPending = true;
    return;
  }

  search();
}

void BrowserHistoryViewHost::beforePop() {
  ListViewHost::beforePop();
  m_searchThrottle.stop();
  m_searchPending = false;
  context()->services->browserExtension()->clearHistoryResults();
}

void BrowserHistoryViewHost::handleBrowsersChanged() {
  const auto access =
      context()->services->browserExtension()->permissionAccess(BrowserExtensionService::HISTORY_PERMISSION);

  if (access != m_access) {
    m_access = access;
    emit accessChanged();
  }

  if (m_access == Access::Granted) search();

  reload();
}

void BrowserHistoryViewHost::search() {
  m_searchedQuery = m_query;
  m_searchThrottle.start();
  context()->services->browserExtension()->searchHistory(m_searchedQuery);
}

void BrowserHistoryViewHost::reload() {
  const auto browser = context()->services->browserExtension();
  const bool fetching = browser->isFetchingHistory();

  setLoading(fetching);

  if (m_access != Access::Granted) {
    m_section.setItems({});
    setActions(emptyActionPanel());
    return;
  }

  auto items = browser->history();

  if (fetching && items.empty()) return;

  m_section.setTruncated(browser->isHistoryTruncated());

  if (m_searchedQuery == m_appliedQuery) {
    m_section.setItems(std::move(items));
    return;
  }

  m_appliedQuery = m_searchedQuery;
  m_section.setItems(std::move(items), false);
  ListViewHost::textChanged(QString::fromStdString(m_appliedQuery));
}

void BrowserHistoryViewHost::toggleDetail() {
  m_detailVisible = !m_detailVisible;
  m_section.setDetailVisible(m_detailVisible);
  command()->storage().setItem(DETAIL_STORAGE_KEY.toString(), m_detailVisible);
  model()->refreshActionPanel();
  emit detailChanged();
}

QVariantList BrowserHistoryViewHost::detailMetadata() const {
  if (!m_selected) return {};

  QVariantList metadata;
  const QString url = QString::fromStdString(m_selected->url);

  metadata.emplace_back(QVariantMap{{QStringLiteral("label"), tr("Title")},
                                    {QStringLiteral("value"), QString::fromStdString(m_selected->title)}});
  metadata.emplace_back(QVariantMap{{QStringLiteral("type"), QStringLiteral("link")},
                                    {QStringLiteral("label"), tr("URL")},
                                    {QStringLiteral("value"), url},
                                    {QStringLiteral("url"), url}});

  if (m_selected->lastVisitTime > 0) {
    const auto visited = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(m_selected->lastVisitTime));
    metadata.emplace_back(
        QVariantMap{{QStringLiteral("label"), tr("Last Visited")},
                    {QStringLiteral("value"), QLocale().toString(visited, QLocale::ShortFormat)}});
  }

  metadata.emplace_back(QVariantMap{{QStringLiteral("label"), tr("Visits")},
                                    {QStringLiteral("value"), QLocale().toString(m_selected->visitCount)}});

  return metadata;
}

QString BrowserHistoryViewHost::emptyTitle() const {
  switch (m_access) {
  case Access::NeedsPermission:
    return tr("History access needed");
  case Access::NeedsUpdate:
    return tr("Browser extension update needed");
  case Access::NoBrowser:
    return tr("No browser connected");
  case Access::Granted:
    break;
  }
  return {};
}

QString BrowserHistoryViewHost::emptyDescription() const {
  switch (m_access) {
  case Access::NeedsPermission:
    return tr("Allow the Vicinae browser extension to read your browsing history. Your browser will ask you "
              "to confirm.");
  case Access::NeedsUpdate:
    return tr("Update the Vicinae browser extension to search your browsing history.");
  case Access::NoBrowser:
    return tr("You need to connect at least one browser to vicinae using the browser extension in order to "
              "use this command.");
  case Access::Granted:
    break;
  }
  return {};
}

ImageUrl BrowserHistoryViewHost::emptyIcon() const {
  switch (m_access) {
  case Access::NeedsPermission:
    return ImageUrl(ImageURL(BuiltinIcon::Lock).setFill(SemanticColor::Orange));
  case Access::NeedsUpdate:
    return ImageUrl(ImageURL(BuiltinIcon::Clock).setFill(SemanticColor::Orange));
  case Access::NoBrowser:
    return ImageUrl(ImageURL(BuiltinIcon::Link).setFill(SemanticColor::Red));
  case Access::Granted:
    break;
  }
  return {};
}

std::unique_ptr<ActionPanelState> BrowserHistoryViewHost::emptyActionPanel() {
  if (m_access == Access::Granted) return nullptr;

  auto panel = std::make_unique<ListActionPanelState>();
  auto section = panel->createSection();

  if (m_access == Access::NeedsPermission) {
    bool first = true;

    for (const auto &browser : context()->services->browserExtension()->browsers()) {
      if (!browser.canRequestPermission(BrowserExtensionService::HISTORY_PERMISSION) ||
          browser.hasPermission(BrowserExtensionService::HISTORY_PERMISSION))
        continue;

      auto allow = new StaticAction(
          tr("Allow in %1").arg(QString::fromStdString(browser.name)), BuiltinIcon::Lock,
          [id = browser.id](ApplicationContext *ctx) {
            ctx->services->browserExtension()
                ->requestPermission(id, BrowserExtensionService::HISTORY_PERMISSION)
                .then(ctx->navigation.get(), [ctx](const std::expected<void, std::string> &result) {
                  if (!result) {
                    ctx->services->toastService()->failure(
                        tr("Failed to request access: %1").arg(QString::fromStdString(result.error())));
                    return;
                  }
                  ctx->navigation->showHud(tr("Check your browser to allow access"));
                });
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
