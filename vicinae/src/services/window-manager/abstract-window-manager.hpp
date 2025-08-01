#pragma once
#include "ui/keyboard.hpp"
#include <qfuture.h>
#include <qobject.h>
#include <qpromise.h>
#include <qstringview.h>
#include <qtmetamacros.h>
#include <vector>

/**
 * Abstract class from which all window manager implementations should inherit from.
 * Window managers vary in capability and as such, many of the methods in here are optional.
 * Only basic window manager interactions such as listing windows, focusing... are mandatory for all
 * implementations.
 */
class AbstractWindowManager : public QObject {
public:
  /**
   * A Window from the current window manager.
   */
  class AbstractWindow {
  public:
    virtual QString id() const = 0;
    virtual QString title() const = 0;
    virtual QString wmClass() const = 0;
    virtual std::optional<int> pid() const { return std::nullopt; }
  };

  using WindowPtr = std::shared_ptr<AbstractWindow>;
  using WindowList = std::vector<WindowPtr>;
  // using WorkspaceList = std::vector<std::shared_ptr<Workspace>>;

public:
  AbstractWindowManager() {}

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
  virtual bool sendShortcutSync(const AbstractWindow &window, const KeyboardShortcut &shortcut) {
    return false;
  }

  bool pasteToFocusedWindow() {
    auto window = getFocusedWindowSync();

    if (!window) return false;

    return sendShortcutSync(*window.get(), KeyboardShortcut::paste());
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
  virtual void start() const = 0;
};
