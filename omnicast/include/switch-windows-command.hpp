#include "app.hpp"
#include "omni-icon.hpp"
#include "ui/action_popover.hpp"
#include "ui/declarative-omni-list-view.hpp"
#include "wm/hyprland/hyprland.hpp"
#include "wm/window-manager.hpp"
#include <qfuturewatcher.h>
#include <qnamespace.h>

class FocusWindowAction : public AbstractAction {
  std::shared_ptr<AbstractWindowManager::Window> _window;
  AbstractWindowManager *wm = new HyprlandWindowManager;

  void execute(AppWindow &app) override { wm->focus(*_window.get()); }

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
            .category = _window->wmClass(),
            .kind = {}};
  }

public:
  UnamedWindowListItem(const std::shared_ptr<AbstractWindowManager::Window> &item) : WindowItem(item) {}
};

class AppWindowListItem : public WindowItem {
  std::shared_ptr<DesktopExecutable> _app;

  ItemData data() const override {
    return {.iconUrl = _app->iconUrl(), .name = _window->title(), .category = _app->name, .kind = {}};
  }

public:
  AppWindowListItem(const std::shared_ptr<AbstractWindowManager::Window> &item,
                    const std::shared_ptr<DesktopExecutable> &app)
      : WindowItem(item), _app(app) {}
};

class SwitchWindowsCommand : public DeclarativeOmniListView {
  AbstractWindowManager::WindowList windows;
  QFutureWatcher<AbstractWindowManager::WindowList> watcher;
  AbstractWindowManager *wm = new HyprlandWindowManager;
  Service<AppDatabase> appDb;

  void handleResolvedWindowList() {
    windows = watcher.result();
    auto items = generateList(searchText());

    list->updateFromList(items, OmniList::SelectFirst);
  }

public:
  SwitchWindowsCommand(AppWindow &app) : DeclarativeOmniListView(app), appDb(service<AppDatabase>()) {}

  void onSearchChanged(const QString &text) override {
    if (watcher.isRunning()) {
      watcher.cancel();
      watcher.waitForFinished();
    }

    watcher.setFuture(wm->listWindows());
  }

  ItemList generateList(const QString &s) override {
    ItemList list;

    list.push_back(std::make_unique<OmniList::VirtualSection>("Open windows"));

    for (const auto &win : windows) {
      if (win->title().contains(s, Qt::CaseInsensitive)) {
        if (auto app = appDb.findByWMClass(win->wmClass())) {
          list.push_back(std::make_unique<AppWindowListItem>(win, app));
        } else if (auto app = appDb.findById(win->wmClass())) {
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
    connect(&watcher, &QFutureWatcher<AbstractWindowManager::WindowList>::finished, this,
            &SwitchWindowsCommand::handleResolvedWindowList);
  }
};
