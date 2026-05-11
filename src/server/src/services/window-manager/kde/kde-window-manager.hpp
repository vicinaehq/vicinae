#pragma once
#include <memory>
#include <optional>
#include <qdbusservicewatcher.h>
#include <qobject.h>
#include <vector>
#include "environment.hpp"
#include "services/window-manager/abstract-window-manager.hpp"

namespace KDE {

struct WindowInfo {
  QString id;
  QString resourceClass;
  QString resourceName;
  QString caption;
  int pid = 0;
};

class Window : public AbstractWindowManager::AbstractWindow {
public:
  explicit Window(WindowInfo info) : m_info(std::move(info)) {}

  QString id() const override { return m_info.id; }
  QString title() const override { return m_info.caption; }
  QString wmClass() const override { return m_info.resourceClass; }
  std::optional<int> pid() const override {
    if (m_info.pid <= 0) return std::nullopt;
    return m_info.pid;
  }
  bool canClose() const override { return false; }

private:
  WindowInfo m_info;
};

class WindowTracker : public QObject {
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "org.vicinae.WindowTracker")

signals:
  void windowsChanged();
  void focusChanged();

public:
  explicit WindowTracker(QObject *parent = nullptr);

  std::vector<WindowInfo> snapshot() const { return m_windows; }
  QString focusedId() const { return m_focused; }
  void reset();

public slots:
  Q_SCRIPTABLE void add(const QString &id, const QString &resourceClass, const QString &resourceName,
                        const QString &caption, int pid);
  Q_SCRIPTABLE void remove(const QString &id);
  Q_SCRIPTABLE void activated(const QString &id);
  Q_SCRIPTABLE void error(const QString &message);

private:
  std::vector<WindowInfo> m_windows;
  QString m_focused;
};

class WindowManager : public AbstractWindowManager {
  Q_OBJECT

public:
  WindowManager();
  ~WindowManager() override;

  QString id() const override { return "kde"; }
  QString displayName() const override { return "KDE"; }

  WindowList listWindowsSync() const override;
  WindowPtr getFocusedWindowSync() const override;
  void focusWindowSync(const AbstractWindow &window) const override;
  bool supportsFocusTracking() const override { return true; }

  bool isActivatable() const override { return Environment::isWaylandPlasmaDesktop(); }
  bool ping() const override;

  void start() override;

private:
  bool loadTrackerScript();
  void unloadScriptByName(const QString &pluginName) const;
  bool runOneShot(const QString &source, const QString &pluginName) const;

  std::unique_ptr<WindowTracker> m_tracker;
  std::unique_ptr<QDBusServiceWatcher> m_watcher;
  bool m_serviceOwned = false;
};

} // namespace KDE
