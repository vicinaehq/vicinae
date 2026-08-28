#include "services/window-manager/window-manager.hpp"
#include "services/app-service/app-service.hpp"
#include <algorithm>
#include <ranges>
#include "switch-workspaces-view-host.hpp"

void SwitchWorkspacesViewHost::initialize() {
  BaseView::initialize();
  initModel();

  model()->addSource(&m_section);
  setSearchPlaceholderText(tr("Search workspaces..."));

  auto wm = context()->services->windowManager();
  wm->provider()->refresh();
  connect(wm, &WindowManager::windowsChanged, this, &SwitchWorkspacesViewHost::refreshWindows);
}

void SwitchWorkspacesViewHost::loadInitialData() { refreshWindows(); }

void SwitchWorkspacesViewHost::textChanged(const QString &text) { model()->setFilter(text); }

void SwitchWorkspacesViewHost::refreshWindows() {
  auto wm = ServiceRegistry::instance()->windowManager();
  auto appDb = ServiceRegistry::instance()->appDb();
  auto windows = wm->listWindows();
  auto screens = wm->provider()->listScreensSync();
  auto winfos =
      wm->provider()->listWorkspaces() | std::views::transform([&](auto &&workspace) {
        WorkspaceInfo info{.workspace = workspace};
        auto windows = wm->provider()->listWorkspaceWindows(workspace->id());

        info.apps.reserve(windows.size());
        info.windowCount = windows.size();

        qDebug() << "workspace monitor" << workspace->monitor();

        if (auto it = std::ranges::find_if(screens, [&](auto &&s) { return s.name == workspace->monitor(); });
            it != screens.end()) {
          info.screen = *it;
        }

        for (const auto &win : windows) {
          if (auto app = appDb->findById(win->wmClass()); app && !std::ranges::contains(info.apps, app)) {
            info.apps.emplace_back(std::move(app));
          }
        }

        return info;
      }) |
      std::ranges::to<std::vector>();

  m_section.setItems(winfos);

  if (!searchText().isEmpty()) { model()->setFilter(searchText()); }
}
