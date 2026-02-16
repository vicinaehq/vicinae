#pragma once
#include "qml-command-list-model.hpp"
#include "services/raycast/raycast-store.hpp"

class ExtensionRegistry;

class QmlRaycastStoreModel : public QmlCommandListModel {
  Q_OBJECT

signals:

public:
  enum ExtraRole {
    DownloadCount = QmlCommandListModel::Accessory + 1,
    AuthorAvatar,
    IsInstalled,
  };

  explicit QmlRaycastStoreModel(QObject *parent = nullptr);

  void setEntries(const std::vector<Raycast::Extension> &extensions, ExtensionRegistry *registry,
                  const QString &sectionName);
  void setFilter(const QString &) override {}

  QHash<int, QByteArray> roleNames() const override;
  QVariant data(const QModelIndex &index, int role) const override;

protected:
  QString itemTitle(int s, int i) const override;
  QString itemSubtitle(int s, int i) const override;
  QString itemIconSource(int s, int i) const override;
  std::unique_ptr<ActionPanelState> createActionPanel(int s, int i) const override;

private:
  struct Entry {
    Raycast::Extension extension;
    bool installed = false;
  };
  std::vector<Entry> m_entries;
};
