#pragma once
#include "ui/views/list-view-host.hpp"
#include "builtins/wm/switch-workspaces-model.hpp"

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
