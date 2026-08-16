#pragma once
#include "fuzzy-section.hpp"
#include "services/browser-extension-service.hpp"
#include <QCoreApplication>

using BrowserTab = BrowserExtensionService::BrowserTab;

template <> struct fuzzy::FuzzySearchable<BrowserTab> {
  static fuzzy::Match score(const BrowserTab &tab, std::string_view query) {
    return fuzzy::scoreWeighted({{tab.url, 1.0}, {tab.title, 0.6}}, query);
  }
};

class BrowserTabsSection : public FuzzySection<BrowserTab> {
  Q_DECLARE_TR_FUNCTIONS(BrowserTabsSection)
public:
  enum class Kind { PlayingMedia, Tabs };

  explicit BrowserTabsSection(Kind kind) : m_kind(kind) {}

  QString sectionName() const override {
    return m_kind == Kind::PlayingMedia ? tr("Playing Media ({count})") : tr("Tabs ({count})");
  }

protected:
  QString displayTitle(const BrowserTab &tab) const override;
  QString displaySubtitle(const BrowserTab &tab) const override;
  std::optional<ImageURL> displayIcon(const BrowserTab &tab) const override;
  AccessoryList displayAccessories(const BrowserTab &tab) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const BrowserTab &tab) const override;

private:
  Kind m_kind;
};
