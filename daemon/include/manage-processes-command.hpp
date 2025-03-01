#pragma once

#include "command.hpp"
#include "process-manager-service.hpp"
#include "ui/action_popover.hpp"
#include "ui/omni-list-view.hpp"
#include "ui/omni-list.hpp"
#include "view.hpp"
#include <csignal>
#include <QPointer>
#include <qnamespace.h>

class KillProcessAction : public AbstractAction {
  int pid;

  void execute(AppWindow &app) override { kill(pid, SIGKILL); }

public:
  KillProcessAction(int pid)
      : AbstractAction("Kill process", ThemeIconModel{.iconName = ":icons/droplets.svg"}), pid(pid) {}
};

class ManageProcessesMainView : public OmniListView {
  Service<ProcessManagerService> processManager;

  class ProcListItem : public OmniListView::AbstractActionnableItem {
    ProcessInfo info;
    int idx;

    virtual QList<AbstractAction *> generateActions() const override {
      return {new CopyTextAction("Copy pid", QString::number(info.pid)), new KillProcessAction(info.pid)};
    }

    QString id() const override { return QString::number(info.pid); }

    ItemData data() const override {
      return {.icon = "xterm", .name = info.comm, .category = "", .kind = QString::number(info.pid)};
    }

  public:
    const ProcessInfo &proc() const { return info; }

    ProcListItem(const ProcessInfo &info) : info(info) {}

    ~ProcListItem() {}
  };

  void executeSearch(ItemList &list, const QString &s) override {
    auto ps = processManager.list();

    list.push_back(std::make_unique<OmniList::VirtualSection>("Running processes"));

    for (const auto &proc : ps) {
      if (!proc.comm.contains(s, Qt::CaseInsensitive)) continue;

      auto item = std::make_unique<ProcListItem>(proc);

      list.push_back(std::move(item));
    }
  }

  void onMount() override { setSearchPlaceholderText("Search processes..."); }

public:
  ManageProcessesMainView(AppWindow &app)
      : OmniListView(app), processManager(service<ProcessManagerService>()) {
    widget = list;
  }
};

class ManageProcessesCommand : public ViewCommand {
  View *load(AppWindow &app) override { return new ManageProcessesMainView(app); }
};
