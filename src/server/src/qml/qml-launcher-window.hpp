#pragma once
#include "common/context.hpp"
#include <QObject>
#include <QQmlApplicationEngine>

class QmlActionPanelModel;
class QmlConfigBridge;
class QmlImageSource;
class QmlRootSearchModel;
class QmlThemeBridge;
class QmlBridgeViewBase;
class QmlCommandListModel;
class QQuickWindow;
class QQuickItem;
class BaseView;

class QmlLauncherWindow : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool hasCommandView READ hasCommandView NOTIFY hasCommandViewChanged)
  Q_PROPERTY(QString searchPlaceholder READ searchPlaceholder NOTIFY searchPlaceholderChanged)
  Q_PROPERTY(QString commandActionTitle READ commandActionTitle NOTIFY commandActionChanged)
  Q_PROPERTY(bool actionPanelOpen READ actionPanelOpen NOTIFY actionPanelOpenChanged)
  Q_PROPERTY(QmlActionPanelModel *actionPanelModel READ actionPanelModel NOTIFY actionPanelModelChanged)
  Q_PROPERTY(bool hasActions READ hasActions NOTIFY hasActionsChanged)

public:
  explicit QmlLauncherWindow(ApplicationContext &ctx, QObject *parent = nullptr);

  bool hasCommandView() const { return m_hasCommandView; }
  QString searchPlaceholder() const { return m_searchPlaceholder; }
  QString commandActionTitle() const;

  bool actionPanelOpen() const { return m_actionPanelOpen; }
  QmlActionPanelModel *actionPanelModel() const { return m_actionPanelModel; }
  bool hasActions() const { return m_hasActions; }

  Q_INVOKABLE void forwardSearchText(const QString &text);
  Q_INVOKABLE void handleReturn();
  Q_INVOKABLE void forwardKey(int key, int modifiers = 0);
  Q_INVOKABLE void goBack();
  Q_INVOKABLE void popToRoot();
  Q_INVOKABLE bool tryAliasFastTrack();
  Q_INVOKABLE void toggleActionPanel();
  Q_INVOKABLE void closeActionPanel();

signals:
  void hasCommandViewChanged();
  void searchPlaceholderChanged();
  void commandActionChanged();
  void searchTextUpdated(const QString &text);
  void viewNavigatedBack();
  void commandViewPushed(const QUrl &componentUrl, const QVariantMap &properties);
  void commandViewPopped();
  void commandStackCleared();
  void actionPanelOpenChanged();
  void actionPanelModelChanged();
  void hasActionsChanged();
  void actionPanelSubmenuPushed(QmlActionPanelModel *subModel);

private:
  void handleVisibilityChanged(bool visible);
  void handleCurrentViewChanged();
  void handleViewPoped(const BaseView *view);
  void embedWidget(BaseView *view);
  void removeWidget();
  void repositionWidget();

  ApplicationContext &m_ctx;
  QQmlApplicationEngine m_engine;
  QmlRootSearchModel *m_searchModel;
  QmlThemeBridge *m_themeBridge;
  QmlConfigBridge *m_configBridge;
  QmlImageSource *m_imgSource;
  QQuickWindow *m_window = nullptr;
  QQuickItem *m_contentArea = nullptr;
  BaseView *m_activeWidget = nullptr;
  bool m_hasCommandView = false;
  bool m_viewWasPopped = false;
  QString m_searchPlaceholder;

  // Action panel
  void updateActionPanelModel();
  void connectActionPanelModel(QmlActionPanelModel *model);

  bool m_actionPanelOpen = false;
  bool m_hasActions = false;
  QmlActionPanelModel *m_actionPanelModel = nullptr;
};
