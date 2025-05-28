#pragma once
#include "services/bookmark/bookmark-service.hpp"
#include "root-item-manager.hpp"

class RootBookmarkItem : public RootItem {
  std::shared_ptr<Bookmark> m_link;

  QString displayName() const override;
  double baseScoreWeight() const override;
  QString providerId() const override;
  AccessoryList accessories() const override;
  bool isSuitableForFallback() const override;
  ArgumentList arguments() const override;
  OmniIconUrl iconUrl() const override;
  QString uniqueId() const override;
  ActionPanelView *actionPanel() const override;
  ActionPanelView *fallbackActionPanel() const override;

public:
  const Bookmark &bookmark() const { return *m_link.get(); }

  RootBookmarkItem(const std::shared_ptr<Bookmark> &link);
};

class BookmarkRootProvider : public RootProvider {
  BookmarkService &m_db;

public:
  QString displayName() const override;
  QString uniqueId() const override;
  Type type() const override;
  std::vector<std::shared_ptr<RootItem>> loadItems() const override;

  BookmarkRootProvider(BookmarkService &db);
};
