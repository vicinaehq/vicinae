#pragma once
#include "app.hpp"
#include "navigation-list-view.hpp"
#include "quicklist-database.hpp"
#include "ui/action_popover.hpp"
#include "ui/test-list.hpp"
#include "ui/toast.hpp"
#include <memory>
#include <qlabel.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <sys/socket.h>

class OpenLinkAction : public AbstractAction {
  Q_OBJECT
  std::shared_ptr<Quicklink> link;

  void execute(AppWindow &app) override {
    // bool removed = app.quicklinkDatabase->removeOne(link->id);
  }

public:
  OpenLinkAction(const std::shared_ptr<Quicklink> &link,
                 const std::shared_ptr<DesktopExecutable> &app)
      : AbstractAction("Open link", {.iconName = link->iconName}) {}

signals:
  void linkRemoved();
};

class RemoveLinkAction : public AbstractAction {
  Q_OBJECT
  std::shared_ptr<Quicklink> link;

  void execute(AppWindow &app) override {
    bool removed = app.quicklinkDatabase->removeOne(link->id);

    if (removed) {
      app.statusBar->setToast("Removed link");
      emit linkRemoved();
    } else {
      app.statusBar->setToast("Failed to remove link", ToastPriority::Danger);
    }
  }

public:
  RemoveLinkAction(const std::shared_ptr<Quicklink> &link)
      : AbstractAction("Remove link", {.iconName = link->iconName}),
        link(link) {}

signals:
  void linkRemoved();
};

class QuicklinkItem : public AbstractNativeListItem {
  Q_OBJECT
  std::shared_ptr<Quicklink> link;

public:
  QWidget *createItem() const override {
    return new ListItemWidget(
        ImageViewer::createFromModel(ThemeIconModel{.iconName = link->iconName},
                                     {25, 25}),
        link->name, "", "Quicklink");
  }

  QList<AbstractAction *> createActions() const override {
    auto remove = new RemoveLinkAction(link);

    connect(remove, &RemoveLinkAction::linkRemoved, this, [this]() {
      qDebug() << "hello removed";
      emit removed();
    });

    return {remove};
  }

  std::unique_ptr<CompleterData> createCompleter() const override {
    return std::make_unique<CompleterData>(CompleterData{
        .placeholders = {"query"},
        .model = ThemeIconModel{.iconName = link->iconName},
    });
  }

  QWidget *createDetail() const override { return new QLabel(link->name); }

  size_t id() const override { return qHash(link->id); }

public:
  QuicklinkItem(const std::shared_ptr<Quicklink> &link)
      : AbstractNativeListItem(), link(link) {}

signals:
  void removed() const;
};

class ManageQuicklinksView : public NavigationListView {
  Service<QuicklistDatabase> quicklinkDb;

  void onSearchChanged(const QString &s) override {
    model->beginReset();
    model->beginSection("Quicklinks");

    for (const auto &link : quicklinkDb.list()) {
      if (!link->name.contains(s, Qt::CaseInsensitive)) {
        continue;
      }

      auto item = std::make_shared<QuicklinkItem>(link);

      connect(item.get(), &QuicklinkItem::removed, this,
              [this, item]() { qDebug() << "removed by id" << item.get(); });

      model->addItem(item);
    }

    model->endReset();
  }

  void onMount() override { setSearchPlaceholderText("Browse quicklinks..."); }

public:
  ManageQuicklinksView(AppWindow &app)
      : NavigationListView(app), quicklinkDb(service<QuicklistDatabase>()) {}
};
