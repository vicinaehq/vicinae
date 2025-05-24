#pragma once
#include "actions/bookmark/bookmark-actions.hpp"
#include "actions/fallback-actions.hpp"
#include "bookmark-service.hpp"
#include "root-item-manager.hpp"

class RootBookmarkProvider : public RootProvider {
  BookmarkService &m_db;

public:
  class RootBookmarkItem : public RootItem {
    std::shared_ptr<Bookmark> m_link;

    QString displayName() const override { return m_link->name(); }

    double baseScoreWeight() const override { return 1.4; }

    QString providerId() const override { return "bookmark"; }

    AccessoryList accessories() const override {
      return {{.text = "Bookmark", .color = ColorTint::TextSecondary}};
    }

    bool isSuitableForFallback() const override {
      // TODO: handle default arguments properly
      return m_link->arguments().size() == 1;
    }

    ArgumentList arguments() const override {
      ArgumentList args;

      for (const auto &barg : m_link->arguments()) {
        CommandArgument cmdArg;

        cmdArg.type = CommandArgument::Text;
        cmdArg.required = barg.defaultValue.isEmpty();
        cmdArg.placeholder = barg.name;
        args.emplace_back(cmdArg);
      }

      return args;
    }

    OmniIconUrl iconUrl() const override {
      OmniIconUrl url(m_link->icon());

      if (url.type() == OmniIconType::Builtin) { url.setBackgroundTint(ColorTint::Red); }

      return url;
    }

    QString uniqueId() const override { return QString("bookmarks.%1").arg(m_link->id()); }

    QList<AbstractAction *> fallbackActions() const override {
      auto open = new OpenBookmarkFromSearchText(m_link);

      return {open};
    }

    QList<AbstractAction *> actions() const override {
      QList<AbstractAction *> list;

      list << new OpenCompletedBookmarkAction(m_link);
      list << new EditBookmarkAction(m_link);
      list << new DuplicateBookmarkAction(m_link);
      list << new RemoveBookmarkAction(m_link);

      return list;
    }

  public:
    const Bookmark &bookmark() const { return *m_link.get(); }

    RootBookmarkItem(const std::shared_ptr<Bookmark> &link) : m_link(link) {}
  };

public:
  QString displayName() const override { return "Bookmarks"; }
  QString uniqueId() const override { return "bookmarks"; }
  Type type() const override { return RootProvider::Type::GroupProvider; }

  std::vector<std::shared_ptr<RootItem>> loadItems() const override {
    return m_db.bookmarks() | std::views::transform([](const auto &a) {
             return std::static_pointer_cast<RootItem>(std::make_shared<RootBookmarkItem>(a));
           }) |
           std::ranges::to<std::vector>();
  };

  RootBookmarkProvider(BookmarkService &db) : m_db(db) {
    connect(&db, &BookmarkService::bookmarkSaved, this, [this]() { emit itemsChanged(); });
    connect(&db, &BookmarkService::bookmarkRemoved, this, [this]() { emit itemsChanged(); });
  }
};
