#include "app.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include "ui/declarative-omni-list-view.hpp"
#include "wm/window-manager.hpp"
#include "ui/action-pannel/move-to-workspace-action.hpp"
#include <chrono>
#include <qfuturewatcher.h>
#include <qnamespace.h>

class FocusWindowAction : public AbstractAction {
  std::shared_ptr<AbstractWindowManager::Window> _window;

  void execute(AppWindow &app) override {
    auto wm = ServiceRegistry::instance()->windowManager();

    wm->focusWindowSync(*_window.get());
  }

public:
  FocusWindowAction(const std::shared_ptr<AbstractWindowManager::Window> &window)
      : AbstractAction("Focus window", BuiltinOmniIconUrl("app-window")), _window(window) {}
};

class WindowItem : public AbstractDefaultListItem, public DeclarativeOmniListView::IActionnable {
protected:
  std::shared_ptr<AbstractWindowManager::Window> _window;

  QString id() const override { return _window->id(); }

  virtual QList<AbstractAction *> generateActions() const override {
    return {new FocusWindowAction(_window)};
  }

public:
  WindowItem(const std::shared_ptr<AbstractWindowManager::Window> &item) : _window(item) {}
};

class UnamedWindowListItem : public WindowItem {
  ItemData data() const override {
    return {.iconUrl = BuiltinOmniIconUrl("app-window"),
            .name = _window->title(),
            .category = _window->wmClass()};
  }

public:
  UnamedWindowListItem(const std::shared_ptr<AbstractWindowManager::Window> &item) : WindowItem(item) {}
};

class AppWindowListItem : public WindowItem {
  std::shared_ptr<Application> _app;

  ItemData data() const override {
    return {.iconUrl = _app->iconUrl(), .name = _window->title(), .category = _app->name()};
  }

public:
  AppWindowListItem(const std::shared_ptr<AbstractWindowManager::Window> &item,
                    const std::shared_ptr<Application> &app)
      : WindowItem(item), _app(app) {}
};

class SwitchWindowsCommand : public DeclarativeOmniListView {
  AbstractWindowManager::WindowList windows;
  QFutureWatcher<AbstractWindowManager::WindowList> watcher;
  std::chrono::time_point<std::chrono::high_resolution_clock> m_lastWindowFetch =
      std::chrono::high_resolution_clock::now();

  void handleResolvedWindowList() {
    windows = watcher.result();
    auto items = generateList(searchText());

    list->updateFromList(items, OmniList::SelectFirst);
  }

public:
  SwitchWindowsCommand(AppWindow &app) : DeclarativeOmniListView(app) {}

  ItemList generateList(const QString &s) override {
    auto wm = ServiceRegistry::instance()->windowManager();
    auto appDb = ServiceRegistry::instance()->appDb();

    ItemList list;
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(now - m_lastWindowFetch).count();

    if (windows.empty() || elapsedSeconds > 1) {
      windows = wm->listWindowsSync();
      m_lastWindowFetch = now;
    }

    list.reserve(windows.size());
    list.push_back(std::make_unique<OmniList::VirtualSection>("Open windows"));

    for (const auto &win : windows) {
      if (win->title().contains(s, Qt::CaseInsensitive)) {
        if (auto app = appDb->findByClass(win->wmClass())) {
          list.push_back(std::make_unique<AppWindowListItem>(win, app));
        } else if (auto app = appDb->findById(win->wmClass())) {
          list.push_back(std::make_unique<AppWindowListItem>(win, app));
        } else {
          list.push_back(std::make_unique<UnamedWindowListItem>(win));
        }
      }
    }

    return list;
  }

  void onMount() override {
    DeclarativeOmniListView::onMount();
    setSearchPlaceholderText("Search open window...");
  }
};
