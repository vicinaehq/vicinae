#pragma once
#include <QAbstractListModel>
#include <optional>
#include <vector>
#include "bar/bar-menu-model.hpp"
#include "services/tray-host/tray-item.hpp"

class AbstractTrayHost;

class BarTrayModel : public QAbstractListModel {
  Q_OBJECT
  Q_PROPERTY(BarMenuModel *menu READ menu CONSTANT)
  Q_PROPERTY(QString menuItemKey READ menuItemKey NOTIFY menuItemChanged)
  Q_PROPERTY(bool menuLoading READ menuLoading NOTIFY menuItemChanged)

signals:
  void menuItemChanged();
  void menuActivated();

public:
  enum Role { Key = Qt::UserRole + 1, Title, Tooltip, Icon, Attention, HasMenu, IsMenu };

  explicit BarTrayModel(AbstractTrayHost *host, QObject *parent = nullptr);

  BarMenuModel *menu() { return &m_menu; }
  QString menuItemKey() const { return m_menuItem ? m_menuItem->key() : QString(); }
  bool menuLoading() const { return m_menuLoading; }

  Q_INVOKABLE void activate(const QString &key);
  Q_INVOKABLE void secondaryActivate(const QString &key);
  Q_INVOKABLE void openMenu(const QString &key);
  Q_INVOKABLE void closeMenu();
  Q_INVOKABLE void triggerMenuItem(int menuId);

  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

private:
  void reload();
  void loadMenu();
  const TrayItem *find(const QString &key) const;

  AbstractTrayHost *m_host;
  std::vector<TrayItem> m_items;
  std::optional<TrayItem> m_menuItem;
  bool m_menuLoading = false;
  BarMenuModel m_menu{this};
};
