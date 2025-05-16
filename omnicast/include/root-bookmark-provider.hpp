#pragma once
#include "bookmark-actions.hpp"
#include "bookmark-service.hpp"
#include "root-item-manager.hpp"

class RootBookmarkProvider : public RootProvider {
  BookmarkService &m_db;

  class RootBookmarkItem : public RootItem {
    std::shared_ptr<Bookmark> m_link;

    QString displayName() const override { return m_link->name(); }

    double baseScoreWeight() const override { return 1.4; }

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

    QList<AbstractAction *> actions() const override {
      QList<AbstractAction *> list;

      list << new OpenBookmarkAction(m_link);

      // list << new OpenCompletedBookmarkAction(m_link);
      // list << new EditBookmarkAction(m_link);
      // list << new DuplicateBookmarkAction(m_link);
      // list << new RemoveBookmarkAction(m_link);

      return list;
    }

  public:
    RootBookmarkItem(const std::shared_ptr<Bookmark> &link) : m_link(link) {}
  };

public:
  QString displayName() const override { return "Bookmarks"; }
  QString uniqueId() const override { return "links"; }
  Type type() const override { return RootProvider::Type::GroupProvider; }

  std::vector<std::shared_ptr<RootItem>> loadItems() const override {
    return m_db.bookmarks() | std::views::transform([](const auto &a) {
             return std::static_pointer_cast<RootItem>(std::make_shared<RootBookmarkItem>(a));
           }) |
           std::ranges::to<std::vector>();
  };

  RootBookmarkProvider(BookmarkService &db) : m_db(db) {
    connect(&db, &BookmarkService::bookmarkSaved, this, [this]() { emit itemsChanged(); });
  }
};
