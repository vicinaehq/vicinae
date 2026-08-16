#pragma once
#include "builtin_icon.hpp"
#include "fuzzy-section.hpp"
#include <QCoreApplication>

struct IconEntry {
  BuiltinIcon icon;
  std::string name;
};

template <> struct fuzzy::FuzzySearchable<IconEntry> {
  static fuzzy::Match score(const IconEntry &e, std::string_view query) {
    return fuzzy::scoreWeighted({{e.name, 1.0}}, query);
  }
};

class BuiltinIconsSection : public FuzzySection<IconEntry> {
  Q_DECLARE_TR_FUNCTIONS(BuiltinIconsSection)
public:
  QString sectionName() const override { return tr("Icons ({count})"); }

protected:
  QString displayTitle(const IconEntry &e) const override;
  QString displaySubtitle(const IconEntry &) const override { return {}; }
  std::optional<ImageURL> displayIcon(const IconEntry &e) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const IconEntry &e) const override;
};
