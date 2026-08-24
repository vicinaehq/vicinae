#pragma once
#include "abstract-app-runtime.hpp"
#include <memory>
#include <qobject.h>
#include <qtmetamacros.h>

class WindowManager;

class AppService;

class AppRuntime : public QObject {
  Q_OBJECT

signals:
  void runningAppsChanged();
  void frontmostAppChanged();

public:
  AppRuntime(WindowManager &wm, AppService &appService);

  bool isRunning(const AbstractApplication &app) const;
  std::shared_ptr<AbstractApplication> frontmostApp() const;
  QString pasteTargetName() const;
  std::shared_ptr<AbstractApplication> pasteTargetApp() const;
  void capturePasteTarget();
  bool activate(const AbstractApplication &app) const;
  bool quit(const AbstractApplication &app) const;
  bool forceQuit(const AbstractApplication &app) const;

private:
  static std::unique_ptr<AbstractAppRuntime> createProvider(WindowManager &wm, AppService &appService);
  void rememberPasteTarget();

  std::unique_ptr<AbstractAppRuntime> m_provider;
  QString m_pasteTargetName;
  std::shared_ptr<AbstractApplication> m_pasteTargetApp;
};
