#pragma once
#include "ui/action-pannel/action.hpp"
#include "ui/omni-list/omni-list.hpp"
#include "../src/ui/image/url.hpp"
#include "service-registry.hpp"
#include "services/window-manager/window-manager.hpp"
#include <chrono>
#include <functional>
#include <qfuturewatcher.h>
#include <qnamespace.h>
#include "ui/views/list-view.hpp"
#include "services/app-service/app-service.hpp"
#include <QTimer>

class FocusWindowAction : public AbstractAction {
  std::shared_ptr<AbstractWindowManager::AbstractWindow> _window;

  void execute(ApplicationContext *ctx) override {
    auto wm = ctx->services->windowManager();
    wm->provider()->focusWindowSync(*_window.get());
  }

public:
  FocusWindowAction(const std::shared_ptr<AbstractWindowManager::AbstractWindow> &window)
      : AbstractAction("Focus window", ImageURL::builtin("app-window")), _window(window) {}
};

class CloseWindowAction : public AbstractAction {
  std::shared_ptr<AbstractWindowManager::AbstractWindow> _window;
  std::function<void()> _refreshCallback;

  void execute(ApplicationContext *ctx) override {
    auto wm = ctx->services->windowManager();
    bool success = wm->provider()->closeWindow(*_window.get());

    // If the window was closed successfully and we have a refresh callback, trigger it
    if (success && _refreshCallback) { _refreshCallback(); }
  }

public:
  CloseWindowAction(const std::shared_ptr<AbstractWindowManager::AbstractWindow> &window,
                    std::function<void()> refreshCallback = nullptr)
      : AbstractAction("Close window", ImageURL::builtin("xmark")), _window(window),
        _refreshCallback(refreshCallback) {
    setStyle(AbstractAction::Danger);
  }
};

class WindowItem : public AbstractDefaultListItem, public ListView::Actionnable {
protected:
  std::shared_ptr<AbstractWindowManager::AbstractWindow> _window;
  std::function<void()> _refreshCallback;

  QString generateId() const override { return _window->id(); }

  // Helper method to generate workspace accessories
  AccessoryList generateWorkspaceAccessories() const {
    AccessoryList accessories;

    // Use abstract interface for workspace information
    if (auto workspace = _window->workspace()) {
      accessories.push_back(ListAccessory{.text = QString("WS %1").arg(*workspace + 1),
                                          .color = std::nullopt,
                                          .tooltip = QString("Workspace %1").arg(*workspace + 1),
                                          .fillBackground = false,
                                          .icon = std::nullopt});
    }

    return accessories;
  }

  virtual QList<AbstractAction *> generateActions() const override {
    QList<AbstractAction *> actions;
    auto focusAction = new FocusWindowAction(_window);
    focusAction->setPrimary(true);
    actions.append(focusAction);
    actions.append(new CloseWindowAction(_window, _refreshCallback));
    return actions;
  }

  virtual std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx) const override {
    auto state = std::make_unique<ActionPanelState>();
    auto focusAction = new FocusWindowAction(_window);
    focusAction->setPrimary(true);
    auto closeAction = new CloseWindowAction(_window, _refreshCallback);

    auto section = state->createSection("Window Actions");
    section->addAction(focusAction);
    section->addAction(closeAction);

    return state;
  }

public:
  WindowItem(const std::shared_ptr<AbstractWindowManager::AbstractWindow> &item,
             std::function<void()> refreshCallback = nullptr)
      : _window(item), _refreshCallback(refreshCallback) {}
};

class UnamedWindowListItem : public WindowItem {
  ItemData data() const override {
    return {.iconUrl = ImageURL::builtin("app-window"),
            .name = _window->title(),
            .subtitle = _window->wmClass(),
            .accessories = generateWorkspaceAccessories()};
  }

public:
  UnamedWindowListItem(const std::shared_ptr<AbstractWindowManager::AbstractWindow> &item,
                       std::function<void()> refreshCallback = nullptr)
      : WindowItem(item, refreshCallback) {}
};

class AppWindowListItem : public WindowItem {
  std::shared_ptr<Application> _app;

  ItemData data() const override {
    return {.iconUrl = _app->iconUrl(),
            .name = _window->title(),
            .subtitle = _app->name(),
            .accessories = generateWorkspaceAccessories()};
  }

public:
  AppWindowListItem(const std::shared_ptr<AbstractWindowManager::AbstractWindow> &item,
                    const std::shared_ptr<Application> &app, std::function<void()> refreshCallback = nullptr)
      : WindowItem(item, refreshCallback), _app(app) {}
};

class SwitchWindowsView : public ListView {
  AbstractWindowManager::WindowList windows;
  std::chrono::time_point<std::chrono::high_resolution_clock> m_lastWindowFetch =
      std::chrono::high_resolution_clock::now();

public:
  // Method to force refresh the windows list
  void refreshWindowsList() {
    // Force a refresh by clearing the cache
    windows.clear();
    m_lastWindowFetch = std::chrono::time_point<std::chrono::high_resolution_clock>{};

    // Refresh the current view
    textChanged(searchText());
  }

  // Method to refresh with a delay (for after window operations)
  void refreshWindowsListDelayed() {
    // Use a small delay to ensure the window manager has processed the close operation
    QTimer::singleShot(100, this, [this]() { refreshWindowsList(); });
  }

  void textChanged(const QString &s) override {
    auto wm = ServiceRegistry::instance()->windowManager();
    auto appDb = ServiceRegistry::instance()->appDb();
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(now - m_lastWindowFetch).count();

    if (windows.empty() || elapsedSeconds > 1) {
      windows = wm->listWindowsSync();
      m_lastWindowFetch = now;
    }

    m_list->beginResetModel();

    auto &section = m_list->addSection("Open Windows");

    // Create a refresh callback that this view can use
    auto refreshCallback = [this]() { refreshWindowsListDelayed(); };

    for (const auto &win : windows) {
      if (win->title().contains(s, Qt::CaseInsensitive)) {
        if (auto app = appDb->findByClass(win->wmClass())) {
          section.addItem(std::make_unique<AppWindowListItem>(win, app, refreshCallback));
        } else if (auto app = appDb->findById(win->wmClass())) {
          section.addItem(std::make_unique<AppWindowListItem>(win, app, refreshCallback));
        } else {
          section.addItem(std::make_unique<UnamedWindowListItem>(win, refreshCallback));
        }
      }
    }

    m_list->endResetModel(OmniList::SelectFirst);
  }

  void initialize() override {
    setSearchPlaceholderText("Search open window...");
    textChanged("");
  }

  SwitchWindowsView() {}
};
