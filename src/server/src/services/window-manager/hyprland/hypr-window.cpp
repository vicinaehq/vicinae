#include "hypr-ipc.hpp"
#include "hyprland.hpp"

HyprlandWindow::HyprlandWindow(const Hyprland::ipc::Window &window) {
  m_id = QString::fromStdString(window.address);
  m_title = QString::fromStdString(window.title);
  m_wmClass = QString::fromStdString(window.wmClass);
  m_workspaceId = window.workspace.id;
  m_pid = window.pid;

  m_bounds.x = window.at[0];
  m_bounds.y = window.at[1];
  m_bounds.width = window.size[0];
  m_bounds.height = window.size[1];
}
