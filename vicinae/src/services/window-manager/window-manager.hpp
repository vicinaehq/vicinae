#pragma once
#include "abstract-window-manager.hpp"
#include "services/app-service/abstract-app-db.hpp"

class WindowManager : public QObject {
  Q_OBJECT

signals:
  void windowsChanged() const;

public:
  AbstractWindowManager *provider() const;
  AbstractWindowManager::WindowList listWindowsSync();
  AbstractWindowManager::WindowPtr getFocusedWindow();
  bool canPaste() const;
  bool focusApp(const AbstractApplication &app) const;

  AbstractWindowManager::WindowList findWindowByClass(const QString &wmClass) const;
  AbstractWindowManager::WindowList findAppWindows(const AbstractApplication &app) const;
  AbstractWindowManager::WindowList listWindows() const;
  const AbstractWindowManager::AbstractWindow *findWindowById(const QString &id);

  WindowManager();

private:
  static std::vector<std::unique_ptr<AbstractWindowManager>> createCandidates();
  static std::unique_ptr<AbstractWindowManager> createProvider();
  void updateWindowCache();

  // we maintain our own window cache so that wm implementations are not required to cache themselves.
  AbstractWindowManager::WindowList m_windows;

  std::unique_ptr<AbstractWindowManager> m_provider;
};
