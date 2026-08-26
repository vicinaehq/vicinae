#pragma once
#include "extension/extension-action-panel-builder.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/action-pannel/action-list-view.hpp"

class ExtensionActionListView : public ActionListView {
  Q_OBJECT

public:
  ExtensionActionListView(ExtensionActionPanelBuilder::NotifyFn notify,
                          const QString &onSearchTextChangeHandler, QObject *parent = nullptr);

private:
  ExtensionActionPanelBuilder::NotifyFn m_notify;
  QString m_onSearchTextChangeHandler;
};

/**
 * Submenu backed by an extension-provided model. The state is rebuilt from the
 * model on demand, which `ActionPanelController::refreshSubmenus` relies on to
 * reconcile open submenus when the extension re-renders.
 */
class ExtensionSubmenuAction : public ListSubmenuAction {
public:
  ExtensionSubmenuAction(ActionPannelSubmenuPtr model, ExtensionActionPanelBuilder::NotifyFn notify,
                         ExtensionActionPanelBuilder::SubmitFn submit);

  void onOpen(ApplicationContext *ctx) override;
  std::unique_ptr<ActionPanelState> buildState(ApplicationContext *ctx) const override;
  ActionPanelView *createView(ApplicationContext *ctx, QObject *parent) override;

private:
  ActionPannelSubmenuPtr m_model;
  ExtensionActionPanelBuilder::NotifyFn m_notify;
  ExtensionActionPanelBuilder::SubmitFn m_submit;
};
