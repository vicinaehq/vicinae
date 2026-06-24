#pragma once
#include "service-registry.hpp"
#include "services/window-manager/window-manager.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/image/url.hpp"

class FocusWindowAction : public AbstractAction {
  std::shared_ptr<AbstractWindowManager::AbstractWindow> m_window;

  void execute(ApplicationContext *ctx) override {
    auto wm = ctx->services->windowManager();
    wm->provider()->focusWindowSync(*m_window);
  }

public:
  FocusWindowAction(const std::shared_ptr<AbstractWindowManager::AbstractWindow> &window)
      : AbstractAction("Focus window", ImageURL::builtin("app-window")), m_window(window) {
    setAutoClose();
  }
};

class CloseWindowAction : public AbstractAction {
  std::shared_ptr<AbstractWindowManager::AbstractWindow> m_window;

  void execute(ApplicationContext *ctx) override {
    auto wm = ctx->services->windowManager();
    wm->provider()->closeWindow(*m_window);
  }

public:
  CloseWindowAction(const std::shared_ptr<AbstractWindowManager::AbstractWindow> &window)
      : AbstractAction("Close window", ImageURL::builtin("xmark")), m_window(window) {
    setStyle(AbstractAction::Style::Danger);
  }
};

class PinWindowAction : public AbstractAction {
  std::shared_ptr<AbstractWindowManager::AbstractWindow> m_window;

  void execute(ApplicationContext *ctx) override {
    auto provider = ctx->services->windowManager()->provider();
    bool currentlySticky = m_window->sticky();
    provider->setSticky(*m_window, !currentlySticky);
  }

public:
  PinWindowAction(const std::shared_ptr<AbstractWindowManager::AbstractWindow> &window)
      : AbstractAction(window->sticky() ? "Unpin from all workspaces" : "Pin to all workspaces",
                       ImageURL::builtin(window->sticky() ? "pin-disabled" : "pin")),
        m_window(window) {}
};

class BringToWorkspaceAction : public AbstractAction {
  std::shared_ptr<AbstractWindowManager::AbstractWindow> m_window;

  void execute(ApplicationContext *ctx) override {
    auto provider = ctx->services->windowManager()->provider();
    auto activeWs = provider->getActiveWorkspace();
    if (!activeWs) return;
    provider->moveToWorkspace(*m_window, activeWs->id());
  }

public:
  BringToWorkspaceAction(const std::shared_ptr<AbstractWindowManager::AbstractWindow> &window)
      : AbstractAction("Bring to current workspace", ImageURL::builtin("move")), m_window(window) {
    setAutoClose();
  }
};
