#pragma once
#include "fuzzy-list-model.hpp"
#include "services/ai/ai-provider.hpp"

template <> struct fuzzy::FuzzySearchable<AI::ProviderModel> {
  static int score(const AI::ProviderModel &m, std::string_view query) {
    return fuzzy::scoreWeighted({{m.name, 1.0}, {m.id, 0.6}, {m.ref.provider, 0.3}}, query);
  }
};

class ManageModelsModel : public FuzzyListModel<AI::ProviderModel> {
  Q_OBJECT

signals:
  void modelSelected(const AI::ProviderModel &model);

public:
  using FuzzyListModel::FuzzyListModel;

protected:
  QString displayTitle(const AI::ProviderModel &item) const override;
  QString displaySubtitle(const AI::ProviderModel &item) const override;
  QString displayIconSource(const AI::ProviderModel &item) const override;
  QVariantList displayAccessory(const AI::ProviderModel &item) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const AI::ProviderModel &item) const override;
  void itemSelected(const AI::ProviderModel &item) override;
  QString sectionLabel() const override;
};
