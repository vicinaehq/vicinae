#pragma once

#include "command.hpp"
#include "omni-icon.hpp"
#include "process-manager-service.hpp"
#include "ui/action_popover.hpp"
#include "ui/declarative-omni-list-view.hpp"
#include "ui/omni-list.hpp"
#include "view.hpp"
#include <csignal>
#include <QPointer>
#include <qnamespace.h>

class KillProcessAction : public AbstractAction {
  int pid;

  void execute(AppWindow &app) override { kill(pid, SIGKILL); }

public:
  KillProcessAction(int pid) : AbstractAction("Kill process", BuiltinOmniIconUrl("droplets")), pid(pid) {}
};

class ManageProcessesMainView : public DeclarativeOmniListView {
  Service<ProcessManagerService> processManager;

  class ProcListItem : public AbstractDefaultListItem, public DeclarativeOmniListView::IActionnable {
    ProcessInfo info;
    int idx;

    virtual QList<AbstractAction *> generateActions() const override {
      return {new CopyTextAction("Copy pid", QString::number(info.pid)), new KillProcessAction(info.pid)};
    }

    QString id() const override { return QString::number(info.pid); }

    ItemData data() const override {
      return {.iconUrl = BuiltinOmniIconUrl("bar-chart"),
              .name = info.comm,
              .category = "",
              .kind = QString::number(info.pid)};
    }

  public:
    const ProcessInfo &proc() const { return info; }

    ProcListItem(const ProcessInfo &info) : info(info) {}

    ~ProcListItem() {}
  };

  ItemList generateList(const QString &s) override {
    auto ps = processManager.list();
    ItemList list;

    list.push_back(std::make_unique<OmniList::VirtualSection>("Running processes"));

    for (const auto &proc : ps) {
      if (!proc.comm.contains(s, Qt::CaseInsensitive)) continue;

      auto item = std::make_unique<ProcListItem>(proc);

      list.push_back(std::move(item));
    }

    return list;
  }

  void onMount() override { setSearchPlaceholderText("Search processes..."); }

public:
  ManageProcessesMainView(AppWindow &app)
      : DeclarativeOmniListView(app), processManager(service<ProcessManagerService>()) {}
};

class ManageProcessesCommand : public ViewCommand {
  View *load(AppWindow &app) override { return new ManageProcessesMainView(app); }
};
