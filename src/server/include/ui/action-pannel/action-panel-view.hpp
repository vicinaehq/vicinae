#pragma once
#include <QObject>
#include <QString>
#include <QUrl>
#include <QVariantMap>

class AbstractAction;
class QKeyEvent;

/**
 * Base class for any view rendered as a frame inside the action panel stack.
 *
 * An action panel view is the action-panel-side analogue of `BaseView`: it owns
 * its own state, has a lifecycle, and renders a QML component. Subclasses
 * supply the component to render and respond to lifecycle events.
 *
 * The default subclass `ActionListView` wraps the existing `ActionPanelState`
 * and renders the standard fuzzy-searchable action list. Custom subclasses can
 * implement entirely different UIs (date pickers, color pickers, embedded
 * forms, etc.) without going through the action list machinery.
 *
 * Step 1 note: this interface is in place but the controller does not yet push
 * views onto the QML stack — it still operates on `ActionPanelState` snapshots.
 * Future commits will route push/pop/reconciliation through this interface.
 */
class ActionPanelView : public QObject {
  Q_OBJECT

public:
  explicit ActionPanelView(QObject *parent = nullptr) : QObject(parent) {}
  ~ActionPanelView() override = default;

  /**
   * Stable identity used for reconciliation across re-renders of the parent
   * view. Native code can set this explicitly via `setId()`. Extension views
   * inherit the React stable id automatically. An empty id means the view
   * is anonymous and will be popped on parent re-render.
   */
  QString id() const { return m_id; }
  void setId(const QString &id) { m_id = id; }

  /**
   * Lifecycle hooks called by the action panel controller. Default
   * implementations are no-ops.
   *
   * Order: onMount → onActivate → (onDeactivate when child pushed above /
   * onActivate again when child popped) → onUnmount.
   */
  virtual void onMount() {}
  virtual void onActivate() {}
  virtual void onDeactivate() {}
  virtual void onUnmount() {}

  /**
   * QML component URL the controller should push when this view is
   * activated. Subclasses point at their own QML file.
   */
  virtual QUrl componentUrl() const = 0;

  /**
   * Properties passed to the QML component when it is instantiated. Typically
   * includes a model object the view exposes for binding.
   */
  virtual QVariantMap componentProps() const = 0;

  /**
   * Resolve a key event against this view's actions. Returns nullptr if no
   * action is bound. Used by the controller's shortcut routing path.
   */
  virtual AbstractAction *findBoundAction(const QKeyEvent *event) const { return nullptr; }

  /**
   * The action that should run when the user presses Enter (or the equivalent
   * primary-action shortcut). Returns nullptr if there is no primary action.
   */
  virtual AbstractAction *primaryAction() const { return nullptr; }

signals:
  /**
   * Emitted when the view's content has changed and the QML side should
   * refresh. Subclasses decide what counts as "content".
   */
  void contentChanged();

private:
  QString m_id;
};
