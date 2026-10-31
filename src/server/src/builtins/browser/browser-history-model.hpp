#pragma once
#include "ui/views/fuzzy-section.hpp"
#include "services/browser-extension-service.hpp"
#include <QCoreApplication>
#include <functional>

using BrowserHistoryItem = BrowserExtensionService::BrowserHistoryItem;

template <> struct fuzzy::FuzzySearchable<BrowserHistoryItem> {
  static fuzzy::Match score(const BrowserHistoryItem &item, const fuzzy::Query &query) {
    return fuzzy::scoreWeighted({{item.title, 1.0}, {item.url, 0.6}}, query);
  }
};

class BrowserHistorySection : public FuzzySection<BrowserHistoryItem> {
  Q_DECLARE_TR_FUNCTIONS(BrowserHistorySection)

public:
  using SelectionHandler = std::function<void(const BrowserHistoryItem *item)>;

  QString sectionName() const override {
    return m_truncated ? tr("History ({count}+)") : tr("History ({count})");
  }

  void setOnSelectionChanged(SelectionHandler handler) { m_onSelectionChanged = std::move(handler); }
  void setOnToggleDetail(std::function<void()> handler) { m_onToggleDetail = std::move(handler); }
  void setDetailVisible(bool visible) { m_detailVisible = visible; }
  void setTruncated(bool truncated) { m_truncated = truncated; }

  void onSelected(int i) override {
    if (m_onSelectionChanged) m_onSelectionChanged(&at(i));
  }

  void onSelectionCleared() override {
    if (m_onSelectionChanged) m_onSelectionChanged(nullptr);
  }

protected:
  QString displayId(const BrowserHistoryItem &item) const override;
  QString displayTitle(const BrowserHistoryItem &item) const override;
  QString displaySubtitle(const BrowserHistoryItem &item) const override;
  std::optional<ImageURL> displayIcon(const BrowserHistoryItem &item) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const BrowserHistoryItem &item) const override;

private:
  SelectionHandler m_onSelectionChanged;
  std::function<void()> m_onToggleDetail;
  bool m_detailVisible = false;
  bool m_truncated = false;
};
