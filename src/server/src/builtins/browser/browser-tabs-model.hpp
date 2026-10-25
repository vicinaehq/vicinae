#pragma once
#include "ui/views/fuzzy-section.hpp"
#include "services/browser-extension-service.hpp"
#include <QCoreApplication>

using BrowserTab = BrowserExtensionService::BrowserTab;

template <> struct fuzzy::FuzzySearchable<BrowserTab> {
  static fuzzy::Match score(const BrowserTab &tab, const fuzzy::Query &query) {
    return fuzzy::scoreWeighted({{tab.url, 1.0}, {tab.title, 0.6}}, query);
  }
};

class BrowserTabsSection : public FuzzySection<BrowserTab> {
  Q_DECLARE_TR_FUNCTIONS(BrowserTabsSection)
public:
  enum class Kind { Pinned, PlayingMedia, Tabs };

  explicit BrowserTabsSection(Kind kind) : m_kind(kind) {}

  QString sectionName() const override {
    if (m_kind == Kind::Pinned) return tr("Pinned Tabs ({count})");
    if (m_kind == Kind::PlayingMedia) return tr("Playing Media ({count})");
    return tr("Tabs ({count})");
  }

protected:
  bool isActive(int i) const override { return at(i).active; }
  QString displayTitle(const BrowserTab &tab) const override;
  QString displaySubtitle(const BrowserTab &tab) const override;
  std::optional<ImageURL> displayIcon(const BrowserTab &tab) const override;
  AccessoryList displayAccessories(const BrowserTab &tab) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const BrowserTab &tab) const override;

private:
  Kind m_kind;
};
