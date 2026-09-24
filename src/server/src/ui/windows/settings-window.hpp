#pragma once
#include "ui/qml-engine-scope.hpp"
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
class AISettingsModel;
class ToolSettingsModel;
class QQuickWindow;

class SettingsWindow : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(Settings)
  QML_SINGLETON

public:
  static SettingsWindow *create(QQmlEngine *engine, QJSEngine *) {
    return QmlEngineScope::get<SettingsWindow>(engine);
  }

private:
  Q_PROPERTY(QString currentPage READ currentPage WRITE setCurrentPage NOTIFY currentPageChanged)
  Q_PROPERTY(QString currentSubpage READ currentSubpage WRITE setCurrentSubpage NOTIFY currentSubpageChanged)
  Q_PROPERTY(bool canGoBack READ canGoBack NOTIFY historyChanged)
  Q_PROPERTY(bool canGoForward READ canGoForward NOTIFY historyChanged)
  Q_PROPERTY(
      QString pendingCommandId READ pendingCommandId WRITE setPendingCommandId NOTIFY pendingCommandIdChanged)
  Q_PROPERTY(SettingsSidebarModel *sidebarModel READ sidebarModel CONSTANT)
  Q_PROPERTY(QString version READ version CONSTANT)
  Q_PROPERTY(QString commitHash READ commitHash CONSTANT)
  Q_PROPERTY(QString buildInfo READ buildInfo CONSTANT)
  Q_PROPERTY(QString headline READ headline CONSTANT)
  Q_PROPERTY(GeneralSettingsModel *generalModel READ generalModel CONSTANT)
  Q_PROPERTY(KeybindSettingsModel *keybindModel READ keybindModel CONSTANT)
  Q_PROPERTY(ExtensionSettingsModel *extensionModel READ extensionModel CONSTANT)
  Q_PROPERTY(AISettingsModel *aiModel READ aiModel CONSTANT)
  Q_PROPERTY(ToolSettingsModel *toolModel READ toolModel CONSTANT)

public:
  explicit SettingsWindow(ApplicationContext &ctx, QObject *parent = nullptr);

  QString currentPage() const { return m_route.page; }
  void setCurrentPage(const QString &page);
  QString currentSubpage() const { return m_route.subpage; }
  void setCurrentSubpage(const QString &subpage);

  QString pendingCommandId() const { return m_pendingCommandId; }
  void setPendingCommandId(const QString &id);

  SettingsSidebarModel *sidebarModel() const { return m_sidebarModel; }

  QString version() const;
  QString commitHash() const;
  QString buildInfo() const;
  QString headline() const;

  GeneralSettingsModel *generalModel() const { return m_generalModel; }
  KeybindSettingsModel *keybindModel() const { return m_keybindModel; }
  ExtensionSettingsModel *extensionModel() const { return m_extensionModel; }
  AISettingsModel *aiModel() const { return m_aiModel; }
  ToolSettingsModel *toolModel() const { return m_toolModel; }

  bool canGoBack() const { return !m_backStack.isEmpty(); }
  bool canGoForward() const { return !m_forwardStack.isEmpty(); }
  Q_INVOKABLE void goBack();
  Q_INVOKABLE void goForward();

  Q_INVOKABLE void openUrl(const QString &url);
  Q_INVOKABLE void reportBug();
  Q_INVOKABLE void close();
  Q_INVOKABLE void requestDefaultFocus();

  void show();
  void hide();

  void openTab(const QString &tabId);
  Q_INVOKABLE void selectExtension(const QString &entrypointId);
  void openSubpage(const QString &providerId, const QString &subpage);

signals:
  void currentPageChanged();
  void currentSubpageChanged();
  void pendingCommandIdChanged();
  void defaultFocusRequested();
  void historyChanged();

private:
  void ensureInitialized();
  void loadRoot();
  void reloadRoot();

  ApplicationContext &m_ctx;
  QQmlApplicationEngine m_engine;
  ConfigBridge *m_configBridge = nullptr;
  GeneralSettingsModel *m_generalModel = nullptr;
  KeybindSettingsModel *m_keybindModel = nullptr;
  ExtensionSettingsModel *m_extensionModel = nullptr;
  SettingsSidebarModel *m_sidebarModel = nullptr;
  AISettingsModel *m_aiModel = nullptr;
  ToolSettingsModel *m_toolModel = nullptr;
  QQuickWindow *m_window = nullptr;
  struct Route {
    QString page;
    QString subpage;
    bool operator==(const Route &) const = default;
  };

  void navigate(const Route &route);

  Route m_route{.page = QStringLiteral("general")};
  QString m_pendingCommandId;
  QList<Route> m_backStack;
  QList<Route> m_forwardStack;
  bool m_navigatingHistory = false;
  bool m_initialized = false;
};
