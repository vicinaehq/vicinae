#include "switch-windows-view-host.hpp"
#include "switch-windows-model.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "services/window-manager/window-manager.hpp"
#include <QTimer>

QUrl SwitchWindowsViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/CommandListView.qml"));
}

QVariantMap SwitchWindowsViewHost::qmlProperties() {
  return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(m_model))}};
}

void SwitchWindowsViewHost::initialize() {
  BaseView::initialize();

  m_model = new SwitchWindowsModel(this);
  m_model->setScope(ViewScope(context(), this));
  m_model->initialize();

  setSearchPlaceholderText("Search open window...");

  auto wm = context()->services->windowManager();
  connect(wm->provider(), &AbstractWindowManager::windowsChanged, this,
          [this]() { QTimer::singleShot(100, this, &SwitchWindowsViewHost::refreshWindows); });
}

void SwitchWindowsViewHost::loadInitialData() { refreshWindows(); }

void SwitchWindowsViewHost::textChanged(const QString &text) {
  auto now = std::chrono::steady_clock::now();
  if (std::chrono::duration_cast<std::chrono::seconds>(now - m_lastFetch).count() > 1) {
    refreshWindows();
    return;
  }
  m_model->setFilter(text);
}

void SwitchWindowsViewHost::onReactivated() { m_model->refreshActionPanel(); }

void SwitchWindowsViewHost::beforePop() { m_model->beforePop(); }

QObject *SwitchWindowsViewHost::listModel() const { return m_model; }

void SwitchWindowsViewHost::refreshWindows() {
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

  if (!searchText().isEmpty()) { m_model->setFilter(searchText()); }
}
