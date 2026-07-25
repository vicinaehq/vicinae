#pragma once
#include "builtin_icon.hpp"
#include "command-database.hpp"
#include "services/window-manager/window-manager.hpp"
#include "services/toast/toast-service.hpp"
#include "../../ui/image/url.hpp"
#include "single-view-command-context.hpp"
#include "theme.hpp"
#include <qcontainerfwd.h>
#include "switch-windows-command.hpp"
#include <QCoreApplication>

class ToggleFullscreenWindowCommand : public BuiltinCallbackCommand {
  QString id() const override { return "toggle-fullscreen"; }
  QString name() const override {
    return QCoreApplication::translate("ToggleFullscreenWindowCommand", "Toggle Fullscreen");
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::ArrowsExpand).setBackgroundTint(SemanticColor::Cyan);
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
    return ImageURL::builtin(BuiltinIcon::AppWindow).setBackgroundTint(SemanticColor::Cyan);
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

class WindowManagementExtension : public BuiltinCommandRepository {
  QString id() const override { return "wm"; }
  QString displayName() const override {
    return QCoreApplication::translate("WindowManagementExtension", "Window Management");
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::AppWindowList).setBackgroundTint(SemanticColor::Cyan);
  }

public:
  WindowManagementExtension() {
    registerCommand<SwitchWindowsCommand>();
    registerCommand<ToggleFullscreenWindowCommand>();
    registerCommand<ToggleFloatingWindowCommand>();
  }
};
