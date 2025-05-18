#pragma once
#include "app.hpp"
#include "bookmark-actions.hpp"
#include "bookmark-service.hpp"
#include "extend/metadata-model.hpp"
#include "service-registry.hpp"
#include "ui/declarative-omni-list-view.hpp"
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

class QuicklinkItem : public AbstractDefaultListItem, public DeclarativeOmniListView::IActionnable {
  std::shared_ptr<Bookmark> link;

public:
  QList<AbstractAction *> generateActions() const override {
    auto open = new OpenBookmarkAction(link);
    auto edit = new EditBookmarkAction(link);
    auto duplicate = new DuplicateBookmarkAction(link);
    auto remove = new RemoveBookmarkAction(link);

    return {open, edit, duplicate, remove};
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

  QString id() const override { return QString::number(link->id()); }

public:
  QuicklinkItem(const std::shared_ptr<Bookmark> &link) : link(link) {}
};

class ManageQuicklinksView : public DeclarativeOmniListView {
  QString query;

  void onMount() override { setSearchPlaceholderText("Browse quicklinks..."); }

  bool doesUseNewModel() const override { return true; }

  void render(const QString &s) override {
    auto rootItemManager = ServiceRegistry::instance()->rootItemManager();
    auto bookmarkService = ServiceRegistry::instance()->bookmarks();
    auto bookmarkProvider = rootItemManager->provider("bookmarks");

    if (!bookmarkProvider) { return; }

    auto bookmarks =
        bookmarkService->bookmarks() |
        std::views::filter([s](auto bk) { return bk->name().contains(s, Qt::CaseInsensitive); }) |
        std::views::transform([](auto bk) { return std::make_unique<QuicklinkItem>(bk); });

    auto &section = list->addSection("Bookmarks");

    for (auto bk : bookmarks) {
      section.addItem(std::move(bk));
    }
  }

public:
  ManageQuicklinksView(AppWindow &app) : DeclarativeOmniListView(app) {}
};
