#pragma once
#include "ui/qml-engine-scope.hpp"
#include "common/context.hpp"
#include <QObject>
#include <QQmlApplicationEngine>

class ThemeBridge;
class ConfigBridge;
class ImageSource;
class KeyboardBridge;
class GlobalShortcutBridge;
class PlatformBridge;
class GeneralSettingsModel;
class MacosPermissionService;
class QQuickWindow;

class OnboardingWindow : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(Onboarding)
  QML_SINGLETON

public:
  static OnboardingWindow *create(QQmlEngine *engine, QJSEngine *) {
    return QmlEngineScope::get<OnboardingWindow>(engine);
  }

private:
  Q_PROPERTY(GeneralSettingsModel *generalModel READ generalModel CONSTANT)
  Q_PROPERTY(
      bool loginItemEnabled READ loginItemEnabled WRITE setLoginItemEnabled NOTIFY loginItemEnabledChanged)
  Q_PROPERTY(bool loginItemSupported READ loginItemSupported CONSTANT)

signals:
  void loginItemEnabledChanged();

public:
  explicit OnboardingWindow(ApplicationContext &ctx, QObject *parent = nullptr);

  static bool shouldShow();

  GeneralSettingsModel *generalModel() const { return m_generalModel; }
  Q_PROPERTY(MacosPermissionService *permissions READ permissions CONSTANT)
  MacosPermissionService *permissions() const { return m_permissions; }

  bool loginItemEnabled() const { return m_loginItemEnabled; }
  void setLoginItemEnabled(bool enabled);

  bool loginItemSupported() const;

  Q_INVOKABLE void finish();
  Q_INVOKABLE void openUrl(const QString &url);

  void show();

private:
  void ensureInitialized();
  void loadRoot();
  void reloadRoot();
  void markCompleted();

  ApplicationContext &m_ctx;
  QQmlApplicationEngine m_engine;
  ConfigBridge *m_configBridge = nullptr;
  GeneralSettingsModel *m_generalModel = nullptr;
  MacosPermissionService *m_permissions = nullptr;
  QQuickWindow *m_window = nullptr;
  bool m_loginItemEnabled = false;
  bool m_initialized = false;
  bool m_completed = false;
};
