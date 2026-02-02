#pragma once

#include "command.hpp"
#include "omni-icon.hpp"
#include "process-manager-service.hpp"
#include "theme.hpp"
#include "ui/action_popover.hpp"
#include "ui/declarative-omni-list-view.hpp"
#include "ui/default-list-item-widget/default-list-item-widget.hpp"
#include "ui/omni-list/omni-list.hpp"
#include "view.hpp"
#include <csignal>
#include <QPointer>
#include <qnamespace.h>

class KillProcessAction : public AbstractAction {
  int pid;

  void execute(AppWindow &app) override { kill(pid, SIGKILL); }

public:
  KillProcessAction(int pid) : AbstractAction("Kill process", ImageURL::builtin("droplets")), pid(pid) {}
};

class ManageProcessesMainView : public DeclarativeOmniListView {
  Service<ProcessManagerService> processManager;

  class ProcListItem : public AbstractDefaultListItem, public DeclarativeOmniListView::IActionnable {
    ProcessInfo info;
    int idx;

    virtual QList<AbstractAction *> generateActions() const override {
      return {new CopyTextAction("Copy pid", QString::number(info.pid)), new KillProcessAction(info.pid)};
    }

    AccessoryList generateAccessories() const {
      AccessoryList list;

      if (info.comm.contains("systemd", Qt::CaseInsensitive)) {
        list.push_back({
            .text = "Cunt",
            .color = ColorTint::Blue,
            .fillBackground = true,
            .icon = ImageURL::builtin("archlinux"),
        });
      }

      if (info.comm.contains("systemd", Qt::CaseInsensitive)) {
        list.push_back({
            .text = "Magic",
            .color = ColorTint::Magenta,
            .fillBackground = true,
            .icon = ImageURL::builtin("wand"),
        });
      }

      list.push_back({
          .text = "Stornijop",
          .color = ColorTint::Green,
          .fillBackground = true,
          .icon = ImageURL::builtin("cplusplus"),
      });
      list.push_back({.text = "Chad",
                      .color = ColorTint::Yellow,
                      .fillBackground = true,
                      .icon = ImageURL::builtin("zig")});

      return list;
    }

    QString id() const override { return QString::number(info.pid); }

    ItemData data() const override {
      return {.iconUrl = ImageURL::builtin("bar-chart"),
              .name = info.comm,
              .category = "",
              .accessories = generateAccessories()};
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
