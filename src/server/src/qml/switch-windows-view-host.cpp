#include "switch-windows-view-host.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "services/window-manager/window-manager.hpp"

void SwitchWindowsViewHost::initialize() {
  BaseView::initialize();
  initModel();

  model()->addSource(&m_section);

  setSearchPlaceholderText(tr("Search open window..."));

  auto wm = context()->services->windowManager();
  wm->provider()->refresh();
  connect(wm, &WindowManager::windowsChanged, this, &SwitchWindowsViewHost::refreshWindows);
}

void SwitchWindowsViewHost::loadInitialData() { refreshWindows(); }

void SwitchWindowsViewHost::textChanged(const QString &text) { model()->setFilter(text); }

void SwitchWindowsViewHost::refreshWindows() {
  auto wm = ServiceRegistry::instance()->windowManager();
  auto appDb = ServiceRegistry::instance()->appDb();
  const auto &windows = wm->listWindows();

  std::vector<WindowEntry> entries;
  entries.reserve(windows.size());

  for (const auto &win : windows) {
    auto app = appDb->findByClass(win->wmClass());
    if (!app) app = appDb->findById(win->wmClass());

    QString workspaceName;
    if (auto ws = win->workspace()) {
      if (auto workspace = wm->findWorkspaceById(*ws); workspace && workspace->name() != workspace->id()) {
        workspaceName = workspace->name();
      }
    }

    entries.push_back({.window = win, .app = std::move(app), .workspaceName = std::move(workspaceName)});
  }

  m_section.setItems(std::move(entries));

  if (!searchText().isEmpty()) { model()->setFilter(searchText()); }
}
