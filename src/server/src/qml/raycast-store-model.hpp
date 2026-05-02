#pragma once
#include "section-source.hpp"
#include "services/raycast/raycast-store.hpp"

class ExtensionRegistry;

class RaycastStoreSection : public SectionSource {
public:
  enum ExtraRole {
    DownloadCount = 100,
    AuthorAvatar,
    IsInstalled,
    CompatTierRole,
  };

  struct Entry {
    Raycast::Extension extension;
    bool installed = false;
    Raycast::CompatTier compatTier = Raycast::CompatTier::Unknown;
  };

  void setEntries(const std::vector<Raycast::Extension> &extensions, ExtensionRegistry *registry,
                  const Raycast::CompatMap &compat, const QString &sectionName);

  QString sectionName() const override { return m_sectionName; }
  int count() const override { return static_cast<int>(m_entries.size()); }

  const Entry &entryAt(int i) const { return m_entries[i]; }

protected:
  QString itemTitle(int i) const override;
  QString itemSubtitle(int i) const override;
  QString itemIconSource(int i) const override;
  std::unique_ptr<ActionPanelState> actionPanel(int i) const override;

private:
  std::vector<Entry> m_entries;
  QString m_sectionName;
};
