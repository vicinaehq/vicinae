#pragma once

#include "app.hpp"
#include "create-quicklink-command.hpp"
#include "quicklist-database.hpp"
#include "ui/action_popover.hpp"

struct OpenQuicklinkAction : public AbstractAction {
  std::shared_ptr<Quicklink> link;
  QList<QString> args;

  void execute(AppWindow &app) override {
    auto linkApp = app.appDb->getById(link->app);

    if (!linkApp) {
      app.statusBar->setToast("No app with id " + link->app, ToastPriority::Danger);
      return;
    }

    QString url = link->url;

    for (const auto &arg : args)
      url = url.arg(arg);

    if (!app.appDb->launch(*linkApp.get(), {url})) {
      app.statusBar->setToast("Failed to launch app", ToastPriority::Danger);
      return;
    }

    app.closeWindow(true);
  }

  void setArgs(const QList<QString> &args) { this->args = args; }

  OpenQuicklinkAction(const std::shared_ptr<Quicklink> &link, const QList<QString> &args = {})
      : AbstractAction("Open link", ThemeIconModel{.iconName = ":icons/link.svg"}), link(link), args(args) {}
};

struct EditQuicklinkAction : public AbstractAction {
  std::shared_ptr<Quicklink> link;
  QList<QString> args;

  void execute(AppWindow &app) override {
    auto view = new EditCommandQuicklinkView(app, *link);

    emit app.pushView(
        view, {.navigation = NavigationStatus{
                   .title = "Edit link", .icon = ThemeIconModel{.iconName = ":assets/icons/quicklink.png"}}});
  }

  void setArgs(const QList<QString> &args) { this->args = args; }

  EditQuicklinkAction(const std::shared_ptr<Quicklink> &link, const QList<QString> &args = {})
      : AbstractAction("Edit link", ThemeIconModel{.iconName = ":icons/pencil.svg"}), link(link), args(args) {
  }
};

struct RemoveQuicklinkAction : public AbstractAction {
  std::shared_ptr<Quicklink> link;
  QList<QString> args;

public:
  void execute(AppWindow &app) override {
    bool removeResult = app.quicklinkDatabase->removeOne(link->id);

    if (removeResult) {
      app.statusBar->setToast("Removed link");
    } else {
      app.statusBar->setToast("Failed to remove link", ToastPriority::Danger);
    }
  }

  RemoveQuicklinkAction(const std::shared_ptr<Quicklink> &link)
      : AbstractAction("Remove link", ThemeIconModel{.iconName = ":icons/trash.svg"}), link(link) {}
};

struct DuplicateQuicklinkAction : public AbstractAction {
  std::shared_ptr<Quicklink> link;
  QList<QString> args;

  void execute(AppWindow &app) override {
    auto view = new DuplicateQuicklinkCommandView(app, *link);

    emit app.pushView(view, {.navigation = NavigationStatus{
                                 .title = "Duplicate link",
                                 .icon = ThemeIconModel{.iconName = ":assets/icons/quicklink.png"}}});
  }

  void setArgs(const QList<QString> &args) { this->args = args; }

  DuplicateQuicklinkAction(const std::shared_ptr<Quicklink> &link, const QList<QString> &args = {})
      : AbstractAction("Duplicate link", ThemeIconModel{.iconName = ":icons/duplicate.svg"}), link(link),
        args(args) {}
};

struct OpenCompletedQuicklinkAction : public OpenQuicklinkAction {
  void execute(AppWindow &app) {
    if (app.topBar->quickInput) { setArgs(app.topBar->quickInput->collectArgs()); }

    OpenQuicklinkAction::execute(app);
  }

  OpenCompletedQuicklinkAction(const std::shared_ptr<Quicklink> &link, const QList<QString> &args = {})
      : OpenQuicklinkAction(link) {}
};
