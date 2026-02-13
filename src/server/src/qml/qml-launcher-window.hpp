#pragma once
#include "common/context.hpp"
#include <QObject>
#include <QQmlApplicationEngine>

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

public:
  explicit QmlLauncherWindow(ApplicationContext &ctx, QObject *parent = nullptr);

  bool hasCommandView() const { return m_hasCommandView; }
  QString searchPlaceholder() const { return m_searchPlaceholder; }
  QString commandActionTitle() const;

  Q_INVOKABLE void forwardSearchText(const QString &text);
  Q_INVOKABLE void handleReturn();
  Q_INVOKABLE void forwardKey(int key, int modifiers = 0);
  Q_INVOKABLE void goBack();
  Q_INVOKABLE void popToRoot();

signals:
  void hasCommandViewChanged();
  void searchPlaceholderChanged();
  void commandActionChanged();
  void searchTextUpdated(const QString &text);
  void viewNavigatedBack();
  void commandViewPushed(QObject *model);
  void commandViewPopped();
  void commandStackCleared();

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
  QQuickWindow *m_window = nullptr;
  QQuickItem *m_contentArea = nullptr;
  BaseView *m_activeWidget = nullptr;
  bool m_hasCommandView = false;
  bool m_viewWasPopped = false;
  QString m_searchPlaceholder;
};
