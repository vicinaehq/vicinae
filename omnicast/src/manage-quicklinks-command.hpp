#pragma once
#include "app.hpp"
#include "extend/metadata-model.hpp"
#include "quicklink-actions.hpp"
#include "quicklist-database.hpp"
#include "ui/action_popover.hpp"
#include "ui/omni-list-view.hpp"
#include "ui/omni-list.hpp"
#include <memory>
#include <qlabel.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <sys/socket.h>

class QuicklinkItemDetail : public OmniListView::MetadataDetailModel {
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

class QuicklinkItem : public AbstractDefaultListItem, public OmniListView::IActionnable {
  std::shared_ptr<Quicklink> link;

public:
  QList<AbstractAction *> generateActions() const override {
    auto open = new OpenCompletedQuicklinkAction(link);
    auto edit = new EditQuicklinkAction(link);
    auto duplicate = new DuplicateQuicklinkAction(link);
    auto remove = new RemoveQuicklinkAction(link);

    // connect(edit, &EditQuicklinkAction::edited, this, &QuicklinkItem::edited);
    // connect(duplicate, &DuplicateQuicklinkAction::duplicated, this, &QuicklinkItem::duplicated);
    // connect(remove, &RemoveQuicklinkAction::didExecute, this, &QuicklinkItem::removed);

    return {open, edit, duplicate, remove};
  }

  const QString &name() const { return link->name; }

  std::unique_ptr<CompleterData> createCompleter() const override {
    return std::make_unique<CompleterData>(CompleterData{
        .placeholders = link->placeholders,
        .model = ThemeIconModel{.iconName = link->iconName},
    });
  }

  ItemData data() const override {
    return {
        .icon = link->iconName,
        .name = link->name,
    };
  }

  QWidget *generateDetail() const override {
    auto detailModel = std::make_unique<QuicklinkItemDetail>(link);

    return new OmniListView::SideDetailWidget(*detailModel.get());
  }

  QString id() const override { return QString::number(link->id); }

public:
  QuicklinkItem(const std::shared_ptr<Quicklink> &link) : link(link) {}
};

class ManageQuicklinksView : public OmniListView {
  Service<QuicklistDatabase> quicklinkDb;
  QString query;

  class QuicklinkItemFilter : public OmniList::AbstractItemFilter {
    QString query;

    bool matches(const OmniList::AbstractVirtualItem &item) override {
      auto &linkItem = static_cast<const QuicklinkItem &>(item);

      return linkItem.name().contains(query, Qt::CaseInsensitive);
    }

  public:
    QuicklinkItemFilter(const QString &query) : query(query) {}
  };

  void onMount() override {
    setSearchPlaceholderText("Browse quicklinks...");

    list->beginUpdate();

    for (auto &link : quicklinkDb.list()) {
      list->addItem(std::make_unique<QuicklinkItem>(link));
    }

    list->commitUpdate();
  }

  void onSearchChanged(const QString &s) override {
    list->setFilter(std::make_unique<QuicklinkItemFilter>(s));
  }

public:
  ManageQuicklinksView(AppWindow &app) : OmniListView(app), quicklinkDb(service<QuicklistDatabase>()) {}
};
