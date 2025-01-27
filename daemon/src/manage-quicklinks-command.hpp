#pragma once
#include "app.hpp"
#include "extend/metadata-model.hpp"
#include "navigation-list-view.hpp"
#include "quicklink-actions.hpp"
#include "quicklist-database.hpp"
#include "ui/action_popover.hpp"
#include "ui/test-list.hpp"
#include <memory>
#include <qlabel.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <sys/socket.h>

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

class QuicklinkItem : public StandardListItem2 {
  Q_OBJECT
  std::shared_ptr<Quicklink> link;

public:
  QList<AbstractAction *> createActions() const override {
    auto open = new OpenCompletedQuicklinkAction(link);
    auto edit = new EditQuicklinkAction(link);
    auto duplicate = new DuplicateQuicklinkAction(link);
    auto remove = new RemoveQuicklinkAction(link);

    connect(edit, &EditQuicklinkAction::edited, this, &QuicklinkItem::edited);
    connect(duplicate, &DuplicateQuicklinkAction::duplicated, this, &QuicklinkItem::duplicated);
    connect(remove, &RemoveQuicklinkAction::didExecute, this, &QuicklinkItem::removed);

    return {open, edit, duplicate, remove};
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
      : StandardListItem2(link->name, "", "", link->iconName), link(link) {}

signals:
  void edited() const;
  void removed() const;
  void duplicated() const;
};

class ManageQuicklinksView : public NavigationListView {
  Service<QuicklistDatabase> quicklinkDb;
  QString query;

  void handleRefresh() {
    auto old = list->selected();

    resetItems(query);
    list->selectFrom(old);
  }

  void resetItems(const QString &query) {
    model->beginReset();
    model->beginSection("Quicklinks");

    for (const auto &link : quicklinkDb.list()) {
      if (!link->name.contains(query, Qt::CaseInsensitive)) { continue; }

      auto item = std::make_shared<QuicklinkItem>(link);

      connect(item.get(), &QuicklinkItem::edited, this, &ManageQuicklinksView::handleRefresh);
      connect(item.get(), &QuicklinkItem::duplicated, this, &ManageQuicklinksView::handleRefresh);
      connect(item.get(), &QuicklinkItem::removed, this, &ManageQuicklinksView::handleRefresh);

      model->addItem(item);
    }

    model->endReset();
  }

  void onSearchChanged(const QString &s) override {
    query = s;
    resetItems(s);
  }

  void onMount() override { setSearchPlaceholderText("Browse quicklinks..."); }

public:
  ManageQuicklinksView(AppWindow &app) : NavigationListView(app), quicklinkDb(service<QuicklistDatabase>()) {}
};
