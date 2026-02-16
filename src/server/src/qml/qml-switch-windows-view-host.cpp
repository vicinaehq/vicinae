#include "qml-switch-windows-view-host.hpp"
#include "qml-switch-windows-model.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "services/window-manager/window-manager.hpp"
#include <QTimer>

QUrl QmlSwitchWindowsViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/CommandListView.qml"));
}

QVariantMap QmlSwitchWindowsViewHost::qmlProperties() const {
  return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(m_model))}};
}

void QmlSwitchWindowsViewHost::initialize() {
  BaseView::initialize();

  m_model = new QmlSwitchWindowsModel(this);
  m_model->initialize(context());

  setSearchPlaceholderText("Search open window...");

  auto wm = context()->services->windowManager();
  connect(wm->provider(), &AbstractWindowManager::windowsChanged, this, [this]() {
    QTimer::singleShot(100, this, &QmlSwitchWindowsViewHost::refreshWindows);
  });
}

void QmlSwitchWindowsViewHost::loadInitialData() { refreshWindows(); }

void QmlSwitchWindowsViewHost::textChanged(const QString &text) {
  // Re-fetch windows if stale (>1s)
  auto now = std::chrono::steady_clock::now();
  if (std::chrono::duration_cast<std::chrono::seconds>(now - m_lastFetch).count() > 1) {
    refreshWindows();
    return; // refreshWindows calls setItems which re-applies the current filter
  }
  m_model->setFilter(text);
}

void QmlSwitchWindowsViewHost::onReactivated() { m_model->refreshActionPanel(); }

void QmlSwitchWindowsViewHost::beforePop() { m_model->beforePop(); }

QObject *QmlSwitchWindowsViewHost::listModel() const { return m_model; }

void QmlSwitchWindowsViewHost::refreshWindows() {
  auto wm = ServiceRegistry::instance()->windowManager();
  auto appDb = ServiceRegistry::instance()->appDb();
  auto windows = wm->listWindowsSync();

  std::vector<WindowEntry> entries;
  entries.reserve(windows.size());

  for (auto &win : windows) {
    auto app = appDb->findByClass(win->wmClass());
    if (!app) app = appDb->findById(win->wmClass());
    entries.push_back({.window = std::move(win), .app = std::move(app)});
  }

  m_lastFetch = std::chrono::steady_clock::now();
  m_model->setItems(std::move(entries));

  // Re-apply current search text
  if (!searchText().isEmpty()) {
    m_model->setFilter(searchText());
  }
}
