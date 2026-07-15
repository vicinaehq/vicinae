#pragma once
#include "services/window-manager/abstract-window-manager.hpp"
#include <QString>
#include <windows.h>

namespace Win {

/**
 * A top-level Win32 window. The HWND is kept as a live handle for focus/close/resize operations;
 * all other properties are snapshotted at enumeration time since the list is refetched frequently.
 */
class Window : public AbstractWindowManager::AbstractWindow {
public:
  Window(HWND hwnd, QString title, QString wmClass, int pid,
         std::optional<AbstractWindowManager::WindowBounds> bounds, std::optional<QString> workspace)
      : m_hwnd(hwnd), m_title(std::move(title)), m_wmClass(std::move(wmClass)), m_pid(pid), m_bounds(bounds),
        m_workspace(std::move(workspace)) {}

  QString id() const override { return QString::number(reinterpret_cast<quintptr>(m_hwnd)); }
  QString title() const override { return m_title; }
  QString wmClass() const override { return m_wmClass; }
  std::optional<int> pid() const override { return m_pid; }
  std::optional<AbstractWindowManager::WindowBounds> bounds() const override { return m_bounds; }
  std::optional<QString> workspace() const override { return m_workspace; }

  HWND hwnd() const { return m_hwnd; }

private:
  HWND m_hwnd;
  QString m_title;
  QString m_wmClass;
  int m_pid;
  std::optional<AbstractWindowManager::WindowBounds> m_bounds;
  std::optional<QString> m_workspace;
};

} // namespace Win
