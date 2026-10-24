#pragma once
#include "ui/views/fuzzy-section.hpp"
#include "services/browser-extension-service.hpp"
#include <QCoreApplication>
#include <functional>

using BrowserBookmark = BrowserExtensionService::BrowserBookmark;

template <> struct fuzzy::FuzzySearchable<BrowserBookmark> {
  static fuzzy::Match score(const BrowserBookmark &bookmark, const fuzzy::Query &query) {
    return fuzzy::scoreWeighted({{bookmark.title, 1.0}, {bookmark.url, 0.6}}, query);
  }
};

class BrowserBookmarksSection : public FuzzySection<BrowserBookmark> {
  Q_DECLARE_TR_FUNCTIONS(BrowserBookmarksSection)

public:
  using SelectionHandler = std::function<void(const BrowserBookmark *bookmark)>;

  QString sectionName() const override { return tr("Bookmarks ({count})"); }

  void setOnSelectionChanged(SelectionHandler handler) { m_onSelectionChanged = std::move(handler); }
  void setOnToggleDetail(std::function<void()> handler) { m_onToggleDetail = std::move(handler); }
  void setDetailVisible(bool visible) { m_detailVisible = visible; }

  void onSelected(int i) override {
    if (m_onSelectionChanged) m_onSelectionChanged(&at(i));
  }

  void onSelectionCleared() override {
    if (m_onSelectionChanged) m_onSelectionChanged(nullptr);
  }

protected:
  QString displayId(const BrowserBookmark &bookmark) const override;
  QString displayTitle(const BrowserBookmark &bookmark) const override;
  QString displaySubtitle(const BrowserBookmark &bookmark) const override;
  std::optional<ImageURL> displayIcon(const BrowserBookmark &bookmark) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const BrowserBookmark &bookmark) const override;

private:
  SelectionHandler m_onSelectionChanged;
  std::function<void()> m_onToggleDetail;
  bool m_detailVisible = false;
};
