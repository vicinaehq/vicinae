#include <array>
#include <QCoreApplication>
#include <qcontainerfwd.h>
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
#include "theme/colors.hpp"

namespace {
const auto COLOR = SemanticColor::Blue;

struct LayoutCommandInfo {
  WindowLayout::Kind kind;
  const char *id;
  const char *name;
  BuiltinIcon icon;
};

using Layout = WindowLayout::Kind;
constexpr auto LAYOUT_COMMANDS = std::to_array<LayoutCommandInfo>({
    {Layout::LeftHalf, "left-half", QT_TRANSLATE_NOOP("WindowLayoutCommand", "Left Half"),
     BuiltinIcon::AppWindowSidebarLeft},
    {Layout::RightHalf, "right-half", QT_TRANSLATE_NOOP("WindowLayoutCommand", "Right Half"),
     BuiltinIcon::AppWindowSidebarRight},
    {Layout::TopHalf, "top-half", QT_TRANSLATE_NOOP("WindowLayoutCommand", "Top Half"), BuiltinIcon::ArrowUp},
    {Layout::BottomHalf, "bottom-half", QT_TRANSLATE_NOOP("WindowLayoutCommand", "Bottom Half"),
     BuiltinIcon::ArrowDown},
    {Layout::TopLeftQuarter, "top-left-quarter", QT_TRANSLATE_NOOP("WindowLayoutCommand", "Top Left Quarter"),
     BuiltinIcon::AppWindowGrid2x2},
    {Layout::TopRightQuarter, "top-right-quarter",
     QT_TRANSLATE_NOOP("WindowLayoutCommand", "Top Right Quarter"), BuiltinIcon::AppWindowGrid2x2},
    {Layout::BottomLeftQuarter, "bottom-left-quarter",
     QT_TRANSLATE_NOOP("WindowLayoutCommand", "Bottom Left Quarter"), BuiltinIcon::AppWindowGrid2x2},
    {Layout::BottomRightQuarter, "bottom-right-quarter",
     QT_TRANSLATE_NOOP("WindowLayoutCommand", "Bottom Right Quarter"), BuiltinIcon::AppWindowGrid2x2},
    {Layout::FirstThird, "first-third", QT_TRANSLATE_NOOP("WindowLayoutCommand", "First Third"),
     BuiltinIcon::AppWindowSidebarLeft},
    {Layout::CenterThird, "center-third", QT_TRANSLATE_NOOP("WindowLayoutCommand", "Center Third"),
     BuiltinIcon::Center},
    {Layout::LastThird, "last-third", QT_TRANSLATE_NOOP("WindowLayoutCommand", "Last Third"),
     BuiltinIcon::AppWindowSidebarRight},
    {Layout::FirstTwoThirds, "first-two-thirds", QT_TRANSLATE_NOOP("WindowLayoutCommand", "First Two Thirds"),
     BuiltinIcon::AppWindowSidebarLeft},
    {Layout::LastTwoThirds, "last-two-thirds", QT_TRANSLATE_NOOP("WindowLayoutCommand", "Last Two Thirds"),
     BuiltinIcon::AppWindowSidebarRight},
    {Layout::Center, "center", QT_TRANSLATE_NOOP("WindowLayoutCommand", "Center"), BuiltinIcon::Center},
    {Layout::Maximize, "maximize", QT_TRANSLATE_NOOP("WindowLayoutCommand", "Maximize"),
     BuiltinIcon::Maximize},
    {Layout::AlmostMaximize, "almost-maximize", QT_TRANSLATE_NOOP("WindowLayoutCommand", "Almost Maximize"),
     BuiltinIcon::Maximize},
    {Layout::MakeSmaller, "make-smaller", QT_TRANSLATE_NOOP("WindowLayoutCommand", "Make Smaller"),
     BuiltinIcon::Minimize},
    {Layout::MakeLarger, "make-larger", QT_TRANSLATE_NOOP("WindowLayoutCommand", "Make Larger"),
     BuiltinIcon::Maximize},
    {Layout::NextDisplay, "move-to-next-display",
     QT_TRANSLATE_NOOP("WindowLayoutCommand", "Move to Next Display"), BuiltinIcon::ArrowRight},
    {Layout::PreviousDisplay, "move-to-previous-display",
     QT_TRANSLATE_NOOP("WindowLayoutCommand", "Move to Previous Display"), BuiltinIcon::ArrowLeft},
    {Layout::Restore, "restore", QT_TRANSLATE_NOOP("WindowLayoutCommand", "Restore"), BuiltinIcon::Undo},
});

class WindowLayoutCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(WindowLayoutCommand)

public:
  explicit WindowLayoutCommand(LayoutCommandInfo info) : m_info(info) {}

