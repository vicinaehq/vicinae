#pragma once
#include "base-view.hpp"
#include "actions/bookmark/bookmark-actions.hpp"
#include "common.hpp"
#include "services/bookmark/bookmark-service.hpp"
#include "extend/metadata-model.hpp"
#include "service-registry.hpp"
#include "services/bookmark/bookmark.hpp"
#include "ui/horizontal-metadata.hpp"
#include "ui/omni-list.hpp"
#include "ui/omni-scroll-bar.hpp"
#include "ui/typography.hpp"
#include <memory>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <ranges>
#include <sys/socket.h>
#include <qscrollarea.h>

class DetailWithMetadataWidget : public QWidget {
  HorizontalMetadata *m_metadata = new HorizontalMetadata;
  QScrollArea *m_contentScrollArea = new QScrollArea(this);
  HDivider *hdivider = new HDivider;

public:
  void setMetadata(const std::vector<MetadataItem> &items) {
    m_metadata->clear();

    for (const auto &item : items) {
      m_metadata->addItem(item);
    }

    hdivider->show();
    m_metadata->show();
  }

  void clearMetadata() {
    m_metadata->clear();
    hdivider->hide();
    m_metadata->hide();
  }

  QWidget *content() const { return m_contentScrollArea->widget(); }

  void setContent(QWidget *widget) {
    m_contentScrollArea->setWidget(widget);
    m_contentScrollArea->setWidgetResizable(true);
    m_contentScrollArea->setAutoFillBackground(false);
    setAutoFillBackground(false);
  }

  void setupUI() {
    auto layout = new QVBoxLayout;

    m_contentScrollArea->setVerticalScrollBar(new OmniScrollBar);
    m_contentScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_contentScrollArea->setWidgetResizable(true);
    m_contentScrollArea->setAutoFillBackground(false);
    setAutoFillBackground(false);
    m_contentScrollArea->setAttribute(Qt::WA_TranslucentBackground);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_contentScrollArea, 1);
    m_contentScrollArea->setFocusPolicy(Qt::NoFocus);
    layout->addWidget(hdivider);
    layout->addWidget(m_metadata);
    setLayout(layout);

    hdivider->hide();
    m_metadata->hide();
  }

public:
  DetailWithMetadataWidget(QWidget *parent = nullptr) : QWidget(parent) { setupUI(); }
};

class BookmarkDetailWidget : public DetailWithMetadataWidget {
  TypographyWidget *m_expandedLink = new TypographyWidget(this);
  std::shared_ptr<Bookmark> m_bookmark;

  QString expandLink(const Bookmark &bookmark, const std::vector<QString> &arguments) {
    auto appDb = ServiceRegistry::instance()->appDb();
    QString expanded;
    size_t argumentIndex = 0;

    for (const auto &part : m_bookmark->parts()) {
      if (auto s = std::get_if<QString>(&part)) {
        expanded += *s;
      } else if (auto placeholder = std::get_if<Bookmark::ParsedPlaceholder>(&part)) {
        if (placeholder->id == "clipboard") {
          expanded += QApplication::clipboard()->text();
        } else if (placeholder->id == "selected") {
          // TODO: selected text
        } else if (placeholder->id == "uuid") {
          expanded += QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces);
        } else {
          if (argumentIndex < arguments.size()) { expanded += arguments.at(argumentIndex++); }
        }
      }
    }

    return expanded;
  }

public:
  void setBookmark(const std::shared_ptr<Bookmark> &bookmark) {
    m_bookmark = bookmark;

    auto name = MetadataLabel{
        .text = m_bookmark->name(),
        .title = "Name",
    };
    auto app = MetadataLabel{
        .text = m_bookmark->app(),
        .title = "Application",
    };
    auto opened = MetadataLabel{
        .text = QString::number(0),
        .title = "Opened",
    };

    setMetadata({name, app, opened});
    m_expandedLink->setText(expandLink(*m_bookmark.get(), {}));
  }

  void updateArguments(const std::vector<QString> &arguments) {
    m_expandedLink->setText(expandLink(*m_bookmark.get(), arguments));
  }

  BookmarkDetailWidget(QWidget *parent = nullptr) {
    m_expandedLink->setWordWrap(true);
    m_expandedLink->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setContent(m_expandedLink);
  }
};

class QuicklinkItem : public AbstractDefaultListItem, public ListView::Actionnable {
  std::shared_ptr<Bookmark> link;

public:
  const std::shared_ptr<Bookmark> &bookmark() const { return link; }

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

    return std::make_unique<CompleterData>(CompleterData{
        .iconUrl = link->icon(),
        .arguments = args,
    });
  }

  ItemData data() const override {
    return {
        .iconUrl = link->icon(),
        .name = link->name(),
    };
  }

  QWidget *generateDetail() const override {
    auto widget = new BookmarkDetailWidget;

    widget->setBookmark(link);

    return widget;
  }

  QString generateId() const override { return QString::number(link->id()); }

public:
  QuicklinkItem(const std::shared_ptr<Bookmark> &link) : link(link) {}
};

class ManageBookmarksView : public ListView {
  void renderList(const QString &s, OmniList::SelectionPolicy policy = OmniList::SelectFirst) {
    auto bookmarkService = ServiceRegistry::instance()->bookmarks();
    auto bookmarks =
        bookmarkService->bookmarks() |
        std::views::filter([s](auto bk) { return bk->name().contains(s, Qt::CaseInsensitive); }) |
        std::views::transform([](auto bk) -> std::unique_ptr<OmniList::AbstractVirtualItem> {
          return std::make_unique<QuicklinkItem>(bk);
        }) |
        std::ranges::to<std::vector>();

    m_list->updateModel(
        [&]() {
          auto &section = m_list->addSection("Bookmarks");

          section.addItems(std::move(bookmarks));
        },
        policy);
  }

  void itemSelected(const OmniList::AbstractVirtualItem *item) override {}

  void onBookmarkRemoved() { renderList(searchText(), OmniList::PreserveSelection); }

  void onBookmarkSaved() { renderList(searchText(), OmniList::PreserveSelection); }

  void textChanged(const QString &s) override { renderList(s); }

  void initialize() override { textChanged(""); }

  void argumentValuesChanged(const std::vector<std::pair<QString, QString>> &args) override {
    auto values = args | std::views::transform([](auto &&pair) { return pair.second; }) |
                  std::ranges::to<std::vector>();
    if (auto bookmarkDetail = dynamic_cast<BookmarkDetailWidget *>(detail())) {
      bookmarkDetail->updateArguments(values);
    }
  }

public:
  ManageBookmarksView() {
    auto bookmarkService = ServiceRegistry::instance()->bookmarks();

    setSearchPlaceholderText("Search bookmarks...");
    setNavigationTitle("Search Bookmarks");

    connect(bookmarkService, &BookmarkService::bookmarkSaved, this, &ManageBookmarksView::onBookmarkSaved);
    connect(bookmarkService, &BookmarkService::bookmarkRemoved, this,
            &ManageBookmarksView::onBookmarkRemoved);
  }
};
