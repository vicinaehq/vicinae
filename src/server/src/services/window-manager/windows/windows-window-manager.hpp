#pragma once
#include "services/window-manager/abstract-window-manager.hpp"
#include "virtual-desktops.hpp"
#include "windows-window.hpp"
#include <memory>
#include <unordered_map>
#include <vector>
#include <windows.h>

class QTimer;

namespace Win {

/**
 * Window manager implementation for Windows.
 *
 * Windows are enumerated with EnumWindows using alt-tab-like eligibility rules, and the cache is
 * kept fresh through WinEvent hooks (foreground, show/hide/destroy, minimize, title changes).
 * Virtual desktops are exposed as workspaces via `VirtualDesktops`; windows living on another
 * desktop are listed too, since focusing them makes the shell switch desktops automatically.
 * Moving other apps' windows across desktops has no stable API and is not supported.
 */
class WindowManager : public AbstractWindowManager {
public:
  WindowManager() = default;
  ~WindowManager() override;

  QString id() const override { return "windows"; }
  QString displayName() const override { return "Windows"; }

  WindowList listWindowsSync() const override { return m_cache; }
  WindowPtr getFocusedWindowSync() const override;
  bool supportsFocusTracking() const override { return true; }
  bool supportsFocusHandoffDetection() const override { return true; }
  void focusWindowSync(const AbstractWindow &window) const override;
  bool closeWindow(const AbstractWindow &window) const override;
  bool setWindowBounds(const AbstractWindow &window, const WindowBounds &bounds) const override;
  void refresh() const override { scheduleRebuild(); }

  bool hasWorkspaces() const override { return m_desktops && m_desktops->available(); }
  WorkspaceList listWorkspaces() const override;
  WorkspacePtr getActiveWorkspace() const override;
  bool supportsWorkspaceActivation() const override { return true; }
  bool activateWorkspace(const QString &workspaceId) const override;

  bool ping() const override { return true; }
  bool isActivatable() const override { return true; }
  void start() override;

  // Invoked by the WinEvent hook callback.
  void handleWinEvent(DWORD event, HWND hwnd);

private:
  void scheduleRebuild() const;
  void rebuildCache();
  bool isListable(HWND hwnd) const;
  std::shared_ptr<Window> buildWindow(HWND hwnd) const;
  QString appKeyFor(HWND hwnd, DWORD pid) const;
  QString keyForPid(DWORD pid) const;
  HWND ensureHelperWindow() const;

  std::unique_ptr<VirtualDesktops> m_desktops;
  std::vector<HWINEVENTHOOK> m_hooks;
  QTimer *m_rebuildTimer = nullptr;
  WindowList m_cache;
  mutable std::unordered_map<DWORD, QString> m_keyByPid;
  mutable HWND m_helper = nullptr;
};

} // namespace Win