  QString id() const override { return QString::fromLatin1(m_info.id); }
  QString name() const override { return tr(m_info.name); }
  QString description() const override {
    switch (m_info.kind) {
    case Layout::Center:
      return tr("Center the active window without resizing it.");
    case Layout::AlmostMaximize:
      return tr("Center the active window at 90% of the usable screen size.");
    case Layout::MakeSmaller:
      return tr("Shrink the active window by 10% of the usable screen size.");
    case Layout::MakeLarger:
      return tr("Enlarge the active window by 10% of the usable screen size.");
    case Layout::NextDisplay:
      return tr("Move the active window to the next display, keeping its relative size and position.");
    case Layout::PreviousDisplay:
      return tr("Move the active window to the previous display, keeping its relative size and position.");
    case Layout::Restore:
      return tr("Restore the size and position before the last window management command.");
    default:
      return tr("Move and resize the active window on its current display.");
    }
  }
  std::vector<QString> keywords() const override {
    switch (m_info.kind) {
    case Layout::FirstThird:
    case Layout::FirstTwoThirds:
      return {"window", "resize", "left"};
    case Layout::LastThird:
    case Layout::LastTwoThirds:
      return {"window", "resize", "right"};
    case Layout::CenterThird:
      return {"window", "resize", "middle third"};
    case Layout::NextDisplay:
    case Layout::PreviousDisplay:
      return {"window", "move", "monitor", "screen"};
    default:
      return {"window", "resize", "move"};
    }
  }
  ImageURL iconUrl() const override { return ImageURL::builtin(m_info.icon).setBackgroundTint(COLOR); }

  void execute(CommandController &ctrl) const override {
    auto wm = ctrl.context()->services->windowManager();
    auto toast = ctrl.context()->services->toastService();
    auto window = wm->getFocusedWindow();
    if (!window) {
      toast->failure(tr("No active window"));
      return;
    }
    if (!wm->isOnActiveWorkspace(*window)) {
      toast->failure(tr("Active window is not on the current workspace"));
      return;
    }
    switch (wm->applyLayout(*window, m_info.kind)) {
    case WindowLayout::Result::Success:
      ctrl.context()->navigation->closeWindow();
      break;
    case WindowLayout::Result::NoBounds:
      toast->failure(tr("Could not read the window's position and size"));
      break;
    case WindowLayout::Result::NoScreen:
      toast->failure(tr("No available display"));
      break;
    case WindowLayout::Result::NoOtherDisplay:
      toast->failure(tr("No other display connected"));
      break;
    case WindowLayout::Result::Fullscreen:
      toast->failure(tr("Exit fullscreen before moving or resizing this window"));
      break;
    case WindowLayout::Result::NothingToRestore:
      toast->failure(tr("No previous window size to restore"));
      break;
    case WindowLayout::Result::Failed:
      toast->failure(tr("Could not move or resize this window"));
      break;
    }
  }

private:
  LayoutCommandInfo m_info;
};

class MinimizeWindowCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(MinimizeWindowCommand)

public:
  QString id() const override { return "minimize"; }
  QString name() const override { return tr("Minimize Window"); }
  QString description() const override { return tr("Minimize the active window."); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Minimize).setBackgroundTint(COLOR);
  }
  void execute(CommandController &ctrl) const override {
    auto wm = ctrl.context()->services->windowManager();
    auto toast = ctrl.context()->services->toastService();
    auto window = wm->getFocusedWindow();
    if (!window) {
      toast->failure(tr("No active window"));
      return;
    }
    if (!wm->isOnActiveWorkspace(*window)) {
      toast->failure(tr("Active window is not on the current workspace"));
      return;
    }
    if (!wm->provider()->minimizeWindow(*window)) {
      toast->failure(tr("Could not minimize this window"));
      return;
    }
    ctrl.context()->navigation->closeWindow();
  }
};

class AdjacentWorkspaceCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(AdjacentWorkspaceCommand)
  using Direction = AbstractWindowManager::Direction;

