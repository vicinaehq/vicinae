#pragma once
#include "service-registry.hpp"
#include "services/window-manager/window-manager.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/image/url.hpp"
#include <QTimer>

class FocusWindowAction : public AbstractAction {
  std::shared_ptr<AbstractWindowManager::AbstractWindow> m_window;

  void execute(ApplicationContext *ctx) override {
    auto wm = ctx->services->windowManager();
    auto window = m_window;
    QTimer::singleShot(0, [wm, window]() { wm->provider()->focusWindowSync(*window.get()); });
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
    wm->provider()->closeWindow(*m_window.get());
  }

public:
  CloseWindowAction(const std::shared_ptr<AbstractWindowManager::AbstractWindow> &window)
      : AbstractAction("Close window", ImageURL::builtin("xmark")), m_window(window) {
    setStyle(AbstractAction::Style::Danger);
  }
};
