#include "wm-extension.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "service-registry.hpp"
#include "services/window-manager/abstract-window-manager.hpp"
#include "services/window-manager/window-manager.hpp"
#include "services/toast/toast-service.hpp"
#include "builtins/wm/switch-windows-view-host.hpp"
#include "builtins/wm/switch-workspaces-view-host.hpp"
#include "ui/image/url.hpp"
#include "command/single-view-command-context.hpp"
#include "theme/theme.hpp"
#include <qcontainerfwd.h>
#include "theme/colors.hpp"
#include <QCoreApplication>
#include <QGuiApplication>
#include <QScreen>
#include <algorithm>

namespace {
const auto COLOR = SemanticColor::Blue;

enum class SnapPosition {
  LeftHalf,
  RightHalf,
  TopHalf,
  BottomHalf,
  Maximize,
  AlmostMaximize,
  Center,
  FirstThird,
  CenterThird,
  LastThird,
  FirstTwoThirds,
  LastTwoThirds
};

inline void snapActiveWindow(CommandController &ctrl, SnapPosition pos) {
  auto wm = ctrl.context()->services->windowManager();
  auto toast = ctrl.context()->services->toastService();
  auto provider = wm->provider();

  auto window = provider->getFocusedWindowSync();
  if (!window && provider->supportsFrontmostWindow()) { window = provider->getFrontmostWindowSync(); }

  if (!window) {
    toast->failure(QCoreApplication::translate("WindowManagement", "No active window found"));
    return;
  }

  auto screens = provider->listScreensSync();
  QRect screenBounds;
  for (const auto &s : screens) {
    if (s.active) {
      screenBounds = s.bounds;
      break;
    }
  }
  if (screenBounds.isEmpty() && !screens.empty()) { screenBounds = screens.front().bounds; }
  if (screenBounds.isEmpty()) {
    if (auto primary = QGuiApplication::primaryScreen()) { screenBounds = primary->geometry(); }
  }

  AbstractWindowManager::WindowBounds target;
  int w = screenBounds.width();
  int h = screenBounds.height();
  int sx = screenBounds.x();
  int sy = screenBounds.y();

  switch (pos) {
  case SnapPosition::LeftHalf:
    target = {sx, sy, w / 2, h};
    break;
  case SnapPosition::RightHalf:
    target = {sx + w / 2, sy, w - w / 2, h};
    break;
  case SnapPosition::TopHalf:
    target = {sx, sy, w, h / 2};
    break;
  case SnapPosition::BottomHalf:
    target = {sx, sy + h / 2, w, h - h / 2};
    break;
  case SnapPosition::Maximize:
    target = {sx, sy, w, h};
    break;
  case SnapPosition::AlmostMaximize: {
    int aw = static_cast<int>(w * 0.90);
    int ah = static_cast<int>(h * 0.90);
    target = {sx + (w - aw) / 2, sy + (h - ah) / 2, aw, ah};
    break;
  }
  case SnapPosition::Center: {
    auto b = window->bounds().value_or(AbstractWindowManager::WindowBounds{0, 0, w * 2 / 3, h * 2 / 3});
    int cw = std::min(b.width, w);
    int ch = std::min(b.height, h);
    target = {sx + (w - cw) / 2, sy + (h - ch) / 2, cw, ch};
    break;
  }
  case SnapPosition::FirstThird:
    target = {sx, sy, w / 3, h};
    break;
  case SnapPosition::CenterThird:
    target = {sx + w / 3, sy, w / 3, h};
    break;
  case SnapPosition::LastThird:
    target = {sx + 2 * (w / 3), sy, w - 2 * (w / 3), h};
    break;
  case SnapPosition::FirstTwoThirds:
    target = {sx, sy, 2 * (w / 3), h};
    break;
  case SnapPosition::LastTwoThirds:
    target = {sx + w / 3, sy, w - w / 3, h};
    break;
  }

  if (provider->setWindowBounds(*window, target)) {
    ctrl.context()->navigation->closeWindow();
  } else {
    toast->failure(
        QCoreApplication::translate("WindowManagement", "Window manager cannot resize this window"));
  }
}

class LeftHalfCommand : public BuiltinCallbackCommand {
  QString id() const override { return "left-half"; }
  QString name() const override { return QCoreApplication::translate("WindowManagement", "Left Half"); }
  std::vector<QString> keywords() const override { return {"left", "tile left", "split left", "snap"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::AppWindowSidebarLeft).setBackgroundTint(COLOR);
  }
  void execute(CommandController &ctrl) const override { snapActiveWindow(ctrl, SnapPosition::LeftHalf); }
};

class RightHalfCommand : public BuiltinCallbackCommand {
  QString id() const override { return "right-half"; }
  QString name() const override { return QCoreApplication::translate("WindowManagement", "Right Half"); }
  std::vector<QString> keywords() const override { return {"right", "tile right", "split right", "snap"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::AppWindowSidebarRight).setBackgroundTint(COLOR);
  }
  void execute(CommandController &ctrl) const override { snapActiveWindow(ctrl, SnapPosition::RightHalf); }
};

class TopHalfCommand : public BuiltinCallbackCommand {
  QString id() const override { return "top-half"; }
  QString name() const override { return QCoreApplication::translate("WindowManagement", "Top Half"); }
  std::vector<QString> keywords() const override { return {"top", "up", "tile top", "snap"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::ArrowUp).setBackgroundTint(COLOR);
  }
  void execute(CommandController &ctrl) const override { snapActiveWindow(ctrl, SnapPosition::TopHalf); }
};

class BottomHalfCommand : public BuiltinCallbackCommand {
  QString id() const override { return "bottom-half"; }
  QString name() const override { return QCoreApplication::translate("WindowManagement", "Bottom Half"); }
  std::vector<QString> keywords() const override { return {"bottom", "down", "tile bottom", "snap"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::ArrowDown).setBackgroundTint(COLOR);
  }
  void execute(CommandController &ctrl) const override { snapActiveWindow(ctrl, SnapPosition::BottomHalf); }
};

class MaximizeWindowCommand : public BuiltinCallbackCommand {
  QString id() const override { return "maximize"; }
  QString name() const override { return QCoreApplication::translate("WindowManagement", "Maximize"); }
  std::vector<QString> keywords() const override { return {"maximize", "full", "fill", "snap"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::ArrowsExpand).setBackgroundTint(COLOR);
  }
  void execute(CommandController &ctrl) const override { snapActiveWindow(ctrl, SnapPosition::Maximize); }
};

class AlmostMaximizeWindowCommand : public BuiltinCallbackCommand {
  QString id() const override { return "almost-maximize"; }
  QString name() const override { return QCoreApplication::translate("WindowManagement", "Almost Maximize"); }
  std::vector<QString> keywords() const override { return {"almost maximize", "centered", "snap"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::AppWindow).setBackgroundTint(COLOR);
  }
  void execute(CommandController &ctrl) const override {
    snapActiveWindow(ctrl, SnapPosition::AlmostMaximize);
  }
};

class CenterWindowCommand : public BuiltinCallbackCommand {
  QString id() const override { return "center"; }
  QString name() const override { return QCoreApplication::translate("WindowManagement", "Center"); }
  std::vector<QString> keywords() const override { return {"center", "middle", "snap"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::AlignCentre).setBackgroundTint(COLOR);
  }
  void execute(CommandController &ctrl) const override { snapActiveWindow(ctrl, SnapPosition::Center); }
};

class FirstThirdCommand : public BuiltinCallbackCommand {
  QString id() const override { return "first-third"; }
  QString name() const override { return QCoreApplication::translate("WindowManagement", "First Third"); }
  std::vector<QString> keywords() const override { return {"third", "left third", "snap"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::AppWindowList).setBackgroundTint(COLOR);
  }
  void execute(CommandController &ctrl) const override { snapActiveWindow(ctrl, SnapPosition::FirstThird); }
};

class CenterThirdCommand : public BuiltinCallbackCommand {
  QString id() const override { return "center-third"; }
  QString name() const override { return QCoreApplication::translate("WindowManagement", "Center Third"); }
  std::vector<QString> keywords() const override { return {"third", "center third", "middle third", "snap"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::AppWindowList).setBackgroundTint(COLOR);
  }
  void execute(CommandController &ctrl) const override { snapActiveWindow(ctrl, SnapPosition::CenterThird); }
};

class LastThirdCommand : public BuiltinCallbackCommand {
  QString id() const override { return "last-third"; }
  QString name() const override { return QCoreApplication::translate("WindowManagement", "Last Third"); }
  std::vector<QString> keywords() const override { return {"third", "right third", "snap"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::AppWindowList).setBackgroundTint(COLOR);
  }
  void execute(CommandController &ctrl) const override { snapActiveWindow(ctrl, SnapPosition::LastThird); }
};

class FirstTwoThirdsCommand : public BuiltinCallbackCommand {
  QString id() const override { return "first-two-thirds"; }
  QString name() const override {
    return QCoreApplication::translate("WindowManagement", "First Two Thirds");
  }
  std::vector<QString> keywords() const override { return {"two thirds", "left two thirds", "snap"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::AppWindowList).setBackgroundTint(COLOR);
  }
  void execute(CommandController &ctrl) const override {
    snapActiveWindow(ctrl, SnapPosition::FirstTwoThirds);
  }
};

class LastTwoThirdsCommand : public BuiltinCallbackCommand {
  QString id() const override { return "last-two-thirds"; }
  QString name() const override { return QCoreApplication::translate("WindowManagement", "Last Two Thirds"); }
  std::vector<QString> keywords() const override { return {"two thirds", "right two thirds", "snap"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::AppWindowList).setBackgroundTint(COLOR);
  }
  void execute(CommandController &ctrl) const override {
    snapActiveWindow(ctrl, SnapPosition::LastTwoThirds);
  }
};

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
    return ImageURL::builtin(BuiltinIcon::SwitchWindows).setBackgroundTint(COLOR);
  }
};

