#pragma once
#include "ui/action-pannel/action.hpp"
#include "ui/omni-list/omni-list.hpp"
#include "../src/ui/image/url.hpp"
#include "service-registry.hpp"
#include "services/window-manager/window-manager.hpp"
#include <chrono>
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
      : AbstractAction("Focus window", ImageURL::builtin("app-window")), _window(window) {
    setAutoClose();
  }
};

class CloseWindowAction : public AbstractAction {
  std::shared_ptr<AbstractWindowManager::AbstractWindow> _window;

  void execute(ApplicationContext *ctx) override {
    auto wm = ctx->services->windowManager();
    bool success = wm->provider()->closeWindow(*_window.get());
  }

public:
  CloseWindowAction(const std::shared_ptr<AbstractWindowManager::AbstractWindow> &window)
      : AbstractAction("Close window", ImageURL::builtin("xmark")), _window(window) {
    setStyle(AbstractAction::Style::Danger);
  }
};

class WindowItem : public AbstractDefaultListItem, public ListView::Actionnable {
protected:
  std::shared_ptr<AbstractWindowManager::AbstractWindow> _window;

  QString generateId() const override { return _window->id(); }

  // Helper method to generate workspace accessories
  AccessoryList generateWorkspaceAccessories() const {
    AccessoryList accessories;

    // Use abstract interface for workspace information
    if (auto workspace = _window->workspace()) {
      accessories.push_back(ListAccessory{.text = QString("WS %1").arg(*workspace),
                                          .color = std::nullopt,
                                          .tooltip = QString("Workspace %1").arg(*workspace),
                                          .fillBackground = false,
                                          .icon = std::nullopt});
    }

    return accessories;
  }

  virtual std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx) const override {
    auto state = std::make_unique<ListActionPanelState>();
    auto focusAction = new FocusWindowAction(_window);
    auto closeAction = new CloseWindowAction(_window);

    closeAction->setShortcut(Keyboard::Shortcut(Qt::Key_Q, Qt::ControlModifier));

    auto section = state->createSection("Window Actions");
    section->addAction(focusAction);
    section->addAction(closeAction);

    return state;
  }

public:
  WindowItem(const std::shared_ptr<AbstractWindowManager::AbstractWindow> &item) : _window(item) {}
};

class UnamedWindowListItem : public WindowItem {
  ItemData data() const override {
    return {.iconUrl = ImageURL::builtin("app-window"),
            .name = _window->title(),
            .subtitle = _window->wmClass(),
            .accessories = generateWorkspaceAccessories()};
  }

public:
  UnamedWindowListItem(const std::shared_ptr<AbstractWindowManager::AbstractWindow> &item)
      : WindowItem(item) {}
};

class AppWindowListItem : public WindowItem {
  std::shared_ptr<AbstractApplication> _app;

  ItemData data() const override {
    return {.iconUrl = _app->iconUrl(),
            .name = _window->title(),
            .subtitle = _app->displayName(),
            .accessories = generateWorkspaceAccessories()};
  }

public:
  AppWindowListItem(const std::shared_ptr<AbstractWindowManager::AbstractWindow> &item,
                    const std::shared_ptr<AbstractApplication> &app)
      : WindowItem(item), _app(app) {}
};

class SwitchWindowsView : public ListView {
  AbstractWindowManager::WindowList windows;
  std::chrono::time_point<std::chrono::high_resolution_clock> m_lastWindowFetch =
      std::chrono::high_resolution_clock::now();

public:
  void refreshWindowsList() {
    // Force a refresh by clearing the cache
    windows.clear();
    m_lastWindowFetch = std::chrono::time_point<std::chrono::high_resolution_clock>{};
    textChanged(searchText());
  }

  // Method to refresh with a delay (for after window operations)
  // Uses a small delay to ensure the window manager has processed the close operation
  void refreshWindowsListDelayed() {
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

    for (const auto &win : windows) {
      auto app = appDb->findByClass(win->wmClass());

      if (!app) { app = appDb->findById(win->wmClass()); }

      bool appMatches = app && app->displayName().contains(s, Qt::CaseInsensitive);
      bool matches = appMatches || win->title().contains(s, Qt::CaseInsensitive) ||
                     win->wmClass().contains(s, Qt::CaseInsensitive);

      if (!matches) continue;

      if (app) {
        section.addItem(std::make_unique<AppWindowListItem>(win, app));
      } else {
        section.addItem(std::make_unique<UnamedWindowListItem>(win));
      }
    }

    m_list->endResetModel(OmniList::SelectFirst);
  }

  void initialize() override {
    auto wm = context()->services->windowManager();

    connect(wm->provider(), &AbstractWindowManager::windowsChanged, this,
            &SwitchWindowsView::refreshWindowsListDelayed);

    setSearchPlaceholderText("Search open window...");
    textChanged("");
  }

  SwitchWindowsView() {}
};
