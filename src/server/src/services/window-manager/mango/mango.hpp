#pragma once
#include "services/window-manager/abstract-wayland-window-manager.hpp"
#include <cstdint>
#include <optional>
#include <qsocketnotifier.h>
#include <string>
#include <utility>

namespace Mango {

namespace ipc {
struct Client;
}

class Window : public AbstractWindowManager::AbstractWindow {
public:
  QString id() const override { return QString::number(m_id); }
  std::optional<int> pid() const override { return m_pid; }
  QString title() const override { return m_title; }
  QString wmClass() const override { return m_wmClass; }
  std::optional<QString> workspace() const override { return m_workspace; }
  std::optional<AbstractWindowManager::WindowBounds> bounds() const override { return m_bounds; }
  bool fullScreen() const override { return m_fullScreen; }
  bool sticky() const override { return m_sticky; }

  void apply(const ipc::Client &client);

private:
  std::uint32_t m_id = 0;
  QString m_title;
  QString m_wmClass;
  std::optional<int> m_pid;
  std::optional<QString> m_workspace;
  AbstractWindowManager::WindowBounds m_bounds;
  bool m_fullScreen = false;
  bool m_sticky = false;
};

class Workspace : public AbstractWindowManager::AbstractWorkspace {
public:
  QString id() const override { return m_id; }
  QString name() const override { return m_name; }
  std::optional<QString> monitor() const override { return m_monitor; }

  Workspace(QString id, QString name, QString monitor)
      : m_id(std::move(id)), m_name(std::move(name)), m_monitor(std::move(monitor)) {}

private:
  QString m_id;
  QString m_name;
  QString m_monitor;
};

class WindowManager : public AbstractWaylandWindowManager {
public:
  WindowManager();
  ~WindowManager() override;

  QString id() const override { return "mango"; }
  QString displayName() const override { return "Mango"; }

  WindowList listWindowsSync() const override;
  AbstractWindowManager::WindowPtr getFocusedWindowSync() const override;
  bool supportsFocusTracking() const override { return true; }
  bool supportsFocusHandoffDetection() const override { return true; }
  void focusWindowSync(const AbstractWindow &window) const override;
  bool closeWindow(const AbstractWindow &window) const override;
  bool toggleFullscreen(const AbstractWindow &window) override;
  bool toggleFloating(const AbstractWindow &window) override;
  bool toggleOverview() override;

  bool supportsSetSticky() const override { return true; }
  bool setSticky(const AbstractWindow &window, bool sticky) const override;

  bool hasWorkspaces() const override { return true; }
  WorkspaceList listWorkspaces() const override;
  WorkspacePtr getActiveWorkspace() const override;
  void focusWorkspaceSync(const AbstractWorkspace &workspace) const override;
  bool supportsMoveToWorkspace() const override { return true; }
  bool moveToWorkspace(const AbstractWindow &window, const QString &workspaceId) const override;
  bool supportsWorkspaceActivation() const override { return true; }
  bool activateWorkspace(const QString &workspaceId) const override;

  QFlags<Capability> capabilities() const override {
    return {Capability::Fullscreen, Capability::ToggleFloating, Capability::ToggleOverview,
            Capability::SetSticky};
  }

  bool ping() const override;
  bool isActivatable() const override;
  void start() override;

private:
  void handleClientsReadable();
  void handleFocusReadable();

  QSocketNotifier m_clientsNotifier{QSocketNotifier::Type::Read};
  QSocketNotifier m_focusNotifier{QSocketNotifier::Type::Read};
  int m_clientsFd = -1;
  int m_focusFd = -1;
};

} // namespace Mango
