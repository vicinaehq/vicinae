#pragma once
#include "ui/action-panel/action-panel-view.hpp"
#include <memory>

class ActionPanelState;
class ActionPanelModel;

class ActionListView : public ActionPanelView {
  Q_OBJECT

public:
  explicit ActionListView(QObject *parent = nullptr);
  ~ActionListView() override;

  void adoptState(std::unique_ptr<ActionPanelState> state);
  const ActionPanelState *state() const { return m_state.get(); }

  QUrl componentUrl() const override;
  QVariantMap componentProps() override;
  AbstractAction *findBoundAction(const Keyboard::KeyPress &press) const override;
  AbstractAction *primaryAction() const override;
  std::shared_ptr<AbstractAction> retainAction(AbstractAction *action) const override;
  bool hasActions() const override;
  bool hasMultipleActions() const override;
  void resetState() override;

protected:
  ActionPanelModel *model() const { return m_model; }

  /**
   * Called with the action the user picked. The default hands it over for
   * execution, which closes the panel: override to act while staying open.
   */
  virtual void activate(AbstractAction *action) { emit actionExecuted(action); }

private:
  std::unique_ptr<ActionPanelState> m_state;
  ActionPanelModel *m_model = nullptr;
};
