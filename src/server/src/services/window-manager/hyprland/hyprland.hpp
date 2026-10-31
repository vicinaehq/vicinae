#pragma once
#include "services/window-manager/abstract-wayland-window-manager.hpp"
#include "services/window-manager/hyprland/hypr-listener.hpp"

namespace Hyprland::ipc {
struct Window;
}

class HyprlandWindow : public AbstractWindowManager::AbstractWindow {
public:
  QString id() const override { return m_id; }
  std::optional<int> pid() const override {
    if (m_pid <= 0) return std::nullopt;
    return m_pid;
  }
  QString title() const override { return m_title; }
  QString wmClass() const override { return m_wmClass; }
  std::optional<AbstractWindowManager::WindowBounds> bounds() const override { return m_bounds; }
  std::optional<QString> workspace() const override { return QString::number(m_workspaceId); }
  bool canClose() const override { return true; }

  HyprlandWindow(const Hyprland::ipc::Window &window);

private:
  QString m_id;
  QString m_title;
  QString m_wmClass;
  int m_workspaceId;
  int m_pid = 0;
  AbstractWindowManager::WindowBounds m_bounds;
};

class HyprlandWindowManager : public AbstractWaylandWindowManager {
public:
  HyprlandWindowManager();

  WindowList listWindowsSync() const override;
  AbstractWindowManager::WindowPtr getFocusedWindowSync() const override;
  AbstractWindowManager::WindowPtr getFrontmostWindowSync() const override;
  bool supportsFrontmostWindow() const override { return true; }
  bool supportsFocusTracking() const override { return true; }

  WorkspacePtr getActiveWorkspace() const override;

  void focusWindowSync(const AbstractWindow &window) const override;
  void focusWorkspaceSync(const AbstractWorkspace &workspace) const override;
  bool closeWindow(const AbstractWindow &window) const override;
  bool toggleFullscreen(const AbstractWindow &window) override;
  bool toggleFloating(const AbstractWindow &window) override;
  bool isActivatable() const override;
  bool hasWorkspaces() const override;
  WorkspaceList listWorkspaces() const override;

  QFlags<Capability> capabilities() const override {
    return {Capability::ToggleFloating, Capability::Fullscreen};
  }

  bool ping() const override;
  void start() override;

private:
  QString id() const override;
  QString displayName() const override;

  Hyprland::EventListener m_ev;
};
