#pragma once
#include "common/context.hpp"
#include <QObject>
#include <QUrl>
#include <QVariantMap>
#include <QVariantList>

class AbstractAction;
class ActionPanelView;
class BaseView;
class QKeyEvent;
class SubmenuAction;

class ActionPanelController : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool open READ isOpen NOTIFY openChanged)
  Q_PROPERTY(bool hasActions READ hasActions NOTIFY hasActionsChanged)
  Q_PROPERTY(bool hasMultipleActions READ hasMultipleActions NOTIFY hasMultipleActionsChanged)
  Q_PROPERTY(QString primaryActionTitle READ primaryActionTitle NOTIFY primaryActionChanged)
  Q_PROPERTY(
      QVariantList primaryActionShortcutTokens READ primaryActionShortcutTokens NOTIFY primaryActionChanged)
  Q_PROPERTY(int depth READ depth NOTIFY depthChanged)

signals:
  void openChanged();
  void hasActionsChanged();
  void hasMultipleActionsChanged();
  void primaryActionChanged();
  void depthChanged();
  void panelPushRequested(const QUrl &componentUrl, const QVariantMap &properties);
  void panelPopRequested();
  void stackClearRequested();

public:
  explicit ActionPanelController(ApplicationContext &ctx, QObject *parent = nullptr);

  bool isOpen() const { return m_open; }
  bool hasActions() const { return m_hasActions; }
  bool hasMultipleActions() const { return m_hasMultipleActions; }
  QString primaryActionTitle() const;
  QVariantList primaryActionShortcutTokens() const;
  int depth() const { return m_depth; }

  void syncToView(BaseView *view);

  Q_INVOKABLE void toggle();
  Q_INVOKABLE void open();
  Q_INVOKABLE void close();

  void pushPanel(const QUrl &componentUrl, const QVariantMap &properties);
  Q_INVOKABLE void pop();

  Q_INVOKABLE void onPanelPushed(QObject *panel);
  Q_INVOKABLE void onPanelPopped(QObject *currentPanel);

  Q_INVOKABLE bool tryShortcut(int key, int modifiers);

  bool executePrimaryAction();
  void executeAction(AbstractAction *action);
  void openSubmenu(SubmenuAction *action);

private:
  void openRootPanel();
  void connectView(ActionPanelView *view);

  ActionPanelView *activeRoot() const;

  ApplicationContext &m_ctx;
  bool m_open = false;
  bool m_hasActions = false;
  bool m_hasMultipleActions = false;
  int m_depth = 0;
  QObject *m_currentPanel = nullptr;
  QObject *m_connectionGuard = nullptr;
  BaseView *m_activeView = nullptr;
};
