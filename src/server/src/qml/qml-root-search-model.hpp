#pragma once
#include "common/context.hpp"
#include "extensions/root/root-search-model.hpp"
#include "services/app-service/app-service.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include <QAbstractListModel>

class ActionPanelState;

class QmlRootSearchModel : public QAbstractListModel {
  Q_OBJECT
  Q_PROPERTY(QString primaryActionTitle READ primaryActionTitle NOTIFY primaryActionChanged)
  Q_PROPERTY(QString primaryActionIcon READ primaryActionIcon NOTIFY primaryActionChanged)

public:
  enum Role {
    IsSection = Qt::UserRole + 1,
    IsSelectable,
    SectionName,
    ItemType,
    Title,
    Subtitle,
    IconSource,
    Alias,
    IsActive,
  };

  explicit QmlRootSearchModel(ApplicationContext &ctx, QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  Q_INVOKABLE void setFilter(const QString &text);
  Q_INVOKABLE int nextSelectableIndex(int from, int direction) const;
  Q_INVOKABLE void setSelectedIndex(int index);
  Q_INVOKABLE void activateSelected();

  QString primaryActionTitle() const;
  QString primaryActionIcon() const;

signals:
  void primaryActionChanged();

private:
  // Safe copy of a search result that owns the shared_ptr
  // (ScoredItem uses reference_wrapper which can dangle)
  struct OwnedResult {
    std::shared_ptr<RootItem> item;
    RootItemMetadata meta;
  };

  struct FlatItem {
    enum Kind { SectionHeader, ResultItem, FallbackItem, FavoriteItem, LinkItem } kind;
    int dataIndex;
    SectionType section;
  };

  void rebuildFlatList();
  void addSection(SectionType section, const std::string &name, int count);
  QString imageSourceFor(const ImageURL &url) const;
  QString itemTypeString(FlatItem::Kind kind) const;

  ApplicationContext *m_ctx;
  RootItemManager *m_manager;
  AppService *m_appDb;

  std::string m_query;
  std::vector<FlatItem> m_flat;
  std::vector<OwnedResult> m_results;
  std::vector<std::shared_ptr<RootItem>> m_fallbackItems;
  std::vector<std::shared_ptr<RootItem>> m_favorites;
  std::optional<LinkItem> m_defaultOpener;

  std::unique_ptr<ActionPanelState> m_actionPanel;
  int m_selectedIndex = -1;
};
