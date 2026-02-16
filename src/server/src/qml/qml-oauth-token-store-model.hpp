#pragma once
#include "qml-fuzzy-list-model.hpp"
#include "services/oauth/oauth-token-store.hpp"

template <> struct fuzzy::FuzzySearchable<OAuth::TokenSet> {
  static int score(const OAuth::TokenSet &set, std::string_view query) {
    auto extId = set.extensionId.toStdString();
    auto providerId = set.providerId.value_or("").toStdString();
    return fuzzy::scoreWeighted({{extId, 1.0}, {providerId, 0.8}}, query);
  }
};

class QmlOAuthTokenStoreModel : public QmlFuzzyListModel<OAuth::TokenSet> {
  Q_OBJECT

public:
  using QmlFuzzyListModel::QmlFuzzyListModel;

protected:
  QString displayTitle(const OAuth::TokenSet &set) const override;
  QString displaySubtitle(const OAuth::TokenSet &set) const override;
  QString displayIconSource(const OAuth::TokenSet &set) const override;
  QString displayAccessory(const OAuth::TokenSet &set) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const OAuth::TokenSet &set) const override;
  QString sectionLabel() const override;
};
