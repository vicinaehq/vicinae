#include "keybind-settings-model.hpp"
#include "keyboard/keybind-manager.hpp"
#include "keyboard/keyboard.hpp"
#include "lib/fuzzy/fuzzy-searchable.hpp"
#include <algorithm>

KeybindSettingsModel::KeybindSettingsModel(QObject *parent) : QAbstractListModel(parent) { rebuild({}); }

int KeybindSettingsModel::rowCount(const QModelIndex &) const { return static_cast<int>(m_entries.size()); }

QVariant KeybindSettingsModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(m_entries.size())) return {};
  const auto &e = m_entries[index.row()];
  switch (role) {
  case NameRole:
    return e.name;
  case IconRole:
    return e.icon;
  case DescriptionRole:
    return e.description;
  case ShortcutRole:
    return e.shortcut;
  case KeybindIdRole:
    return e.keybindId;
  default:
    return {};
  }
}

QHash<int, QByteArray> KeybindSettingsModel::roleNames() const {
  return {{NameRole, "name"},
          {IconRole, "icon"},
          {DescriptionRole, "description"},
          {ShortcutRole, "shortcut"},
          {KeybindIdRole, "keybindId"}};
}

QString KeybindSettingsModel::selectedName() const {
  return (m_selectedRow >= 0 && m_selectedRow < static_cast<int>(m_entries.size()))
             ? m_entries[m_selectedRow].name
             : QString();
}

QString KeybindSettingsModel::selectedDescription() const {
  return (m_selectedRow >= 0 && m_selectedRow < static_cast<int>(m_entries.size()))
             ? m_entries[m_selectedRow].description
             : QString();
}

QString KeybindSettingsModel::selectedIcon() const {
  return (m_selectedRow >= 0 && m_selectedRow < static_cast<int>(m_entries.size()))
             ? m_entries[m_selectedRow].icon
             : QString();
}

bool KeybindSettingsModel::hasSelection() const {
  return m_selectedRow >= 0 && m_selectedRow < static_cast<int>(m_entries.size());
}

void KeybindSettingsModel::setFilter(const QString &text) { rebuild(text); }

void KeybindSettingsModel::select(int row) {
  if (row == m_selectedRow) return;
  m_selectedRow = row;
  emit selectedChanged();
}

void KeybindSettingsModel::moveUp() {
  if (m_selectedRow > 0) select(m_selectedRow - 1);
}

void KeybindSettingsModel::moveDown() {
  if (m_selectedRow < rowCount() - 1) select(m_selectedRow + 1);
}

static QString shortcutString(const Keyboard::Shortcut &s) { return s.toDisplayString(); }

QString KeybindSettingsModel::validateShortcut(int key, int modifiers) const {
  Keyboard::Shortcut shortcut(static_cast<Qt::Key>(key), static_cast<Qt::KeyboardModifiers>(modifiers));
  if (!shortcut.hasMods() && !shortcut.isFunctionKey()) return QStringLiteral("Modifier required");
  if (auto existing = KeybindManager::instance()->findBoundInfo(shortcut)) {
    return QStringLiteral("Already bound to \"%1\"").arg(existing->name);
  }
  return {};
}

void KeybindSettingsModel::setShortcut(int row, int key, int modifiers) {
  if (row < 0 || row >= static_cast<int>(m_entries.size())) return;
  auto &e = m_entries[row];
  Keyboard::Shortcut shortcut(static_cast<Qt::Key>(key), static_cast<Qt::KeyboardModifiers>(modifiers));
  KeybindManager::instance()->setKeybind(static_cast<Keybind>(e.keybindId), shortcut);
  e.shortcut = shortcutString(shortcut);
  auto idx = index(row);
  emit dataChanged(idx, idx, {ShortcutRole});
}

QString KeybindSettingsModel::shortcutDisplayString(int key, int modifiers) const {
  Keyboard::Shortcut shortcut(static_cast<Qt::Key>(key), static_cast<Qt::KeyboardModifiers>(modifiers));
  return shortcut.toDisplayString();
}

void KeybindSettingsModel::rebuild(const QString &filter) {
  beginResetModel();
  m_entries.clear();

  auto query = filter.toStdString();
  std::vector<Scored<Entry>> scored;

  for (const auto &[id, info] : KeybindManager::instance()->orderedInfoList()) {
    int sc = 0;
    if (!query.empty()) {
      auto name = info->name.toStdString();
      auto desc = info->description.toStdString();
      sc = fuzzy::scoreWeighted({{name, 1.0}, {desc, 0.5}}, query);
      if (sc <= 0) continue;
    }

    Entry e;
    e.keybindId = static_cast<int>(id);
    e.name = info->name;
    e.icon = info->icon;
    e.description = info->description;
    if (auto s = KeybindManager::instance()->resolve(id); s.isValid()) e.shortcut = shortcutString(s);
    scored.push_back({.data = std::move(e), .score = sc});
  }

  if (!query.empty()) std::stable_sort(scored.begin(), scored.end(), std::greater{});

  m_entries.reserve(scored.size());
  for (auto &s : scored)
    m_entries.emplace_back(std::move(s.data));

  endResetModel();
  m_selectedRow = -1;
  select(m_entries.empty() ? -1 : 0);
}
