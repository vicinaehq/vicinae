#pragma once
#include <memory>
#include <optional>
#include <QCoreApplication>
#include <QString>
#include "fuzzy/fuzzy-searchable.hpp"
#include "services/dictation/dictation-styles.hpp"
#include "ui/views/mono-list-view-host.hpp"

class ApplicationContext;

namespace AI {
class Service;
}

template <> struct fuzzy::FuzzySearchable<DictationStyles::Style> {
  static fuzzy::Match score(const DictationStyles::Style &style, const fuzzy::Query &query) {
    return fuzzy::scoreWeighted({{style.name, 1.0}}, query);
  }
};

namespace Dictation {

/**
 * Open the list of styles or the creation form, or the AI setup screen when a model is needed first.
 */
void openStyles(const ApplicationContext *ctx);
void openStyleForm(const ApplicationContext *ctx);
std::optional<QString> styleModelName(AI::Service &ai, const DictationStyles::Style &style);

} // namespace Dictation

class DictationStylesViewHost : public MonoListViewHost<DictationStyles::Style> {
  Q_DECLARE_TR_FUNCTIONS(DictationStylesViewHost)

public:
  void onMount() override;

  QString sectionName() const override { return tr("Styles ({count})"); }
  QString displayId(const ItemType &style) const override { return QString::fromStdString(style.id); }
  QString displayTitle(const ItemType &style) const override { return QString::fromStdString(style.name); }
  QString displaySubtitle(const ItemType &) const override { return {}; }
  std::optional<ImageURL> displayIcon(const ItemType &) const override;
  AccessoryList displayAccessories(const ItemType &style) const override;
  bool hasDetailPane() const override { return true; }
  std::optional<ListItemDetail> displayDetail(const ItemType &style) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const ItemType &style) const override;

protected:
  std::unique_ptr<ActionPanelState> emptyActionPanel() override;

private:
  void reload();
};
