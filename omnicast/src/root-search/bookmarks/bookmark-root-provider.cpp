#include "root-search/bookmarks/bookmark-root-provider.hpp"
#include "action-panel/action-panel.hpp"
#include "actions/app/app-actions.hpp"
#include "actions/bookmark/bookmark-actions.hpp"
#include "actions/fallback-actions.hpp"
#include "argument.hpp"
#include "services/bookmark/bookmark-service.hpp"
#include "root-item-manager.hpp"
#include <memory>
#include <ranges>

ActionPanelView *RootBookmarkItem::actionPanel() const {
  auto panel = new ActionPanelStaticListView;

  panel->setTitle(m_link->name());
  panel->addAction(new OpenCompletedBookmarkAction(m_link));
  panel->addAction(new OpenWithAppAction({m_link->url()}));
  panel->addAction(new EditBookmarkAction(m_link));
  panel->addAction(new DuplicateBookmarkAction(m_link));
  panel->addAction(new RemoveBookmarkAction(m_link));

  return panel;
};

ActionPanelView *RootBookmarkItem::fallbackActionPanel() const {
  auto panel = new ActionPanelStaticListView;
  auto open = new OpenBookmarkFromSearchText(m_link);
  auto manage = new ManageFallbackActions;

  open->setPrimary(true);
  panel->setTitle(m_link->name());
  panel->addAction(open);
  panel->addAction(manage);

  return panel;
}

QString RootBookmarkItem::uniqueId() const { return QString("bookmarks.%1").arg(m_link->id()); }

QString RootBookmarkItem::displayName() const { return m_link->name(); }

double RootBookmarkItem::baseScoreWeight() const { return 1.4; }

QString RootBookmarkItem::providerId() const { return "bookmark"; }

AccessoryList RootBookmarkItem::accessories() const {
  return {{.text = "Bookmark", .color = ColorTint::TextSecondary}};
}

bool RootBookmarkItem::isSuitableForFallback() const { return m_link->arguments().size() == 1; }

ArgumentList RootBookmarkItem::arguments() const {
  auto mapArg = [](const Bookmark::Argument &arg) -> CommandArgument {
    CommandArgument cmdArg;

    cmdArg.type = CommandArgument::Text;
    cmdArg.required = arg.defaultValue.isEmpty();
    cmdArg.placeholder = arg.name;

    return cmdArg;
  };

  return m_link->arguments() | std::views::transform(mapArg) | std::ranges::to<std::vector>();
}

OmniIconUrl RootBookmarkItem::iconUrl() const {
  OmniIconUrl url(m_link->icon());

  if (url.type() == OmniIconType::Builtin) { url.setBackgroundTint(ColorTint::Red); }

  return url;
}

RootBookmarkItem::RootBookmarkItem(const std::shared_ptr<Bookmark> &link) : m_link(link) {}

std::vector<std::shared_ptr<RootItem>> BookmarkRootProvider::loadItems() const {
  auto mapBookmark = [](auto &&bookmark) -> std::shared_ptr<RootItem> {
    return std::make_shared<RootBookmarkItem>(bookmark);
  };

  return m_db.bookmarks() | std::views::transform(mapBookmark) | std::ranges::to<std::vector>();
};

QString BookmarkRootProvider::displayName() const { return "Bookmarks"; }
QString BookmarkRootProvider::uniqueId() const { return "bookmarks"; }
RootProvider::Type BookmarkRootProvider::type() const { return RootProvider::Type::GroupProvider; }

BookmarkRootProvider::BookmarkRootProvider(BookmarkService &db) : m_db(db) {
  connect(&db, &BookmarkService::bookmarkSaved, this, [this]() { emit itemsChanged(); });
  connect(&db, &BookmarkService::bookmarkRemoved, this, [this]() { emit itemsChanged(); });
}
