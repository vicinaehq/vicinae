#pragma once
#include "common/context.hpp"
#include <QObject>
#include <QQmlApplicationEngine>
#include <QVariantList>
#include <string>
#include <unordered_map>
#include <vector>

class ConfigBridge;
class ImageSource;
class ThemeBridge;
class KeyboardBridge;
class GlobalShortcutBridge;
class PlatformBridge;
class GeneralSettingsModel;
class KeybindSettingsModel;
class ExtensionSettingsModel;
class SettingsSidebarModel;
class QQuickWindow;

class SettingsWindow : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString currentPage READ currentPage WRITE setCurrentPage NOTIFY currentPageChanged)
  Q_PROPERTY(
      QString pendingCommandId READ pendingCommandId WRITE setPendingCommandId NOTIFY pendingCommandIdChanged)
  Q_PROPERTY(SettingsSidebarModel *sidebarModel READ sidebarModel CONSTANT)
  Q_PROPERTY(QString version READ version CONSTANT)
  Q_PROPERTY(QString commitHash READ commitHash CONSTANT)
  Q_PROPERTY(QString buildInfo READ buildInfo CONSTANT)
  Q_PROPERTY(QString headline READ headline CONSTANT)
  Q_PROPERTY(QString updateStatus READ updateStatus NOTIFY updateStatusChanged)
  Q_PROPERTY(bool updateChecksSupported READ updateChecksSupported CONSTANT)
  Q_PROPERTY(GeneralSettingsModel *generalModel READ generalModel CONSTANT)
  Q_PROPERTY(KeybindSettingsModel *keybindModel READ keybindModel CONSTANT)
  Q_PROPERTY(ExtensionSettingsModel *extensionModel READ extensionModel CONSTANT)

public:
  explicit SettingsWindow(ApplicationContext &ctx, QObject *parent = nullptr);

  QString currentPage() const { return m_currentPage; }
  void setCurrentPage(const QString &page);

  QString pendingCommandId() const { return m_pendingCommandId; }
  void setPendingCommandId(const QString &id);

  SettingsSidebarModel *sidebarModel() const { return m_sidebarModel; }

  QString version() const;
  QString commitHash() const;
  QString buildInfo() const;
  QString headline() const;
  QString updateStatus() const;
  bool updateChecksSupported() const;
  Q_INVOKABLE void checkForUpdates();

  GeneralSettingsModel *generalModel() const { return m_generalModel; }
  KeybindSettingsModel *keybindModel() const { return m_keybindModel; }
  ExtensionSettingsModel *extensionModel() const { return m_extensionModel; }

  Q_INVOKABLE void openUrl(const QString &url);
  Q_INVOKABLE void close();
  Q_INVOKABLE void requestDefaultFocus();

  void show();
  void hide();

  void openTab(const QString &tabId);
  Q_INVOKABLE void selectExtension(const QString &entrypointId);

signals:
  void currentPageChanged();
  void pendingCommandIdChanged();
  void defaultFocusRequested();
  void updateStatusChanged();

private:
  void ensureInitialized();

  ApplicationContext &m_ctx;
  QQmlApplicationEngine m_engine;
  ThemeBridge *m_themeBridge = nullptr;
  ConfigBridge *m_configBridge = nullptr;
  ImageSource *m_imgSource = nullptr;
  KeyboardBridge *m_keyboardBridge = nullptr;
  GlobalShortcutBridge *m_globalShortcutBridge = nullptr;
  PlatformBridge *m_platformBridge = nullptr;
  GeneralSettingsModel *m_generalModel = nullptr;
  KeybindSettingsModel *m_keybindModel = nullptr;
  ExtensionSettingsModel *m_extensionModel = nullptr;
  SettingsSidebarModel *m_sidebarModel = nullptr;
  QQuickWindow *m_window = nullptr;
  QString m_currentPage = QStringLiteral("general");
  QString m_pendingCommandId;
  bool m_initialized = false;
};
