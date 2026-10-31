#pragma once
#include "abstract-window-manager.hpp"
#include "services/app-service/abstract-app-db.hpp"

class WindowManager : public QObject {
  Q_OBJECT

signals:
  void windowsChanged() const;
  void focusChanged() const;

public:
  bool isCapable() const;

  AbstractWindowManager *provider() const;
  AbstractWindowManager::WindowList listWindowsSync();
  AbstractWindowManager::WindowPtr getFocusedWindow();
  // raw provider result: null while the launcher holds focus, never the remembered window
  AbstractWindowManager::WindowPtr focusedForeignWindow() const;

  AbstractWindowManager::WindowList findAppWindows(const AbstractApplication &app) const;
  const AbstractWindowManager::WindowList &listWindows() const;
  const AbstractWindowManager::AbstractWindow *findWindowById(const QString &id);
  AbstractWindowManager::WorkspacePtr findWorkspaceById(const QString &id);

  bool isOnActiveWorkspace(const AbstractWindowManager::AbstractWindow &window) const;

  WindowManager();

private:
  static std::vector<std::unique_ptr<AbstractWindowManager>> createCandidates();
  static std::unique_ptr<AbstractWindowManager> createProvider();
  void updateWindowCache();
  void updateFocusMemory();
  AbstractWindowManager::WindowPtr rememberedWindow();

  // we maintain our own window cache so that wm implementations are not required to cache themselves.
  AbstractWindowManager::WindowList m_windows;

  AbstractWindowManager::WindowPtr m_lastFocusedWindow;

  // fetched on first lookup, invalidated on windowsChanged; some backends list workspaces over IPC
  std::optional<AbstractWindowManager::WorkspaceList> m_workspaces;

  std::unique_ptr<AbstractWindowManager> m_provider;
};
