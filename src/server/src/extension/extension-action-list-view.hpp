#pragma once
#include "extension/extension-action-panel-builder.hpp"
#include "ui/action-pannel/action-list-view.hpp"

class ExtensionActionListView : public ActionListView {
  Q_OBJECT

public:
  ExtensionActionListView(ExtensionActionPanelBuilder::NotifyFn notify,
                          const QString &onSearchTextChangeHandler, QObject *parent = nullptr);

protected:
  ActionListView *createSubmenuChild(SubmenuAction *action) override;

private:
  ExtensionActionPanelBuilder::NotifyFn m_notify;
  QString m_onSearchTextChangeHandler;
};
