#pragma once
#include <qfuture.h>
#include <qobject.h>
#include <qpromise.h>
#include <qstringview.h>
#include <qtmetamacros.h>
#include <vector>

class AbstractWindowManager : public QObject {
  struct WindowSize {
    int width;
    int height;
  };

  struct WindowPosition {
    int x;
    int y;
  };

public:
  class Workspace {
    QString _id;
    QString _name;

  public:
    const QString &id() { return _id; }
    const QString &name() { return _name; }
  };

  class Window {
    enum WindowType { Hyprland };

    enum PlatformType {
      PlatformX11,
      PlatformXWayland,
      PlatformWayland,
    };
    enum LayoutMode { LayoutModeTiling, LayoutModeFloating };

  protected:
    QString _id;
    QString _title;
    QString _wmClass;
    int _pid;
    PlatformType _platform;
    WindowPosition _position;
    WindowSize _size;
    bool _fullscreen;
    bool _hidden;
    QString _workspaceId;
    LayoutMode _layoutMode;

  public:
    const QString &id() const { return _id; }
    const QString &title() const { return _title; }
    const QString &wmClass() const { return _wmClass; }
    const QString &workspaceId() const { return _workspaceId; }
    LayoutMode layoutMode() const { return _layoutMode; }
    int pid() const { return _pid; }
    PlatformType platform() const { return _platform; }
    const WindowPosition &position() const { return _position; }
    const WindowSize &size() const { return _size; }
    bool isFullScreen() const { return _fullscreen; }
  };

  using WindowList = std::vector<std::shared_ptr<Window>>;
  using WorkspaceList = std::vector<std::shared_ptr<Workspace>>;

public:
  AbstractWindowManager() {}

  virtual QString name() const = 0;
  virtual WindowList listWindowsSync() const { return {}; };
  virtual std::shared_ptr<Window> getActiveWindowSync() const { return nullptr; }
  virtual std::shared_ptr<Workspace> getActiveWorkspaceSync() const { return nullptr; }
  virtual WorkspaceList listWorkspacesSync() const { return {}; }
  virtual void moveToWorkspaceSync(const Window &window, const Workspace &workspace) {}
  virtual void focusWindowSync(const Window &window) const {};

  virtual bool ping() const = 0;
  virtual bool isActivatable() const = 0;
  virtual void start() const = 0;
};
