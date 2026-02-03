#pragma once
#include "services/window-manager/abstract-window-manager.hpp"
#include "x11-window.hpp"
#include <QHash>
#include <QString>
#include <xcb/xcb.h>
#include <memory>

class X11EventListener;

/**
 * Window manager implementation for X11 using XCB (X C Bindings).
 * Communicates with EWMH-compliant window managers to enumerate and control windows.
 *
 * Supports:
 * - Window enumeration via _NET_CLIENT_LIST
 * - Window activation via _NET_ACTIVE_WINDOW
 * - Window closing via WM_DELETE_WINDOW
 * - Workspace support via _NET_WM_DESKTOP
 */
class X11WindowManager : public AbstractWindowManager {
public:
  X11WindowManager();
  ~X11WindowManager() override;

  // AbstractWindowManager interface implementation
  QString id() const override { return "x11"; }
  QString displayName() const override { return "X11"; }

  WindowList listWindowsSync() const override;
  std::shared_ptr<AbstractWindow> getFocusedWindowSync() const override;
  void focusWindowSync(const AbstractWindow &window) const override;
  bool closeWindow(const AbstractWindow &window) const override;

  bool hasWorkspaces() const override;
  WorkspaceList listWorkspaces() const override;
  std::shared_ptr<AbstractWorkspace> getActiveWorkspace() const override;

  bool supportsPaste() const override { return false; }
  bool pasteToWindow(const AbstractWindow *window, const AbstractApplication *app) override { return false; }
  bool ping() const override;
  bool isActivatable() const override;
  void start() override;

private:
  /**
   * Get or create XCB connection to X server
   */
  xcb_connection_t *getConnection() const;

  /**
   * Get the root window of the default screen
   */
  xcb_window_t getRootWindow() const;

  /**
   * Get screen information
   */
  xcb_screen_t *getScreen() const;

  /**
   * Intern an atom (with caching)
   */
  xcb_atom_t internAtom(const char *name, bool only_if_exists = false) const;

  /**
   * Get list of all client windows from _NET_CLIENT_LIST
   */
  std::vector<xcb_window_t> getClientList() const;

  /**
   * Get the currently focused window from _NET_ACTIVE_WINDOW
   */
  xcb_window_t getActiveWindow() const;

  /**
   * Check if window manager supports EWMH
   */
  bool checkEWMHSupport() const;

  /**
   * Get number of desktops/workspaces from _NET_NUMBER_OF_DESKTOPS
   */
  std::optional<uint32_t> getNumberOfDesktops() const;

  /**
   * Get current desktop from _NET_CURRENT_DESKTOP
   */
  std::optional<uint32_t> getCurrentDesktop() const;

  /**
   * Get desktop names from _NET_DESKTOP_NAMES
   */
  QStringList getDesktopNames() const;

  mutable xcb_connection_t *m_connection;
  mutable xcb_screen_t *m_screen;
  mutable QHash<QString, xcb_atom_t> m_atomCache;
  bool m_ewmhSupported;
  std::unique_ptr<X11EventListener> m_eventListener;
};
