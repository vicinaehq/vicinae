#pragma once
#include <string>
#include <vector>
#include "command-list-model.hpp"
#include "fuzzy/fuzzy-searchable.hpp"
#include "services/extension-store/vicinae-store.hpp"

class ExtensionRegistry;

class VicinaeStoreModel : public CommandListModel {
  Q_OBJECT

signals:

public:
  struct Entry {
    VicinaeStore::Extension extension;
    bool installed = false;
  };

  enum ExtraRole {
    DownloadCount = CommandListModel::Accessory + 1,
    AuthorAvatar,
    IsInstalled,
    CompatTierRole,
  };

  explicit VicinaeStoreModel(QObject *parent = nullptr);

  void setEntries(const std::vector<VicinaeStore::Extension> &extensions, ExtensionRegistry *registry,
                  const QString &sectionName);
  void setFilter(const QString &text) override;

  QHash<int, QByteArray> roleNames() const override;
  QVariant data(const QModelIndex &index, int role) const override;

protected:
  QString itemTitle(int s, int i) const override;
  QString itemSubtitle(int s, int i) const override;
  QString itemIconSource(int s, int i) const override;
  std::unique_ptr<ActionPanelState> createActionPanel(int s, int i) const override;

private:
  void applyFilter();
  const Entry &resolvedEntry(int i) const;

  std::vector<Entry> m_entries;
  std::vector<Scored<int>> m_filtered;
  std::string m_query;
  QString m_sectionName;
};
