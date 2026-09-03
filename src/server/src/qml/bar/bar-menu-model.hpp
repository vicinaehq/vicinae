#pragma once
#include <QAbstractListModel>
#include <vector>
#include "image-url.hpp"
#include "services/tray-host/tray-item.hpp"

/**
 * Hierarchical dbusmenu tree exposed one level at a time: `enter` drills into a submenu,
 * `back` returns to its parent.
 */
class BarMenuModel : public QAbstractListModel {
  Q_OBJECT
  Q_PROPERTY(bool canGoBack READ canGoBack NOTIFY levelChanged)
  Q_PROPERTY(QString title READ title NOTIFY levelChanged)

signals:
  void levelChanged();

public:
  enum Role { Id = Qt::UserRole + 1, Label, Enabled, Separator, Submenu, ToggleType, Checked, Icon, HasIcon };

  using QAbstractListModel::QAbstractListModel;

  void setEntries(std::vector<TrayMenuItem> entries);
  void clear();

  bool canGoBack() const { return !m_path.empty(); }
  QString title() const;

  Q_INVOKABLE void enter(int row);
  Q_INVOKABLE void back();

  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

private:
  const std::vector<TrayMenuItem> &currentLevel() const;
  void rebuildLevel();

  std::vector<TrayMenuItem> m_root;
  std::vector<int> m_path;
  std::vector<const TrayMenuItem *> m_level;
};
