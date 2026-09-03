#pragma once
#include <QAbstractListModel>
#include <QObject>
#include <QString>
#include <vector>
#include "bar/bar-host.hpp"

/**
 * Workspaces shown on one screen: those the window manager places on that output, plus
 * output-agnostic workspaces (X11 desktops, GNOME) shown everywhere.
 */
class BarWorkspacesModel : public QAbstractListModel {
  Q_OBJECT

public:
  enum Role { Id = Qt::UserRole + 1, Name, Active, WindowCount, Icons, HasWindows };

  BarWorkspacesModel(BarHost &host, QString screenName, QObject *parent = nullptr);

  Q_INVOKABLE void focus(int row);

  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

private:
  void reload();

  BarHost &m_host;
  QString m_screenName;
  std::vector<const BarWorkspace *> m_rows;
};

class BarScreen : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString name READ name CONSTANT)
  Q_PROPERTY(BarWorkspacesModel *workspaces READ workspaces CONSTANT)

public:
  BarScreen(BarHost &host, QString screenName, QObject *parent = nullptr);

  QString name() const { return m_name; }
  BarWorkspacesModel *workspaces() { return &m_workspaces; }

private:
  QString m_name;
  BarWorkspacesModel m_workspaces;
};
