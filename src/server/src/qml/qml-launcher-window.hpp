#pragma once
#include "common/context.hpp"
#include "qml-image-url.hpp"
#include <QObject>
#include <QQmlApplicationEngine>

class ActionPanelController;
class QmlAlertModel;
class QmlConfigBridge;
class QmlImageSource;
class QmlRootSearchModel;
class QmlThemeBridge;
class QmlBridgeViewBase;
class QmlCommandListModel;
class QQuickWindow;
class BaseView;
class DialogContentWidget;

class QmlLauncherWindow : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool hasCommandView READ hasCommandView NOTIFY hasCommandViewChanged)
  Q_PROPERTY(QString searchPlaceholder READ searchPlaceholder NOTIFY searchPlaceholderChanged)
  Q_PROPERTY(QUrl searchAccessoryUrl READ searchAccessoryUrl NOTIFY searchAccessoryChanged)
  Q_PROPERTY(QObject *commandViewHost READ commandViewHost NOTIFY commandViewHostChanged)
  Q_PROPERTY(QString navigationTitle READ navigationTitle NOTIFY navigationStatusChanged)
  Q_PROPERTY(QmlImageUrl navigationIcon READ navigationIcon NOTIFY navigationStatusChanged)
  Q_PROPERTY(bool toastActive READ toastActive NOTIFY toastActiveChanged)
  Q_PROPERTY(QString toastTitle READ toastTitle NOTIFY toastChanged)
  Q_PROPERTY(QString toastMessage READ toastMessage NOTIFY toastChanged)
  Q_PROPERTY(int toastStyle READ toastStyle NOTIFY toastChanged)
  Q_PROPERTY(QmlAlertModel *alertModel READ alertModel CONSTANT)
  Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)
  Q_PROPERTY(bool searchVisible READ searchVisible NOTIFY searchVisibleChanged)
  Q_PROPERTY(bool searchInteractive READ searchInteractive NOTIFY searchInteractiveChanged)
  Q_PROPERTY(bool compacted READ compacted NOTIFY compactedChanged)
  Q_PROPERTY(bool hasCompleter READ hasCompleter NOTIFY completerChanged)
  Q_PROPERTY(QVariantList completerArgs READ completerArgs NOTIFY completerChanged)
  Q_PROPERTY(QString completerIcon READ completerIcon NOTIFY completerChanged)
  Q_PROPERTY(QVariantList completerValues READ completerValues NOTIFY completerValuesChanged)

public:
  explicit QmlLauncherWindow(ApplicationContext &ctx, QObject *parent = nullptr);

  bool hasCommandView() const { return m_hasCommandView; }
  QString searchPlaceholder() const { return m_searchPlaceholder; }
  QUrl searchAccessoryUrl() const { return m_searchAccessoryUrl; }
  QObject *commandViewHost() const { return m_commandViewHost; }

  QString navigationTitle() const { return m_navigationTitle; }
  QmlImageUrl navigationIcon() const { return m_navigationIcon; }
  bool toastActive() const { return m_toastActive; }
  QString toastTitle() const { return m_toastTitle; }
  QString toastMessage() const { return m_toastMessage; }
  int toastStyle() const { return m_toastStyle; }
  QmlAlertModel *alertModel() const { return m_alertModel; }
  bool isLoading() const { return m_isLoading; }
  bool searchVisible() const { return m_searchVisible; }
  bool searchInteractive() const { return m_searchInteractive; }
  bool compacted() const { return m_compacted; }
  bool hasCompleter() const { return m_hasCompleter; }
  QVariantList completerArgs() const { return m_completerArgs; }
  QString completerIcon() const { return m_completerIcon; }
  QVariantList completerValues() const { return m_completerValues; }

  Q_INVOKABLE void expand();
  Q_INVOKABLE void forwardSearchText(const QString &text);
  Q_INVOKABLE void handleReturn();
  Q_INVOKABLE bool forwardKey(int key, int modifiers = 0);
  Q_INVOKABLE void goBack();
  Q_INVOKABLE void popToRoot();
  Q_INVOKABLE bool tryAliasFastTrack();
  Q_INVOKABLE void setCompleterValue(int index, const QString &value);

signals:
  void compactedChanged();
  void hasCommandViewChanged();
  void searchPlaceholderChanged();
  void searchAccessoryChanged();
  void commandViewHostChanged();
  void searchTextUpdated(const QString &text);
  void viewNavigatedBack();
  void commandViewPushed(const QUrl &componentUrl, const QVariantMap &properties);
  void commandViewPopped();
  void commandStackCleared();
  void navigationStatusChanged();
  void toastActiveChanged();
  void toastChanged();
  void openSearchAccessoryRequested();
  void isLoadingChanged();
  void searchVisibleChanged();
  void searchInteractiveChanged();
  void completerChanged();
  void completerValuesChanged();
  void completerValidationFailed();

private:
  void handleVisibilityChanged(bool visible);
  void handleCurrentViewChanged();
  void handleViewPoped(const BaseView *view);
  void setCompacted(bool value);
  void tryCompaction();

  ApplicationContext &m_ctx;
  QQmlApplicationEngine m_engine;
  QmlRootSearchModel *m_searchModel;
  QmlThemeBridge *m_themeBridge;
  QmlConfigBridge *m_configBridge;
  QmlImageSource *m_imgSource;
  QQuickWindow *m_window = nullptr;
  bool m_compacted = false;
  bool m_hasCommandView = false;
  bool m_isLoading = false;
  bool m_searchVisible = true;
  bool m_searchInteractive = true;
  bool m_viewWasPopped = false;
  QString m_searchPlaceholder;
  QUrl m_searchAccessoryUrl;
  QMetaObject::Connection m_searchAccessoryConnection;
  QObject *m_commandViewHost = nullptr;

  // Navigation status
  QString m_navigationTitle;
  QmlImageUrl m_navigationIcon;

  // Toast
  bool m_toastActive = false;
  QString m_toastTitle;
  QString m_toastMessage;
  int m_toastStyle = 0;

  // Action panel
  ActionPanelController *m_actionPanel;

  // Alert
  QmlAlertModel *m_alertModel = nullptr;

  // Completer
  bool m_hasCompleter = false;
  QVariantList m_completerArgs;
  QString m_completerIcon;
  QVariantList m_completerValues;
};
