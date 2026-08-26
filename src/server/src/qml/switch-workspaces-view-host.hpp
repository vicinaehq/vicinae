#pragma once
#include "list-view-host.hpp"
#include "switch-workspaces-model.hpp"

class SwitchWorkspacesViewHost : public ListViewHost {
  Q_OBJECT

public:
  void initialize() override;
  void loadInitialData() override;
  void textChanged(const QString &text) override;

private:
  void refreshWindows();

  SwitchWorkspacesSection m_section{tr("Open Workspaces")};
};
