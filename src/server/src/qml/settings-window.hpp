#pragma once
#include "common/context.hpp"
#include <QObject>
#include <QQmlApplicationEngine>
#include <QVariantList>

class ConfigBridge;
class ImageSource;
class ThemeBridge;
class GeneralSettingsModel;
class KeybindSettingsModel;
class ExtensionSettingsModel;
class QQuickWindow;

class SettingsWindow : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString currentPage READ currentPage WRITE setCurrentPage NOTIFY currentPageChanged)
  Q_PROPERTY(QVariantList sidebarExtensions READ sidebarExtensions NOTIFY sidebarExtensionsChanged)
  Q_PROPERTY(QString version READ version CONSTANT)
  Q_PROPERTY(QString commitHash READ commitHash CONSTANT)
  Q_PROPERTY(QString buildInfo READ buildInfo CONSTANT)
  Q_PROPERTY(QString headline READ headline CONSTANT)
  Q_PROPERTY(GeneralSettingsModel *generalModel READ generalModel CONSTANT)
  Q_PROPERTY(KeybindSettingsModel *keybindModel READ keybindModel CONSTANT)
  Q_PROPERTY(ExtensionSettingsModel *extensionModel READ extensionModel CONSTANT)

public:
  explicit SettingsWindow(ApplicationContext &ctx, QObject *parent = nullptr);

  QString currentPage() const { return m_currentPage; }
  void setCurrentPage(const QString &page);

  QVariantList sidebarExtensions() const;

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
  void currentPageChanged();
  void sidebarExtensionsChanged();

private:
  void ensureInitialized();
  void updateBlur();
  void rebuildSidebarExtensions();

  ApplicationContext &m_ctx;
  QQmlApplicationEngine m_engine;
  ThemeBridge *m_themeBridge = nullptr;
  ConfigBridge *m_configBridge = nullptr;
  ImageSource *m_imgSource = nullptr;
  GeneralSettingsModel *m_generalModel = nullptr;
  KeybindSettingsModel *m_keybindModel = nullptr;
  ExtensionSettingsModel *m_extensionModel = nullptr;
  QQuickWindow *m_window = nullptr;
  QString m_currentPage = QStringLiteral("general");
  QVariantList m_sidebarExtensions;
  bool m_initialized = false;
};
