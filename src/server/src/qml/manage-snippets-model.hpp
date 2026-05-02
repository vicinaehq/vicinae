#pragma once
#include "fuzzy-section.hpp"
#include "services/snippet/snippet-db.hpp"

template <> struct fuzzy::FuzzySearchable<snippet::SerializedSnippet> {
  static int score(const snippet::SerializedSnippet &item, std::string_view query) {
    auto name = item.name;
    std::string keyword;
    if (item.expansion) keyword = item.expansion->keyword;
    return fuzzy::scoreWeighted({{name, 1.0}, {keyword, 0.3}}, query);
  }
};

class ManageSnippetsSection : public FuzzySection<snippet::SerializedSnippet> {
public:
  QString sectionName() const override { return QStringLiteral("Snippets ({count})"); }

  void setOnSnippetSelected(std::function<void(const snippet::SerializedSnippet &)> cb) {
    m_onSnippetSelected = std::move(cb);
  }

  void onSelected(int i) override {
    if (m_onSnippetSelected) m_onSnippetSelected(at(i));
  }

protected:
  QString displayTitle(const snippet::SerializedSnippet &item) const override;
  QString displayIconSource(const snippet::SerializedSnippet &item) const override;
  QVariantList displayAccessories(const snippet::SerializedSnippet &item) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const snippet::SerializedSnippet &item) const override;

private:
  std::function<void(const snippet::SerializedSnippet &)> m_onSnippetSelected;
};
