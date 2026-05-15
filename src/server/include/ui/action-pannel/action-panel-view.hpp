#pragma once
#include <QObject>
#include <QString>
#include <QUrl>
#include <QVariantMap>
#include <memory>

class AbstractAction;
class QKeyEvent;

class ActionPanelView : public QObject {
  Q_OBJECT

public:
  explicit ActionPanelView(QObject *parent = nullptr) : QObject(parent) {}
  ~ActionPanelView() override = default;

  QString id() const { return m_id; }
  void setId(const QString &id) { m_id = id; }

  virtual void onMount() {}
  virtual void onActivate() {}
  virtual void onDeactivate() {}
  virtual void onUnmount() {}

  virtual QUrl componentUrl() const = 0;
  virtual QVariantMap componentProps() = 0;

  virtual AbstractAction *findBoundAction(const QKeyEvent *event) const { return nullptr; }
  virtual AbstractAction *primaryAction() const { return nullptr; }
  virtual std::shared_ptr<AbstractAction> retainAction(AbstractAction *action) const { return nullptr; }
  virtual bool hasActions() const { return false; }
  virtual bool hasMultipleActions() const { return false; }
  virtual void resetState() {}

signals:
  void contentChanged();
  void actionExecuted(AbstractAction *action);
  void closeRequested();
  void pushViewRequested(ActionPanelView *view);

private:
  QString m_id;
};
