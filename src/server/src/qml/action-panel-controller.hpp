#pragma once
#include "common/context.hpp"
#include <QElapsedTimer>
#include <QObject>
#include <QUrl>
#include <QVariantMap>
#include <QVariantList>
#include <memory>
#include <vector>

class AbstractAction;
class ActionListView;
class ActionPanelState;
class ActionPanelView;
class BaseView;
class QKeyEvent;
class SubmenuAction;

class ActionPanelController : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool open READ isOpen NOTIFY openChanged)
  Q_PROPERTY(bool hasActions READ hasActions NOTIFY actionsChanged)
  Q_PROPERTY(bool hasMultipleActions READ hasMultipleActions NOTIFY actionsChanged)
  Q_PROPERTY(QString primaryActionTitle READ primaryActionTitle NOTIFY primaryActionChanged)
  Q_PROPERTY(
      QVariantList primaryActionShortcutTokens READ primaryActionShortcutTokens NOTIFY primaryActionChanged)
  Q_PROPERTY(int depth READ depth NOTIFY depthChanged)

signals:
  void openChanged();
  void actionsChanged();
  void primaryActionChanged();
  void depthChanged();
  void panelPushRequested(const QUrl &componentUrl, const QVariantMap &properties);
  void panelPopRequested();
  void stackClearRequested();

public:
  explicit ActionPanelController(ApplicationContext &ctx, QObject *parent = nullptr);

  bool isOpen() const { return m_open; }
  bool hasActions() const;
  bool hasMultipleActions() const;
  QString primaryActionTitle() const;
  QVariantList primaryActionShortcutTokens() const;
  int depth() const { return m_open ? 1 + static_cast<int>(m_submenuStack.size()) : 0; }

  void syncToView(BaseView *view);
  void setActions(std::unique_ptr<ActionPanelState> actions);

  Q_INVOKABLE void toggle(bool fromClick = false);
  Q_INVOKABLE void open();
  Q_INVOKABLE void close();

  void pushPanel(const QUrl &componentUrl, const QVariantMap &properties);
  Q_INVOKABLE void pop();

  Q_INVOKABLE void onPanelPushed(QObject *panel);
  Q_INVOKABLE void onPanelPopped(QObject *currentPanel);

  Q_INVOKABLE bool tryShortcut(int key, int modifiers);

  /**
   * Activate the action the key event is bound to, if any: submenus are opened,
   * regular actions are executed. Enter/return uniformization is handled as part
   * of the shortcut matching.
   */
  bool activateBoundAction(const QKeyEvent *event);

  Q_INVOKABLE bool executePrimaryAction();
  void executeAction(AbstractAction *action);
  void openSubmenu(SubmenuAction *action);

private:
  void openRootPanel();
  void connectView(ActionPanelView *view);
  void clearSubmenuStack();
  void refreshSubmenus();

  ActionPanelView *activeRoot() const;

  ApplicationContext &m_ctx;
  bool m_open = false;
  QObject *m_currentPanel = nullptr;
  QObject *m_connectionGuard = nullptr;
  BaseView *m_activeView = nullptr;
  std::unique_ptr<ActionListView> m_ownedRoot;
  std::vector<ActionPanelView *> m_submenuStack;
  QElapsedTimer m_closedTimer;
};
