#pragma once

#include "app.hpp"
#include "command.hpp"
#include "grid-view.hpp"
#include "process-manager-service.hpp"
#include "ui/action_popover.hpp"
#include "ui/list-view.hpp"
#include "view.hpp"
#include <csignal>

class KillProcessAction : public AbstractAction {
  int pid;

  void execute(AppWindow &app) override { kill(pid, SIGKILL); }

public:
  KillProcessAction(int pid)
      : AbstractAction("Kill process", ThemeIconModel{.iconName = ":icons/droplets.svg"}), pid(pid) {}
};

class ManageProcessesMainView : public GridView {
  Service<ProcessManagerService> processManager;

  class ProcListItem : public SimpleListGridItem {
    ProcessInfo info;
    int idx;

    virtual QList<AbstractAction *> createActions() const override {
      return {new CopyTextAction("Copy pid", QString::number(info.pid)), new KillProcessAction(info.pid)};
    }

    int key() const override { return info.pid; }

  public:
    ProcListItem(const ProcessInfo &info, int idx)
        : SimpleListGridItem("application-x-executable", info.comm, "", QString::number(info.pid)),
          info(info), idx(idx) {}
  };

  void onSearchChanged(const QString &text) override {
    grid->clear();

    auto processes = grid->section("Running processes");
    size_t limit = 0;
    size_t i = 0;

    for (const auto &proc : processManager.list()) {
      ++i;
      if (!proc.comm.contains(text, Qt::CaseInsensitive)) continue;

      processes->addItem(new ProcListItem(proc, i - 1));
      ++limit;
    }

    grid->updateLayout();
    grid->setSelected(0);
  }

  void onMount() override { setSearchPlaceholderText("Search processes..."); }

public:
  ManageProcessesMainView(AppWindow &app) : GridView(app), processManager(service<ProcessManagerService>()) {
    grid->setSpacing(0);
    grid->setMargins(10, 5, 10, 5);
  }
};

class ManageProcessesCommand : public ViewCommand {
  View *load(AppWindow &app) override { return new ManageProcessesMainView(app); }
};
