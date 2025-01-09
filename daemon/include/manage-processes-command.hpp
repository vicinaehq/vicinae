#pragma once

#include "app.hpp"
#include "command.hpp"
#include "navigation-list-view.hpp"
#include "process-manager-service.hpp"
#include "ui/test-list.hpp"
#include "ui/virtual-list.hpp"
#include "view.hpp"
#include <qnamespace.h>
#include <type_traits>

class ManageProcessesMainView : public View {
  Service<ProcessManagerService> processManager;
  VirtualListWidget *list;

  class ProcListItem : public AbstractNativeListItem {
    ProcessInfo info;
    int idx;

    QWidget *createItem() const override {
      return new ListItemWidget(
          ImageViewer::createFromModel(
              ThemeIconModel{.iconName = "application-x-executable"}, {25, 25}),
          info.comm, "", QString::number(idx));
    }

  public:
    ProcListItem(const ProcessInfo &info, int idx) : info(info), idx(idx) {}
  };

  void onSearchChanged(const QString &text) override {
    // model->beginReset();
    // model->beginSection("Running processes");

    QList<std::shared_ptr<AbstractNativeListItem>> items;

    size_t limit = 0;
    size_t i = 0;

    for (const auto &proc : processManager.list()) {
      ++i;
      if (limit >= 500)
        break;
      if (!proc.comm.contains(text, Qt::CaseInsensitive))
        continue;

      items << std::make_shared<ProcListItem>(proc, i - 1);
      ++limit;
    }

    list->setItems(items);
  }

public:
  ManageProcessesMainView(AppWindow &app)
      : View(app), list(new VirtualListWidget),
        processManager(service<ProcessManagerService>()) {
    widget = list;
  }
};

class ManageProcessesCommand : public ViewCommand {
  View *load(AppWindow &app) override {
    return new ManageProcessesMainView(app);
  }
};
