#pragma once
#include "services/window-manager/abstract-window-manager.hpp"

class QTimer;

#ifdef __OBJC__
@class MacosWindowObserver;
#else
class MacosWindowObserver;
#endif

/**
 * Window manager implementation for macOS, built on top of the Accessibility (AX) API.
 *
 * On macOS there is no separate window manager protocol to talk to: windows are owned by applications and
 * are enumerated by walking each running application's AX element tree. The same Accessibility permission
 * that lets us read window titles also lets us raise and close individual windows, so no Screen Recording
 * permission is required.
 *
 * macOS Spaces are intentionally not exposed as workspaces: there is no public API to enumerate them or
 * move windows across them reliably.
 */
class MacosWindowManager : public AbstractWindowManager {
public:
  MacosWindowManager();
  ~MacosWindowManager() override;

  QString id() const override { return "macos"; }
  QString displayName() const override { return "macOS"; }

  WindowList listWindowsSync() const override;
  std::shared_ptr<AbstractWindow> getFocusedWindowSync() const override;
  bool supportsFocusTracking() const override { return true; }
  void focusWindowSync(const AbstractWindow &window) const override;
  bool closeWindow(const AbstractWindow &window) const override;
  void requestWindowAccess() const override;

  bool ping() const override { return true; }
  bool isActivatable() const override;
  void start() override;

  // Invoked by the observer when an application-level event invalidates the window list.
  void notifyWindowsChanged();
  void notifyFocusChanged();

private:
  void scheduleRebuild() const;
  void rebuildCache();

  MacosWindowObserver *m_observer = nullptr;
  QTimer *m_rebuildTimer = nullptr;
  mutable WindowList m_cache;
  mutable bool m_didPromptTrust = false;
  bool m_rebuilding = false;
  bool m_rebuildPending = false;
};
