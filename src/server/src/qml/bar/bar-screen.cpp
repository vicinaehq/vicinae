#include "bar/bar-screen.hpp"
#include <QVariantList>
#include <ranges>

BarWorkspacesModel::BarWorkspacesModel(BarHost &host, QString screenName, QObject *parent)
    : QAbstractListModel(parent), m_host(host), m_screenName(std::move(screenName)) {
  connect(&m_host, &BarHost::workspacesChanged, this, &BarWorkspacesModel::reload);
  reload();
}

void BarWorkspacesModel::reload() {
  beginResetModel();
  m_rows.clear();
  const auto &all = m_host.workspaces();
  m_rows.reserve(all.size());
  for (const auto &entry : all) {
    const auto monitor = entry.workspace->monitor();
    if (monitor && *monitor != m_screenName) continue;
    m_rows.emplace_back(&entry);
  }
  endResetModel();
}

void BarWorkspacesModel::focus(int row) {
  if (row < 0 || std::cmp_greater_equal(row, m_rows.size())) return;
  m_host.focusWorkspace(m_rows[row]->workspace->id());
}

int BarWorkspacesModel::rowCount(const QModelIndex &parent) const {
  return parent.isValid() ? 0 : static_cast<int>(m_rows.size());
}

QVariant BarWorkspacesModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || std::cmp_greater_equal(index.row(), m_rows.size())) return {};
  const auto &entry = *m_rows[index.row()];

  switch (role) {
  case Id:
    return entry.workspace->id();
  case Name:
    return entry.workspace->name();
  case Active:
    return entry.active;
  case WindowCount:
    return static_cast<int>(entry.windowCount);
  case HasWindows:
    return entry.windowCount > 0;
  case Icons: {
    QVariantList icons;
    icons.reserve(static_cast<qsizetype>(entry.icons.size()));
    for (const auto &icon : entry.icons) {
      icons.push_back(QVariant::fromValue(icon));
    }
    return icons;
  }
  default:
    return {};
  }
}

QHash<int, QByteArray> BarWorkspacesModel::roleNames() const {
  return {{Id, "workspaceId"},          {Name, "name"},   {Active, "active"},
          {WindowCount, "windowCount"}, {Icons, "icons"}, {HasWindows, "hasWindows"}};
}

BarScreen::BarScreen(BarHost &host, QString screenName, QObject *parent)
    : QObject(parent), m_name(screenName), m_workspaces(host, std::move(screenName), this) {}
