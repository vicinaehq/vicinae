#pragma once
#include "fuzzy-list-model.hpp"
#include "services/snippet/snippet-db.hpp"

template <> struct fuzzy::FuzzySearchable<snippet::SerializedSnippet> {
  static int score(const snippet::SerializedSnippet &item, std::string_view query) {
    auto name = item.name;
    std::string keyword;
    if (item.expansion) keyword = item.expansion->keyword;
    return fuzzy::scoreWeighted({{name, 1.0}, {keyword, 0.8}}, query);
  }
};

class ManageSnippetsModel : public FuzzyListModel<snippet::SerializedSnippet> {
  Q_OBJECT

signals:
  void snippetSelected(const snippet::SerializedSnippet &snippet);

public:
  using FuzzyListModel::FuzzyListModel;

protected:
  QString displayTitle(const snippet::SerializedSnippet &item) const override;
  QString displayIconSource(const snippet::SerializedSnippet &item) const override;
  QString displayAccessory(const snippet::SerializedSnippet &item) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const snippet::SerializedSnippet &item) const override;
  void itemSelected(const snippet::SerializedSnippet &item) override;
  QString sectionLabel() const override;
};
