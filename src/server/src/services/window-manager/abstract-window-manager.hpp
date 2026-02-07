#pragma once
#include <cstdint>
#include <qflags.h>
#include <qfuture.h>
#include <qobject.h>
#include <qpromise.h>
#include <qstringview.h>
#include "services/app-service/abstract-app-db.hpp"
#include <QApplication>
#include <QScreen>
#include <ranges>
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
   * dealing with them should request an update. This typically occurs when a new window is created, closed,
   * or killed.
   */
  void windowsChanged() const;

public:
  struct WindowBounds {
    uint32_t x = 0;
    uint32_t y = 0;
    uint32_t width = 0;
    uint32_t height = 0;
  };

  struct Screen {
    QString name;
    QRect bounds;
    QString manufacturer;
    QString model;
    std::optional<QString> serial;
  };

  struct BlurConfig {
    bool enabled = true;
    int rounding = 0;
  };

  /**
   * A Window from the current window manager.
   */
  class AbstractWindow {
  public:
    virtual ~AbstractWindow() = default;

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
    virtual ~AbstractWorkspace() = default;

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

  /**
   * Will eventually be moved elsewhere. For this trigger dim around for Hyprland only.
   */
  virtual bool setDimAround(bool value = true) { return false; }

  virtual WindowList listWindowsSync() const { return {}; };

  virtual std::vector<Screen> listScreensSync() const {
    auto tr = [](const QScreen *qtScreen) -> Screen {
      Screen sc{.name = qtScreen->name(),
                .bounds = qtScreen->geometry(),
                .manufacturer = qtScreen->manufacturer(),
                .model = qtScreen->model()};
      if (auto serial = qtScreen->serialNumber(); !serial.isEmpty()) { sc.serial = serial; }
      return sc;
    };
    return QApplication::screens() | std::views::transform(tr) | std::ranges::to<std::vector>();
  }

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
   * Whether pasting the content of the clipboard to a window is supported.
   * If it is the window manager should implement `pasteToWindow`.
   */
  virtual bool supportsPaste() const { return false; }

  /**
   * Paste the current content of the clipboard to the specified window.
   *
   * The `window` object is the currently focused window, if this information is available. Some environments
   * may be paste-capable but not have a way to get the currently focused window.
   *
   * `app` refers to the application `window` refers to, if such an information is known. It is null
   * otherwise. This is typically used to figure out whether the window is a terminal emulator so that a
   * different shortcut can be sent (ctrl+shift+V for most UNIX terminals).
   *
   *
   */
  virtual bool pasteToWindow(const AbstractWindow *window, const AbstractApplication *app) { return false; }

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
