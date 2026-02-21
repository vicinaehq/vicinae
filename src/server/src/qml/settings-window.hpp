#pragma once
#include "common/context.hpp"
#include <QObject>
#include <QQmlApplicationEngine>

class ConfigBridge;
class ImageSource;
class ThemeBridge;
class GeneralSettingsModel;
class KeybindSettingsModel;
class ExtensionSettingsModel;
class QQuickWindow;

class SettingsWindow : public QObject {
  Q_OBJECT

  Q_PROPERTY(int currentTab READ currentTab WRITE setCurrentTab NOTIFY currentTabChanged)
  Q_PROPERTY(QString version READ version CONSTANT)
  Q_PROPERTY(QString commitHash READ commitHash CONSTANT)
  Q_PROPERTY(QString buildInfo READ buildInfo CONSTANT)
  Q_PROPERTY(QString headline READ headline CONSTANT)
  Q_PROPERTY(GeneralSettingsModel *generalModel READ generalModel CONSTANT)
  Q_PROPERTY(KeybindSettingsModel *keybindModel READ keybindModel CONSTANT)
  Q_PROPERTY(ExtensionSettingsModel *extensionModel READ extensionModel CONSTANT)

public:
  explicit SettingsWindow(ApplicationContext &ctx, QObject *parent = nullptr);

  int currentTab() const { return m_currentTab; }
  void setCurrentTab(int tab);

  QString version() const;
  QString commitHash() const;
  QString buildInfo() const;
  QString headline() const;

  GeneralSettingsModel *generalModel() const { return m_generalModel; }
  KeybindSettingsModel *keybindModel() const { return m_keybindModel; }
  ExtensionSettingsModel *extensionModel() const { return m_extensionModel; }

  Q_INVOKABLE void openUrl(const QString &url);
  Q_INVOKABLE void close();

  void show();
  void hide();

  void openTab(const QString &tabId);
  void selectExtension(const QString &entrypointId);

signals:
  void currentTabChanged();

private:
  void ensureInitialized();
  void updateBlur();

  ApplicationContext &m_ctx;
  QQmlApplicationEngine m_engine;
  ThemeBridge *m_themeBridge = nullptr;
  ConfigBridge *m_configBridge = nullptr;
  ImageSource *m_imgSource = nullptr;
  GeneralSettingsModel *m_generalModel = nullptr;
  KeybindSettingsModel *m_keybindModel = nullptr;
  ExtensionSettingsModel *m_extensionModel = nullptr;
  QQuickWindow *m_window = nullptr;
  int m_currentTab = 0;
  bool m_initialized = false;
};
