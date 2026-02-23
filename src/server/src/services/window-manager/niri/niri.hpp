#pragma once
#include "services/window-manager/abstract-window-manager.hpp"
#include "wayland/virtual-keyboard.hpp"
#include <qsocketnotifier.h>
#include <xkbcommon/xkbcommon-keysyms.h>

namespace Niri {

class Window : public AbstractWindowManager::AbstractWindow {
public:
  struct FocusTimestamp {
    int64_t secs = 0;
    int32_t nanos = 0;
  };

  QString id() const override { return m_id; }
  std::optional<int> pid() const override { return m_pid; }
  QString title() const override { return m_title; }
  QString wmClass() const override { return m_wmClass; }
  std::optional<QString> workspace() const override { return m_workspaceId; }
  bool canClose() const override { return true; }

  bool isFocused() const { return m_focused; }
  void setFocused(bool focused) { m_focused = focused; }
  std::optional<FocusTimestamp> focusTimestamp() const { return m_focusTimestamp; }
  void setFocusTimestamp(std::optional<FocusTimestamp> value) { m_focusTimestamp = value; }
  void updateFromJson(const QJsonObject &json);

private:
  QString m_id;
  QString m_title;
  QString m_wmClass;
  std::optional<int> m_pid;
  std::optional<QString> m_workspaceId;
  bool m_focused = false;
  std::optional<FocusTimestamp> m_focusTimestamp;
};

class Workspace : public AbstractWindowManager::AbstractWorkspace {
public:
  QString id() const override { return m_id; }
  QString name() const override { return m_name; }
  QString monitor() const override { return m_monitor; }

  bool isActive() const { return m_active; }
  bool isFocused() const { return m_focused; }

  void setActive(bool active) { m_active = active; }
  void setFocused(bool focused) { m_focused = focused; }
  void updateFromJson(const QJsonObject &json);

private:
  QString m_id;
  QString m_name;
  QString m_monitor;
  bool m_active = false;
  bool m_focused = false;
};

class WindowManager : public AbstractWindowManager {
public:
  WindowManager();
  ~WindowManager() override;

  QString id() const override { return "niri"; }
  QString displayName() const override { return "Niri"; }

  WindowList listWindowsSync() const override;
  AbstractWindowManager::WindowPtr getFocusedWindowSync() const override;
  void focusWindowSync(const AbstractWindow &window) const override;
  bool closeWindow(const AbstractWindow &window) const override;

  bool hasWorkspaces() const override { return true; }
  WorkspaceList listWorkspaces() const override;
  WorkspacePtr getActiveWorkspace() const override;

  bool supportsPaste() const override;
  bool pasteToWindow(const AbstractWindow *window, const AbstractApplication *app) override;

  bool ping() const override;
  bool isActivatable() const override;
  void start() override;

private:
  bool connectEventStream();
  void handleEventSocketReadable();
  void processEventLine(const QString &line);

  void updateWindowsFromArray(const QJsonArray &windows);
  void upsertWindow(const QJsonObject &window);
  void removeWindow(const QString &id);
  void setFocusedWindow(const QString &id);
  void setWindowFocusTimestamp(const QString &id, const QJsonValue &focusTimestamp);
  void sortWindowsByFocusTimestamp();

  void updateWorkspacesFromArray(const QJsonArray &workspaces);
  void setActiveWorkspace(const QString &id, bool focused);

  std::optional<QJsonValue> sendRequest(const QString &rawRequest) const;
  std::optional<QJsonValue> sendActionRequest(const QString &actionName, const QJsonObject &payload) const;
  std::optional<QJsonValue> sendUnitRequest(const char *name) const;

  static std::optional<uint64_t> parseWindowId(const QString &id);
  static QString niriSocketPath();

  WindowList m_windows;
  WorkspaceList m_workspaces;
  QSocketNotifier *m_eventNotifier = nullptr;
  int m_eventFd = -1;
  QString m_eventBuffer;
  Wayland::VirtualKeyboard m_keyboard;
};

} // namespace Niri
