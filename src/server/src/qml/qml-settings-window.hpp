#pragma once
#include "common/context.hpp"
#include <QObject>
#include <QQmlApplicationEngine>

class QmlConfigBridge;
class QmlImageSource;
class QmlThemeBridge;
class QmlGeneralSettingsModel;
class QmlKeybindSettingsModel;
class QmlExtensionSettingsModel;
class QQuickWindow;

class QmlSettingsWindow : public QObject {
  Q_OBJECT

  Q_PROPERTY(int currentTab READ currentTab WRITE setCurrentTab NOTIFY currentTabChanged)
  Q_PROPERTY(QString version READ version CONSTANT)
  Q_PROPERTY(QString commitHash READ commitHash CONSTANT)
  Q_PROPERTY(QString buildInfo READ buildInfo CONSTANT)
  Q_PROPERTY(QString headline READ headline CONSTANT)
  Q_PROPERTY(QmlGeneralSettingsModel *generalModel READ generalModel CONSTANT)
  Q_PROPERTY(QmlKeybindSettingsModel *keybindModel READ keybindModel CONSTANT)
  Q_PROPERTY(QmlExtensionSettingsModel *extensionModel READ extensionModel CONSTANT)

public:
  explicit QmlSettingsWindow(ApplicationContext &ctx, QObject *parent = nullptr);

  int currentTab() const { return m_currentTab; }
  void setCurrentTab(int tab);

  QString version() const;
  QString commitHash() const;
  QString buildInfo() const;
  QString headline() const;

  QmlGeneralSettingsModel *generalModel() const { return m_generalModel; }
  QmlKeybindSettingsModel *keybindModel() const { return m_keybindModel; }
  QmlExtensionSettingsModel *extensionModel() const { return m_extensionModel; }

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

  ApplicationContext &m_ctx;
  QQmlApplicationEngine m_engine;
  QmlThemeBridge *m_themeBridge = nullptr;
  QmlConfigBridge *m_configBridge = nullptr;
  QmlImageSource *m_imgSource = nullptr;
  QmlGeneralSettingsModel *m_generalModel = nullptr;
  QmlKeybindSettingsModel *m_keybindModel = nullptr;
  QmlExtensionSettingsModel *m_extensionModel = nullptr;
  QQuickWindow *m_window = nullptr;
  int m_currentTab = 0;
  bool m_initialized = false;
};
