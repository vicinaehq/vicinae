#pragma once
#include "ui/action-pannel/action-panel-view.hpp"
#include <memory>

class ActionPanelState;
class ActionPanelModel;
class SubmenuAction;

class ActionListView : public ActionPanelView {
  Q_OBJECT

public:
  explicit ActionListView(QObject *parent = nullptr);
  ~ActionListView() override;

  void adoptState(std::unique_ptr<ActionPanelState> state);
  void activateSubmenu(SubmenuAction *action);
  const ActionPanelState *state() const { return m_state.get(); }

  QUrl componentUrl() const override;
  QVariantMap componentProps() override;
  AbstractAction *findBoundAction(const QKeyEvent *event) const override;
  AbstractAction *primaryAction() const override;
  std::shared_ptr<AbstractAction> retainAction(AbstractAction *action) const override;
  bool hasActions() const override;
  bool hasMultipleActions() const override;
  void resetState() override;

protected:
  virtual ActionListView *createSubmenuChild(SubmenuAction *action);
  ActionPanelModel *model() const { return m_model; }

private:
  std::unique_ptr<ActionPanelState> m_state;
  ActionPanelModel *m_model = nullptr;
};
