#pragma once
#include "fuzzy-list-model.hpp"
#include "services/shortcut/shortcut.hpp"
#include <memory>

template <> struct fuzzy::FuzzySearchable<std::shared_ptr<Shortcut>> {
  static int score(const std::shared_ptr<Shortcut> &s, std::string_view query) {
    auto name = s->name().toStdString();
    return fuzzy::scoreWeighted({{name, 1.0}}, query);
  }
};

class ManageShortcutsModel : public FuzzyListModel<std::shared_ptr<Shortcut>> {
  Q_OBJECT

signals:
  void shortcutSelected(const std::shared_ptr<Shortcut> &shortcut);

public:
  using FuzzyListModel::FuzzyListModel;

protected:
  QString displayTitle(const std::shared_ptr<Shortcut> &item) const override;
  QString displayIconSource(const std::shared_ptr<Shortcut> &item) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const std::shared_ptr<Shortcut> &item) const override;
  void itemSelected(const std::shared_ptr<Shortcut> &item) override;
  QString sectionLabel() const override;
};
