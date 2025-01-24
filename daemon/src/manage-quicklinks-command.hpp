#pragma once
#include "app.hpp"
#include "extend/metadata-model.hpp"
#include "navigation-list-view.hpp"
#include "quicklist-database.hpp"
#include "ui/action_popover.hpp"
#include "ui/list-view.hpp"
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
  OpenLinkAction(const std::shared_ptr<Quicklink> &link, const std::shared_ptr<DesktopExecutable> &app)
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
      : AbstractAction("Remove link", {.iconName = link->iconName}), link(link) {}

signals:
  void linkRemoved();
};

class QuicklinkItemDetail : public AbstractNativeListItemDetail {
  std::shared_ptr<Quicklink> link;

  QWidget *createView() const override {
    auto widget = new QWidget();
    auto layout = new QHBoxLayout();

    layout->setAlignment(Qt::AlignTop);
    layout->addWidget(new QLabel(link->rawUrl));
    widget->setLayout(layout);

    return widget;
  }

  MetadataModel createMetadata() const override {
    QList<MetadataItem> items;

    items << MetadataLabel{
        .text = link->name,
        .title = "Name",
    };
    items << MetadataLabel{
        .text = link->app,
        .title = "Application",
    };
    items << MetadataLabel{
        .text = QString::number(link->openCount),
        .title = "Opened",
    };

    if (link->lastUsedAt) {
      items << MetadataLabel{
          .text = link->lastUsedAt->toString(),
          .title = "Last used at",
      };
    }

    return {.children = items};
  }

public:
  QuicklinkItemDetail(const std::shared_ptr<Quicklink> &quicklink) : link(quicklink) {}
};

class QuicklinkItem : public StandardListItem {
  Q_OBJECT
  std::shared_ptr<Quicklink> link;

public:
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
        .placeholders = link->placeholders,
        .model = ThemeIconModel{.iconName = link->iconName},
    });
  }

  std::unique_ptr<AbstractNativeListItemDetail> createDetail() const override {
    return std::make_unique<QuicklinkItemDetail>(link);
  }

  size_t id() const override { return qHash(link->id); }

public:
  QuicklinkItem(const std::shared_ptr<Quicklink> &link)
      : StandardListItem(link->name, "", "Quicklink", ThemeIconModel{.iconName = link->iconName}),
        link(link) {}

signals:
  void removed() const;
};

class ManageQuicklinksView : public NavigationListView {
  Service<QuicklistDatabase> quicklinkDb;

  void onSearchChanged(const QString &s) override {
    model->beginReset();
    model->beginSection("Quicklinks");

    for (const auto &link : quicklinkDb.list()) {
      if (!link->name.contains(s, Qt::CaseInsensitive)) { continue; }

      auto item = std::make_shared<QuicklinkItem>(link);

      connect(item.get(), &QuicklinkItem::removed, this,
              [this, item]() { qDebug() << "removed by id" << item.get(); });

      model->addItem(item);
    }

    model->endReset();
  }

  void onMount() override { setSearchPlaceholderText("Browse quicklinks..."); }

public:
  ManageQuicklinksView(AppWindow &app) : NavigationListView(app), quicklinkDb(service<QuicklistDatabase>()) {}
};