class SwitchWorkspacesCommand : public BuiltinViewCommand<SwitchWorkspacesViewHost> {
  QString id() const override { return "switch-workspaces"; }
  QString name() const override {
#ifdef Q_OS_WIN
    return QCoreApplication::translate("SwitchWorkspacesCommand", "Switch Desktops");
#else
    return QCoreApplication::translate("SwitchWorkspacesCommand", "Switch Workspaces");
#endif
  }
  std::vector<QString> keywords() const override {
    return {"workspaces", "desktops", "virtual desktops", "spaces"};
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Carousel).setBackgroundTint(COLOR);
  }
};
} // namespace

WindowManagementExtension::WindowManagementExtension(const ServiceRegistry &services) {
  using Cap = AbstractWindowManager::Capability;
  auto wm = services.windowManager()->provider();

  registerCommand<SwitchWindowsCommand>();

  registerCommand<LeftHalfCommand>();
  registerCommand<RightHalfCommand>();
  registerCommand<TopHalfCommand>();
  registerCommand<BottomHalfCommand>();
  registerCommand<MaximizeWindowCommand>();
  registerCommand<AlmostMaximizeWindowCommand>();
  registerCommand<CenterWindowCommand>();
  registerCommand<FirstThirdCommand>();
  registerCommand<CenterThirdCommand>();
  registerCommand<LastThirdCommand>();
  registerCommand<FirstTwoThirdsCommand>();
  registerCommand<LastTwoThirdsCommand>();

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
