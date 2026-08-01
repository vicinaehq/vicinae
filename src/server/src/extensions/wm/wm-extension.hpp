#pragma once
#include "builtin_icon.hpp"
#include "command-database.hpp"
#include "service-registry.hpp"
#include "services/window-manager/abstract-window-manager.hpp"
#include "services/window-manager/window-manager.hpp"
#include "services/toast/toast-service.hpp"
#include "../../ui/image/url.hpp"
#include "single-view-command-context.hpp"
#include "theme.hpp"
#include <qcontainerfwd.h>
#include "switch-windows-command.hpp"
#include "theme/colors.hpp"
#include <QCoreApplication>

namespace {
const auto COLOR = SemanticColor::Cyan;
};

class ToggleFullscreenWindowCommand : public BuiltinCallbackCommand {
  QString id() const override { return "toggle-fullscreen"; }
  QString name() const override {
    return QCoreApplication::translate("ToggleFullscreenWindowCommand", "Toggle Fullscreen");
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Fullscreen).setBackgroundTint(COLOR);
  }
  void execute(CommandController &controller) const override {
    auto wm = controller.context()->services->windowManager();

    if (auto window = wm->provider()->getFocusedWindowSync()) {
      wm->provider()->toggleFullscreen(*window);
    } else {
      controller.context()->services->toastService()->failure("No window to fullscreen");
    }

    controller.context()->navigation->closeWindow();
  }
};

class ToggleFloatingWindowCommand : public BuiltinCallbackCommand {
  QString id() const override { return "toggle-floating"; }
  QString name() const override {
    return QCoreApplication::translate("ToggleFloatingWindowCommand", "Toggle Floating");
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::FloatingWindow).setBackgroundTint(COLOR);
  }
  void execute(CommandController &controller) const override {
    auto wm = controller.context()->services->windowManager();

    if (auto window = wm->provider()->getFocusedWindowSync()) {
      wm->provider()->toggleFloating(*window);
    } else {
      controller.context()->services->toastService()->failure("No active window");
    }

    controller.context()->navigation->closeWindow();
  }
};

class ToggleOverviewCommand : public BuiltinCallbackCommand {
  QString id() const override { return "toggle-overview"; }
  QString name() const override {
    return QCoreApplication::translate("ToggleOverviewCommand", "Toggle Overview");
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Overview).setBackgroundTint(COLOR);
  }
  void execute(CommandController &controller) const override {
    auto wm = controller.context()->services->windowManager();
    wm->provider()->toggleOverview();
    controller.context()->navigation->closeWindow();
  }
};

class WindowManagementExtension : public BuiltinCommandRepository {
  QString id() const override { return "wm"; }
  QString displayName() const override {
    return QCoreApplication::translate("WindowManagementExtension", "Window Management");
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::AppWindowList).setBackgroundTint(COLOR);
  }

public:
  WindowManagementExtension(const ServiceRegistry &services) {
    using Cap = AbstractWindowManager::Capability;
    auto wm = services.windowManager()->provider();

    registerCommand<SwitchWindowsCommand>();

    if (wm->supports(Cap::Fullscreen)) { registerCommand<ToggleFullscreenWindowCommand>(); }
    if (wm->supports(Cap::ToggleFloating)) { registerCommand<ToggleFloatingWindowCommand>(); }
    if (wm->supports(Cap::ToggleOverview)) { registerCommand<ToggleOverviewCommand>(); }
    if (wm->supports(AbstractWindowManager::Capability::SetSticky)) {}
  }
};
