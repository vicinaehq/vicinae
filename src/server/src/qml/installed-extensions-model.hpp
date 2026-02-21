#pragma once
#include "fuzzy-list-model.hpp"
#include "services/extension-registry/extension-manifest.hpp"

template <> struct fuzzy::FuzzySearchable<ExtensionManifest> {
  static int score(const ExtensionManifest &m, std::string_view query) {
    auto title = m.title.toStdString();
    auto desc = m.description.toStdString();
    auto author = m.author.toStdString();
    return fuzzy::scoreWeighted({{title, 1.0}, {desc, 0.5}, {author, 0.7}}, query);
  }
};

class InstalledExtensionsModel : public FuzzyListModel<ExtensionManifest> {
  Q_OBJECT

public:
  using FuzzyListModel::FuzzyListModel;

protected:
  QString displayTitle(const ExtensionManifest &m) const override;
  QString displaySubtitle(const ExtensionManifest &m) const override;
  QString displayIconSource(const ExtensionManifest &m) const override;
  QString displayAccessory(const ExtensionManifest &m) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const ExtensionManifest &m) const override;
  QString sectionLabel() const override;
};
