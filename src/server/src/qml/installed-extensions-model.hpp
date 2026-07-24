#pragma once
#include "fuzzy-section.hpp"
#include "services/extension-registry/extension-manifest.hpp"
#include <QCoreApplication>

template <> struct fuzzy::FuzzySearchable<ExtensionManifest> {
  static int score(const ExtensionManifest &m, std::string_view query) {
    auto title = m.title.toStdString();
    auto desc = m.description.toStdString();
    auto author = m.author.toStdString();
    return fuzzy::scoreWeighted({{title, 1.0}, {desc, 0.5}, {author, 0.2}}, query);
  }
};

class InstalledExtensionsSection : public FuzzySection<ExtensionManifest> {
  Q_DECLARE_TR_FUNCTIONS(InstalledExtensionsSection)
public:
  QString sectionName() const override { return tr("Installed Extensions ({count})"); }

protected:
  QString displayTitle(const ExtensionManifest &m) const override;
  QString displaySubtitle(const ExtensionManifest &m) const override;
  QString displayIconSource(const ExtensionManifest &m) const override;
  QVariantList displayAccessories(const ExtensionManifest &m) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const ExtensionManifest &m) const override;
};
