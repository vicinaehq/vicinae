#pragma once

#include "app.hpp"
#include "command.hpp"
#include "navigation-list-view.hpp"
#include "process-manager-service.hpp"
#include "ui/test-list.hpp"
#include "ui/virtual-list.hpp"
#include "view.hpp"
#include <qnamespace.h>

class ManageProcessesMainView : public NavigationListView {
  Service<ProcessManagerService> processManager;

  class ProcListItem : public AbstractNativeListItem {
    ProcessInfo info;
    int idx;

    QWidget *createItem() const override {
      return new ListItemWidget(
          ImageViewer::createFromModel(ThemeIconModel{.iconName = "application-x-executable"}, {25, 25}),
          info.comm, "", QString::number(idx));
    }

    int height() const override { return 40; }

  public:
    ProcListItem(const ProcessInfo &info, int idx) : info(info), idx(idx) {}
  };

  void onSearchChanged(const QString &text) override {
    model->beginReset();
    model->beginSection("Running processes");

    size_t limit = 0;
    size_t i = 0;

    for (const auto &proc : processManager.list()) {
      ++i;
      if (!proc.comm.contains(text, Qt::CaseInsensitive)) continue;

      model->addItem(std::make_shared<ProcListItem>(proc, i - 1));
      ++limit;
    }

    model->endReset();
  }

public:
  ManageProcessesMainView(AppWindow &app)
      : NavigationListView(app), processManager(service<ProcessManagerService>()) {}
};

class ManageProcessesCommand : public ViewCommand {
  View *load(AppWindow &app) override { return new ManageProcessesMainView(app); }
};
