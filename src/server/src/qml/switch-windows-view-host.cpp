#include "switch-windows-view-host.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "services/window-manager/window-manager.hpp"
#include <QTimer>

void SwitchWindowsViewHost::initialize() {
  BaseView::initialize();
  initModel();

  model()->addSource(&m_section);

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
  model()->setFilter(text);
}

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
  m_section.setItems(std::move(entries));

  if (!searchText().isEmpty()) { model()->setFilter(searchText()); }
}
