#include "bar/bar-tray-model.hpp"
#include <algorithm>
#include <ranges>
#include "image-url.hpp"
#include "services/tray-host/abstract-tray-host.hpp"

static QString trayTitle(const TrayItem &item) { return item.title.isEmpty() ? item.id : item.title; }

BarTrayModel::BarTrayModel(AbstractTrayHost *host, QObject *parent)
    : QAbstractListModel(parent), m_host(host) {
  connect(m_host, &AbstractTrayHost::itemsChanged, this, &BarTrayModel::reload);
  connect(m_host, &AbstractTrayHost::menuChanged, this, [this](const QString &key) {
    if (m_menuItem && m_menuItem->key() == key) loadMenu();
  });
  reload();
}

void BarTrayModel::reload() {
  auto items = m_host->items();
  std::erase_if(items, [](const TrayItem &item) { return item.status == TrayItem::Status::Passive; });
  std::ranges::sort(items, [](const TrayItem &a, const TrayItem &b) {
    return trayTitle(a).compare(trayTitle(b), Qt::CaseInsensitive) < 0;
  });

  beginResetModel();
  m_items = std::move(items);
  endResetModel();

  if (m_menuItem && !find(m_menuItem->key())) closeMenu();
}

const TrayItem *BarTrayModel::find(const QString &key) const {
  auto it = std::ranges::find_if(m_items, [&](const TrayItem &item) { return item.key() == key; });
  return it == m_items.end() ? nullptr : &*it;
}

void BarTrayModel::activate(const QString &key) {
  if (const auto *item = find(key)) m_host->activate(*item);
}

void BarTrayModel::secondaryActivate(const QString &key) {
  if (const auto *item = find(key)) m_host->secondaryActivate(*item);
}

void BarTrayModel::openMenu(const QString &key) {
  const auto *item = find(key);
  if (!item || !item->hasMenu()) return;
  m_menuItem = *item;
  m_menu.clear();
  emit menuItemChanged();
  loadMenu();
}

void BarTrayModel::closeMenu() {
  if (!m_menuItem) return;
  m_menuItem.reset();
  m_menuLoading = false;
  m_menu.clear();
  emit menuItemChanged();
}

void BarTrayModel::loadMenu() {
  if (!m_menuItem) return;
  m_menuLoading = true;
  emit menuItemChanged();
  const auto key = m_menuItem->key();
  m_host->menu(*m_menuItem).then(this, [this, key](std::vector<TrayMenuItem> entries) {
    if (!m_menuItem || m_menuItem->key() != key) return;
    m_menuLoading = false;
    m_menu.setEntries(std::move(entries));
    emit menuItemChanged();
  });
}

void BarTrayModel::triggerMenuItem(int menuId) {
  if (!m_menuItem) return;
  m_host->triggerMenuItem(*m_menuItem, menuId);
  emit menuActivated();
}

int BarTrayModel::rowCount(const QModelIndex &parent) const {
  return parent.isValid() ? 0 : static_cast<int>(m_items.size());
}

QVariant BarTrayModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || std::cmp_greater_equal(index.row(), m_items.size())) return {};
  const auto &item = m_items[index.row()];

  switch (role) {
  case Key:
    return item.key();
  case Title:
    return trayTitle(item);
  case Tooltip: {
    if (!item.tooltipTitle.isEmpty() && item.tooltipTitle != trayTitle(item)) return item.tooltipTitle;
    if (!item.tooltipDescription.isEmpty()) return item.tooltipDescription;
    return trayTitle(item);
  }
  case Icon:
    return QVariant::fromValue(ImageUrl(item.icon()));
  case Attention:
    return item.status == TrayItem::Status::NeedsAttention;
  case HasMenu:
    return item.hasMenu();
  case IsMenu:
    return item.itemIsMenu;
  default:
    return {};
  }
}

QHash<int, QByteArray> BarTrayModel::roleNames() const {
  return {{Key, "key"},      {Title, "title"},         {Tooltip, "hint"},
          {Icon, "icon"},    {Attention, "attention"}, {HasMenu, "hasMenu"},
          {IsMenu, "isMenu"}};
}
