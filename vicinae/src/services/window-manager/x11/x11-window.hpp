#pragma once
#include "services/window-manager/abstract-window-manager.hpp"
#include <xcb/xcb.h>
#include <QString>
#include <optional>

/**
 * Represents a window in an X11 window manager.
 * Uses XCB (X C Bindings) to query window properties following the EWMH specification.
 */
class X11Window : public AbstractWindowManager::AbstractWindow {
public:
  /**
   * Constructor that queries all window properties.
   * @param connection XCB connection to X server
   * @param window X11 window ID (XID)
   */
  X11Window(xcb_connection_t *connection, xcb_window_t window);

  // AbstractWindow interface implementation
  QString id() const override { return m_id; }
  QString title() const override { return m_title; }
  QString wmClass() const override { return m_wmClass; }
  std::optional<int> pid() const override { return m_pid; }
  std::optional<QString> workspace() const override { return m_workspace; }
  std::optional<AbstractWindowManager::WindowBounds> bounds() const override { return m_bounds; }
  bool canClose() const override { return m_canClose; }

  /**
   * Get the underlying X11 window ID
   */
  xcb_window_t windowId() const { return m_window; }

private:
  /**
   * Query window properties from X server
   */
  void queryProperties(xcb_connection_t *connection);

  /**
   * Query window title from _NET_WM_NAME or WM_NAME
   */
  QString queryTitle(xcb_connection_t *connection);

  /**
   * Query window class from WM_CLASS
   */
  QString queryWmClass(xcb_connection_t *connection);

  /**
   * Query process ID from _NET_WM_PID
   */
  std::optional<int> queryPid(xcb_connection_t *connection);

  /**
   * Query desktop/workspace from _NET_WM_DESKTOP
   */
  std::optional<QString> queryWorkspace(xcb_connection_t *connection);

  /**
   * Query window geometry/bounds
   */
  std::optional<AbstractWindowManager::WindowBounds> queryBounds(xcb_connection_t *connection);

  /**
   * Check if window supports WM_DELETE_WINDOW protocol
   */
  bool queryCanClose(xcb_connection_t *connection);

  xcb_window_t m_window;
  QString m_id;
  QString m_title;
  QString m_wmClass;
  std::optional<int> m_pid;
  std::optional<QString> m_workspace;
  std::optional<AbstractWindowManager::WindowBounds> m_bounds;
  bool m_canClose;
};