public:
  enum class Operation { MoveWindow, Switch };

  AdjacentWorkspaceCommand(Direction direction, Operation operation)
      : m_direction(direction), m_operation(operation) {}
  QString id() const override {
    if (m_operation == Operation::Switch)
      return m_direction == Direction::Next ? "switch-to-next-workspace" : "switch-to-previous-workspace";
    return m_direction == Direction::Next ? "move-to-next-workspace" : "move-to-previous-workspace";
  }
  QString name() const override {
#ifdef Q_OS_MACOS
    if (m_operation == Operation::Switch)
      return m_direction == Direction::Next ? tr("Switch to Next Space") : tr("Switch to Previous Space");
    return m_direction == Direction::Next ? tr("Move to Next Space") : tr("Move to Previous Space");
#else
    if (m_operation == Operation::Switch)
      return m_direction == Direction::Next ? tr("Switch to Next Workspace")
                                            : tr("Switch to Previous Workspace");
    return m_direction == Direction::Next ? tr("Move to Next Workspace") : tr("Move to Previous Workspace");
#endif
  }
  QString description() const override {
#ifdef Q_OS_MACOS
    if (m_operation == Operation::Switch)
      return tr("Switch to the adjacent Space without moving any windows.");
    return tr("Move the active window to the adjacent desktop Space and follow it.");
#else
    if (m_operation == Operation::Switch)
      return tr("Switch to the adjacent workspace without moving any windows.");
    return tr("Move the active window to the adjacent workspace and follow it.");
#endif
  }
  std::vector<QString> keywords() const override { return {"desktop", "workspace", "space"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(m_direction == Direction::Next ? BuiltinIcon::ArrowRight
                                                            : BuiltinIcon::ArrowLeft)
        .setBackgroundTint(COLOR);
  }
  void execute(CommandController &ctrl) const override {
    auto wm = ctrl.context()->services->windowManager();
    auto toast = ctrl.context()->services->toastService();
    auto navigation = ctrl.context()->navigation.get();
    const bool moveWindow = m_operation == Operation::MoveWindow;
    auto window = moveWindow ? wm->getFocusedWindow() : nullptr;
    if (moveWindow && !window) {
      toast->failure(tr("No active window"));
      return;
    }
    if (window && !wm->isOnActiveWorkspace(*window)) {
      toast->failure(tr("Active window is not on the current workspace"));
      return;
    }
    toast->dynamic(moveWindow ? tr("Moving window...") : tr("Switching..."));
    auto future = moveWindow ? wm->provider()->moveToAdjacentWorkspace(*window, m_direction)
                             : wm->provider()->switchToAdjacentWorkspace(m_direction);
    future.then(navigation, [toast, navigation, wm, window, moveWindow,
                             direction = m_direction](AbstractWindowManager::WorkspaceChangeResult result) {
      using Result = AbstractWindowManager::WorkspaceChangeResult;
      switch (result) {
      case Result::Success:
        toast->clear();
        navigation->closeWindow();
        if (window) wm->provider()->focusWindowSync(*window);
        break;
      case Result::NoAdjacentWorkspace:
#ifdef Q_OS_MACOS
        toast->failure(direction == Direction::Next ? tr("No next Space on this display")
                                                    : tr("No previous Space on this display"));
#else
            toast->failure(direction == Direction::Next ? tr("No next workspace") : tr("No previous workspace"));
#endif
        break;
      case Result::Unsupported:
#ifdef Q_OS_MACOS
        toast->failure(moveWindow ? tr("This window cannot be moved between Spaces")
                                  : tr("Switching Spaces is unavailable"));
#else
            toast->failure(moveWindow ? tr("This window cannot be moved between workspaces")
                                      : tr("Switching workspaces is unavailable"));
#endif
        break;
      case Result::Failed:
#ifdef Q_OS_MACOS
        toast->failure(moveWindow ? tr("Could not move this window") : tr("Could not switch Spaces"));
#else
            toast->failure(moveWindow ? tr("Could not move this window") : tr("Could not switch workspaces"));
#endif
        break;
      case Result::FollowFailed:
#ifdef Q_OS_MACOS
        toast->failure(tr("Window moved, but could not switch to its Space"));
#else
            toast->failure(tr("Window moved, but could not switch to its workspace"));
#endif
        break;
      case Result::PermissionRequired:
        toast->failure(tr("Enable Accessibility access for Vicinae in System Settings"));
        break;
      case Result::Busy:
        break;
      }
    });
  }

private:
  Direction m_direction;
  Operation m_operation;
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

  if (wm->supports(Cap::WindowPlacement)) {
    for (const auto &info : LAYOUT_COMMANDS)
      registerCommand<WindowLayoutCommand>(info);
  }

  if (wm->hasWorkspaces()) { registerCommand<SwitchWorkspacesCommand>(); }
  if (wm->supports(Cap::Minimize)) { registerCommand<MinimizeWindowCommand>(); }
  if (wm->supports(Cap::MoveToAdjacentWorkspace)) {
    registerCommand<AdjacentWorkspaceCommand>(AbstractWindowManager::Direction::Next,
                                              AdjacentWorkspaceCommand::Operation::MoveWindow);
    registerCommand<AdjacentWorkspaceCommand>(AbstractWindowManager::Direction::Previous,
                                              AdjacentWorkspaceCommand::Operation::MoveWindow);
  }
  if (wm->supports(Cap::SwitchToAdjacentWorkspace)) {
    registerCommand<AdjacentWorkspaceCommand>(AbstractWindowManager::Direction::Next,
                                              AdjacentWorkspaceCommand::Operation::Switch);
    registerCommand<AdjacentWorkspaceCommand>(AbstractWindowManager::Direction::Previous,
                                              AdjacentWorkspaceCommand::Operation::Switch);
  }
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
