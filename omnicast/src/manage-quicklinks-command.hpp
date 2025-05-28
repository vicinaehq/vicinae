#pragma once
#include "base-view.hpp"
#include "actions/bookmark/bookmark-actions.hpp"
#include "bookmark-service.hpp"
#include "extend/metadata-model.hpp"
#include "service-registry.hpp"
#include "ui/omni-list-view.hpp"
#include "ui/omni-list.hpp"
#include <memory>
#include <qlabel.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <ranges>
#include <sys/socket.h>

class QuicklinkItemDetail : public OmniListView::MetadataDetailModel {
  std::shared_ptr<Bookmark> link;

  QWidget *createView() const override {
    auto widget = new QWidget();
    auto layout = new QHBoxLayout();

    layout->setAlignment(Qt::AlignTop);
    layout->addWidget(new QLabel(link->url()));
    widget->setLayout(layout);

    return widget;
  }

  MetadataModel createMetadata() const override {
    QList<MetadataItem> items;

    items << MetadataLabel{
        .text = link->name(),
        .title = "Name",
    };
    items << MetadataLabel{
        .text = link->app(),
        .title = "Application",
    };
    items << MetadataLabel{
        .text = QString::number(0),
        .title = "Opened",
    };

    /*
if (link->lastUsedAt) {
  items << MetadataLabel{
      .text = link->lastUsedAt->toString(),
      .title = "Last used at",
  };
}
    */

    return {.children = items};
  }

public:
  QuicklinkItemDetail(const std::shared_ptr<Bookmark> &quicklink) : link(quicklink) {}
};

class QuicklinkItem : public AbstractDefaultListItem, public ListView::Actionnable {
  std::shared_ptr<Bookmark> link;

public:
  ActionPanelView *actionPanel() const override {
    auto panel = new ActionPanelStaticListView;

    auto open = new OpenCompletedBookmarkAction(link);
    auto edit = new EditBookmarkAction(link);
    auto duplicate = new DuplicateBookmarkAction(link);
    auto remove = new RemoveBookmarkAction(link);

    panel->addAction(open);
    panel->addAction(edit);
    panel->addAction(duplicate);
    panel->addSection();
    panel->addAction(remove);

    return panel;
  }

  std::unique_ptr<CompleterData> createCompleter() const override {
    ArgumentList args;

    for (const auto &barg : link->arguments()) {
      CommandArgument cmdArg;

      cmdArg.type = CommandArgument::Text;
      cmdArg.required = barg.defaultValue.isEmpty();
      cmdArg.placeholder = barg.name;
      args.emplace_back(cmdArg);
    }

    return std::make_unique<CompleterData>(CompleterData{.arguments = args});
  }

  ItemData data() const override {
    return {
        .iconUrl = link->icon(),
        .name = link->name(),
    };
  }

  QWidget *generateDetail() const override {
    auto detailModel = std::make_unique<QuicklinkItemDetail>(link);

    return new OmniListView::SideDetailWidget(*detailModel.get());
  }

  QString generateId() const override { return QString::number(link->id()); }

public:
  QuicklinkItem(const std::shared_ptr<Bookmark> &link) : link(link) {}
};

class ManageQuicklinksView : public ListView {
  void renderList(const QString &s, OmniList::SelectionPolicy policy = OmniList::SelectFirst) {
    auto bookmarkService = ServiceRegistry::instance()->bookmarks();
    auto bookmarks =
        bookmarkService->bookmarks() |
        std::views::filter([s](auto bk) { return bk->name().contains(s, Qt::CaseInsensitive); }) |
        std::views::transform([](auto bk) { return std::make_unique<QuicklinkItem>(bk); });

    m_list->beginResetModel();

    auto &section = m_list->addSection("Bookmarks");

    for (auto bk : bookmarks) {
      section.addItem(std::move(bk));
    }

    m_list->endResetModel(policy);
  }

  void onBookmarkRemoved() { renderList(searchText(), OmniList::PreserveSelection); }

  void onBookmarkSaved() { renderList(searchText(), OmniList::PreserveSelection); }

  void onSearchChanged(const QString &s) override { renderList(s); }

  void initialize() override { onSearchChanged(""); }

public:
  ManageQuicklinksView() {
    auto bookmarkService = ServiceRegistry::instance()->bookmarks();

    setSearchPlaceholderText("Browse quicklinks...");
    setNavigationTitle("Manage Bookmarks");

    connect(bookmarkService, &BookmarkService::bookmarkSaved, this, &ManageQuicklinksView::onBookmarkSaved);
    connect(bookmarkService, &BookmarkService::bookmarkRemoved, this,
            &ManageQuicklinksView::onBookmarkRemoved);
  }
};
