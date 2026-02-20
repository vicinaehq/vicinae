#pragma once
#include "builtin_icon.hpp"
#include "qml-fuzzy-list-model.hpp"

struct IconEntry {
  BuiltinIcon icon;
  std::string name;
};

template <> struct fuzzy::FuzzySearchable<IconEntry> {
  static int score(const IconEntry &e, std::string_view query) {
    return fuzzy::scoreWeighted({{e.name, 1.0}}, query);
  }
};

class QmlBuiltinIconsModel : public QmlFuzzyListModel<IconEntry> {
  Q_OBJECT

public:
  using QmlFuzzyListModel::QmlFuzzyListModel;

protected:
  QString displayTitle(const IconEntry &e) const override;
  QString displaySubtitle(const IconEntry &) const override { return {}; }
  QString displayIconSource(const IconEntry &e) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const IconEntry &e) const override;
  QString sectionLabel() const override;
};
