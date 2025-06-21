#include "root-search/bookmarks/bookmark-root-provider.hpp"
#include "action-panel/action-panel.hpp"
#include "actions/bookmark/bookmark-actions.hpp"
#include "actions/fallback-actions.hpp"
#include "actions/root-search/root-search-actions.hpp"
#include "argument.hpp"
#include "omni-icon.hpp"
#include "services/bookmark/bookmark-service.hpp"
#include "root-item-manager.hpp"
#include <memory>
#include <ranges>

ActionPanelView *RootBookmarkItem::actionPanel() const {
  auto panel = new ActionPanelStaticListView;
  auto open = new OpenCompletedBookmarkAction(m_link);
  auto openWith = new OpenCompletedBookmarkWithAction(m_link);
  auto edit = new EditBookmarkAction(m_link);
  auto duplicate = new DuplicateBookmarkAction(m_link);
  auto remove = new RemoveBookmarkAction(m_link);
  auto markAsFavorite = new MarkItemAsFavorite(uniqueId());
  auto disable = new DisableItemAction(uniqueId());

  open->setPrimary(true);
  open->setShortcut({.key = "return"});
  openWith->setShortcut({.key = "return", .modifiers = {"shift"}});
  duplicate->setShortcut({.key = "N", .modifiers = {"ctrl"}});
  edit->setShortcut({.key = "E", .modifiers = {"ctrl"}});
  remove->setShortcut({.key = "X", .modifiers = {"ctrl"}});
  disable->setShortcut({.key = "X", .modifiers = {"ctrl", "shift"}});

  panel->setTitle(m_link->name());
  panel->addAction(open);
  panel->addAction(openWith);
  panel->addAction(edit);
  panel->addAction(duplicate);
  panel->addSection();
  panel->addAction(remove);
  panel->addAction(disable);

  return panel;
};

ActionPanelView *RootBookmarkItem::fallbackActionPanel() const {
  auto panel = new ActionPanelStaticListView;
  auto open = new OpenBookmarkFromSearchText(m_link);
  auto manage = new ManageFallbackActions;

  open->setShortcut({.key = "return"});
  manage->setShortcut({.key = "return", .modifiers = {"shift"}});
  open->setPrimary(true);
  panel->setTitle(m_link->name());
  panel->addAction(open);
  panel->addAction(manage);

  return panel;
}

QString RootBookmarkItem::typeDisplayName() const { return "Bookmark"; }

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

OmniIconUrl BookmarkRootProvider::icon() const {
  auto icon = BuiltinOmniIconUrl("bookmark");

  icon.setBackgroundTint(ColorTint::Red);

  return icon;
}

QString BookmarkRootProvider::uniqueId() const { return "bookmarks"; }
RootProvider::Type BookmarkRootProvider::type() const { return RootProvider::Type::GroupProvider; }

BookmarkRootProvider::BookmarkRootProvider(BookmarkService &db) : m_db(db) {
  connect(&db, &BookmarkService::bookmarkSaved, this, [this]() { emit itemsChanged(); });
  connect(&db, &BookmarkService::bookmarkRemoved, this, [this]() { emit itemsChanged(); });
}
