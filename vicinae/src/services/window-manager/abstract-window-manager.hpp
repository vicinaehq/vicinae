#pragma once
#include <cstdint>
#include <qfuture.h>
#include <qobject.h>
#include <qpromise.h>
#include <qstringview.h>
#include "lib/keyboard/keyboard.hpp"
#include <qtmetamacros.h>
#include <vector>

/**
 * Abstract class from which all window manager implementations should inherit from.
 * Window managers vary in capability and as such, many of the methods in here are optional.
 * Only basic window manager interactions such as listing windows, focusing... are mandatory for all
 * implementations.
 */
class AbstractWindowManager : public QObject {
  Q_OBJECT

signals:
  /**
   * An event invalidating the current list of windows has occured and views
   * dealing with them should request an update. This typically occurs when a new window is created, close, or
   * killed.
   */
  void windowsChanged() const;

public:
  struct WindowBounds {
    uint32_t x = 0;
    uint32_t y = 0;
    uint32_t width = 0;
    uint32_t height = 0;
  };

  /**
   * A Window from the current window manager.
   */
  class AbstractWindow {
  public:
    virtual QString id() const = 0;
    virtual QString title() const = 0;
    virtual QString wmClass() const = 0;

    /**
     * The pid of the process that owns that window, if such information is available.
     */
    virtual std::optional<int> pid() const { return std::nullopt; }

    virtual std::optional<QString> workspace() const { return std::nullopt; }
    virtual std::optional<WindowBounds> bounds() const { return std::nullopt; }
    bool fullScreen() const { return false; }

    virtual bool canClose() const { return true; }
    virtual bool canFullScreen() const { return true; }
  };

  class AbstractWorkspace {
  public:
    virtual QString id() const = 0;
    virtual QString name() const { return id(); }
    virtual QString monitor() const = 0;
    virtual bool hasFullScreen() const { return false; }
  };

  using WindowPtr = std::shared_ptr<AbstractWindow>;
  using WindowList = std::vector<WindowPtr>;
  using WorkspacePtr = std::shared_ptr<AbstractWorkspace>;
  using WorkspaceList = std::vector<WorkspacePtr>;

public:
  virtual ~AbstractWindowManager() = default;

  /**
   * Unique identifier for this window manager.
   */
  virtual QString id() const = 0;

  /**
   * Window manager name to display in debug context. Unlike `id()` this does not need to return
   * a unique string. Defaults to `id()` if not reimplemented.
   */
  virtual QString displayName() const { return id(); }

  virtual WindowList listWindowsSync() const { return {}; };

  /**
   * Should return nullptr if there is no focused window (practically very rare).
   */
  virtual std::shared_ptr<AbstractWindow> getFocusedWindowSync() const { return nullptr; }

  virtual void focusWindowSync(const AbstractWindow &window) const {}

  /**
   * If this returns true, make sure to implement `workspaces` correctly and also
   * have every window return a correct workspace ID.
   */
  virtual bool hasWorkspaces() const { return false; }

  virtual WorkspaceList listWorkspaces() const { return {}; }

  /**
   * You can reimplement this if your window manager implementation has a more efficient way
   * of fetching windows for a specific workspace. In most cases, the performance impact is negligible.
   */
  virtual WindowList listWorkspaceWindows(const QString &workspaceId) {
    WindowList windows;

    for (const auto &win : listWindowsSync()) {
      if (win->workspace() != workspaceId) continue;
      windows.emplace_back(win);
    }

    return windows;
  }

  /**
   * The active workspace. If the window manager can be in a state where no workspace is active,
   * this should return null.
   */
  virtual WorkspacePtr getActiveWorkspace() const { return {}; }

  /**
   * Close a window. Returns true if successful, false otherwise.
   * This is a common operation that should be supported by all window managers.
   */
  virtual bool closeWindow(const AbstractWindow &window) const { return false; }

  /**
   * Whether the window manager supports sending arbitrary key events to any given window.
   * If this returns true, you should expect `sendShortcutSync` to be called at some point.
   * This is used to provide "paste to focused window" functionnality by copying data into the clipboard
   * and then send a CTRL-V to the focused window.
   * If this is false, Vicinae will automatically fallback to regular clipboard copy and not provide
   * "paste to focused window" actions.
   */
  virtual bool supportsInputForwarding() const { return false; }

  /**
   * If the window manager supports input forwarding, this should send the provided shortcut to the provided
   * window. It's up to you to properly transform the shortcut to whatever representation the window manager
   * expects.
   * Note that for now, we only send CTRL-V shortcuts using this method, so hardcoding the logic for only that
   * specific one is OK (for now).
   */
  virtual bool sendShortcutSync(const AbstractWindow &window, const Keyboard::Shortcut &shortcut) {
    return false;
  }

  bool pasteToFocusedWindow() {
    auto window = getFocusedWindowSync();

    if (!window) return false;

    return sendShortcutSync(*window.get(), Keyboard::Shortcut::osPaste());
  }

  /**
   * To make sure the window manager IPC link is healthy.
   */
  virtual bool ping() const = 0;

  /**
   * Should determine whether this window manager can handle the current environment.
   * This is used to determine which window manager service to spawn at startup. Try to make this check as
   * precise as possible to make the best detection possible and avoid false positives.
   * For example, avoid just checking for wayland if you are dealing with a wayland compositor. Try to look
   * for a special environment variable or socket file that may be present.
   */
  virtual bool isActivatable() const = 0;

  /**
   * Called when the window manager is started, after it was deemed activatable for the current
   * environment.
   */
  virtual void start() = 0;

private:
};
