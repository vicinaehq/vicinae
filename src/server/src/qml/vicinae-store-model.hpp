#pragma once
#include <string>
#include <vector>
#include "fuzzy-section.hpp"
#include "fuzzy/fuzzy-searchable.hpp"
#include "services/extension-store/vicinae-store.hpp"

class ExtensionRegistry;

struct VicinaeStoreEntry {
  VicinaeStore::Extension extension;
  bool installed = false;
};

template <> struct fuzzy::FuzzySearchable<VicinaeStoreEntry> {
  static int score(const VicinaeStoreEntry &entry, std::string_view query) {
    auto title = entry.extension.title.toStdString();
    auto author = entry.extension.author.name.toStdString();
    auto desc = entry.extension.description.toStdString();
    return fuzzy::scoreWeighted({{title, 1.0}, {author, 0.5}, {desc, 0.3}}, query);
  }
};

class VicinaeStoreSection : public FuzzySection<VicinaeStoreEntry> {
public:
  enum ExtraRole {
    DownloadCount = 100,
    AuthorAvatar,
    IsInstalled,
    CompatTierRole,
  };

  void setEntries(const std::vector<VicinaeStore::Extension> &extensions, ExtensionRegistry *registry,
                  const QString &sectionName);

  QString sectionName() const override { return m_sectionName; }

  const VicinaeStoreEntry &resolvedEntry(int i) const { return at(i); }

protected:
  QString displayTitle(const VicinaeStoreEntry &entry) const override;
  QString displaySubtitle(const VicinaeStoreEntry &entry) const override;
  QString displayIconSource(const VicinaeStoreEntry &entry) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const VicinaeStoreEntry &entry) const override;

private:
  QString m_sectionName;
};
