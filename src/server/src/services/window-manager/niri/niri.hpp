#pragma once
#include "services/window-manager/abstract-wayland-window-manager.hpp"
#include "services/window-manager/abstract-window-manager.hpp"
#include <cstdint>
#include <optional>
#include <qsocketnotifier.h>
#include <qtimer.h>
#include <string>

namespace Niri {

using WindowHandle = std::uint64_t;
using WorkspaceHandle = std::uint64_t;

namespace ipc {

struct FocusTimestamp {
  std::int64_t secs = 0;
  std::int32_t nanos = 0;
};

struct Window;
struct Workspace;

} // namespace ipc

class Window : public AbstractWindowManager::AbstractWindow {
public:
  QString id() const override { return QString::number(m_id); }
  std::optional<int> pid() const override { return m_pid; }
  QString title() const override { return m_title; }
  QString wmClass() const override { return m_wmClass; }
  std::optional<QString> workspace() const override { return m_workspaceId; }
  bool canClose() const override { return true; }

  WindowHandle handle() const { return m_id; }
  bool isFocused() const { return m_focused; }
  void setFocused(bool focused) { m_focused = focused; }
  std::optional<ipc::FocusTimestamp> focusTimestamp() const { return m_focusTimestamp; }
  void setFocusTimestamp(std::optional<ipc::FocusTimestamp> value) { m_focusTimestamp = value; }
  void apply(const ipc::Window &window);

private:
  WindowHandle m_id = 0;
  QString m_title;
  QString m_wmClass;
  std::optional<int> m_pid;
  QString m_workspaceId;
  bool m_focused = false;
  std::optional<ipc::FocusTimestamp> m_focusTimestamp;
};

class Workspace : public AbstractWindowManager::AbstractWorkspace {
public:
  QString id() const override { return QString::number(m_id); }
  QString name() const override { return m_name; }
  std::optional<QString> monitor() const override { return m_monitor; }

  WorkspaceHandle handle() const { return m_id; }
  bool isActive() const { return m_active; }
  bool isFocused() const { return m_focused; }

  void setActive(bool active) { m_active = active; }
  void setFocused(bool focused) { m_focused = focused; }
  void apply(const ipc::Workspace &workspace);

private:
  WorkspaceHandle m_id = 0;
  QString m_name;
  QString m_monitor;
  bool m_active = false;
  bool m_focused = false;
};

class WindowManager : public AbstractWaylandWindowManager {
public:
  WindowManager();
  ~WindowManager() override;

  QString id() const override { return "niri"; }
  QString displayName() const override { return "Niri"; }

  WindowList listWindowsSync() const override;
  AbstractWindowManager::WindowPtr getFocusedWindowSync() const override;
  bool supportsFocusTracking() const override { return true; }
  bool supportsFocusHandoffDetection() const override { return true; }
  void focusWindowSync(const AbstractWindow &window) const override;
  void focusWorkspaceSync(const AbstractWorkspace &workspace) const override;
  bool closeWindow(const AbstractWindow &window) const override;
  bool toggleFullscreen(const AbstractWindow &window) override;
  bool toggleFloating(const AbstractWindow &window) override;

  bool hasWorkspaces() const override { return true; }
  WorkspaceList listWorkspaces() const override;
  WorkspacePtr getActiveWorkspace() const override;

  bool ping() const override;
  bool isActivatable() const override;
  void start() override;

  bool toggleOverview() override;

  QFlags<Capability> capabilities() const override {
    return {Capability::ToggleFloating, Capability::ToggleOverview, Capability::Fullscreen};
  }

private:
  bool connectEventStream();
  void handleEventSocketReadable();
  void drainEventBuffer();
  void processEventLine(const std::string &line);
  void scheduleWindowsChanged();

  void updateWindows(const std::vector<ipc::Window> &windows);
  void upsertWindow(const ipc::Window &windowData);
  void removeWindow(WindowHandle handle);
  void setFocusedWindow(std::optional<WindowHandle> handle);
  void setWindowFocusTimestamp(WindowHandle handle, std::optional<ipc::FocusTimestamp> focusTimestamp);
  void sortWindowsByFocusTimestamp();

  void updateWorkspaces(const std::vector<ipc::Workspace> &workspaces);
  void setActiveWorkspace(WorkspaceHandle handle, bool focused);

  static std::optional<WindowHandle> parseWindowHandle(const QString &id);

  WindowList m_windows;
  WorkspaceList m_workspaces;
  QSocketNotifier m_eventNotifier{QSocketNotifier::Type::Read};
  QTimer m_windowsChangedThrottle;
  std::string m_eventBuffer;
  bool m_pendingWindowsChanged = false;
  int m_eventFd = -1;
};

} // namespace Niri
