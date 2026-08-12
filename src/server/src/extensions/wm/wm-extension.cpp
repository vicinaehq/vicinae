#include "wm-extension.hpp"
#include "builtin_icon.hpp"
#include "service-registry.hpp"
#include "services/window-manager/abstract-window-manager.hpp"
#include "services/window-manager/window-manager.hpp"
#include "services/toast/toast-service.hpp"
#include "switch-windows-view-host.hpp"
#include "switch-workspaces-view-host.hpp"
#include "ui/image/url.hpp"
#include "single-view-command-context.hpp"
#include "theme.hpp"
#include <qcontainerfwd.h>
#include "theme/colors.hpp"
#include <QCoreApplication>

namespace {
const auto COLOR = SemanticColor::Cyan;

class ToggleFullscreenWindowCommand : public BuiltinCallbackCommand {
  QString id() const override { return "toggle-fullscreen"; }
  QString name() const override {
    return QCoreApplication::translate("ToggleFullscreenWindowCommand", "Toggle Fullscreen");
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Fullscreen).setBackgroundTint(COLOR);
  }
  void execute(CommandController &ctrl) const override {
    auto wm = ctrl.context()->services->windowManager();
    auto toast = ctrl.context()->services->toastService();

    if (auto window = wm->provider()->getFocusedWindowSync()) {
      if (wm->isOnActiveWorkspace(*window)) {
        wm->provider()->toggleFullscreen(*window);
      } else {
        toast->failure(QCoreApplication::translate("ToggleFullscreenWindowCommand",
                                                   "Active window is not on the current workspace"));
      }
      ctrl.context()->navigation->closeWindow();
    } else {
      toast->failure(QCoreApplication::translate("ToggleFullscreenWindowCommand", "No window to fullscreen"));
    }
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
  void execute(CommandController &ctrl) const override {
    auto wm = ctrl.context()->services->windowManager();
    auto toast = ctrl.context()->services->toastService();

    if (auto window = wm->provider()->getFocusedWindowSync()) {
      if (wm->isOnActiveWorkspace(*window)) {
        wm->provider()->toggleFloating(*window);
      } else {
        toast->failure(QCoreApplication::translate("ToggleFloatingWindowCommand",
                                                   "Active window is not on the current workspace"));
      }
      ctrl.context()->navigation->closeWindow();
    } else {
      toast->failure(QCoreApplication::translate("ToggleFloatingWindowCommand", "No window to toggle"));
    }
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

class SwitchWindowsCommand : public BuiltinViewCommand<SwitchWindowsViewHost> {
  QString id() const override { return "switch-windows"; }
  QString name() const override {
    return QCoreApplication::translate("SwitchWindowsCommand", "Switch Windows");
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::SwitchWindows).setBackgroundTint(SemanticColor::Cyan);
  }
};

class SwitchWorkspacesCommand : public BuiltinViewCommand<SwitchWorkspacesViewHost> {
  QString id() const override { return "switch-workspaces"; }
  QString name() const override {
    return QCoreApplication::translate("SwitchWorkspacesCommand", "Switch Workspaces");
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Carousel).setBackgroundTint(SemanticColor::Cyan);
  }
};
} // namespace

WindowManagementExtension::WindowManagementExtension(const ServiceRegistry &services) {
  using Cap = AbstractWindowManager::Capability;
  auto wm = services.windowManager()->provider();

  registerCommand<SwitchWindowsCommand>();

  if (wm->hasWorkspaces()) { registerCommand<SwitchWorkspacesCommand>(); }
  if (wm->supports(Cap::Fullscreen)) { registerCommand<ToggleFullscreenWindowCommand>(); }
  if (wm->supports(Cap::ToggleFloating)) { registerCommand<ToggleFloatingWindowCommand>(); }
  if (wm->supports(Cap::ToggleOverview)) { registerCommand<ToggleOverviewCommand>(); }
  if (wm->supports(AbstractWindowManager::Capability::SetSticky)) {}
}

QString WindowManagementExtension::id() const { return "wm"; }

QString WindowManagementExtension::displayName() const {
  return QCoreApplication::translate("WindowManagementExtension", "Window Management");
}
ImageURL WindowManagementExtension::iconUrl() const {
  return ImageURL::builtin(BuiltinIcon::AppWindow).setBackgroundTint(COLOR);
}
