#include "qml-keybind-settings-model.hpp"
#include "keyboard/keybind-manager.hpp"
#include "keyboard/keyboard.hpp"
#include <unordered_map>

QmlKeybindSettingsModel::QmlKeybindSettingsModel(QObject *parent) : QAbstractListModel(parent) {
  rebuild({});
}

int QmlKeybindSettingsModel::rowCount(const QModelIndex &) const {
  return static_cast<int>(m_entries.size());
}

QVariant QmlKeybindSettingsModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(m_entries.size()))
    return {};
  const auto &e = m_entries[index.row()];
  switch (role) {
  case NameRole: return e.name;
  case IconRole: return e.icon;
  case DescriptionRole: return e.description;
  case ShortcutRole: return e.shortcut;
  case KeybindIdRole: return e.keybindId;
  default: return {};
  }
}

QHash<int, QByteArray> QmlKeybindSettingsModel::roleNames() const {
  return {{NameRole, "name"},
          {IconRole, "icon"},
          {DescriptionRole, "description"},
          {ShortcutRole, "shortcut"},
          {KeybindIdRole, "keybindId"}};
}

QString QmlKeybindSettingsModel::selectedName() const {
  return (m_selectedRow >= 0 && m_selectedRow < static_cast<int>(m_entries.size()))
             ? m_entries[m_selectedRow].name
             : QString();
}

QString QmlKeybindSettingsModel::selectedDescription() const {
  return (m_selectedRow >= 0 && m_selectedRow < static_cast<int>(m_entries.size()))
             ? m_entries[m_selectedRow].description
             : QString();
}

QString QmlKeybindSettingsModel::selectedIcon() const {
  return (m_selectedRow >= 0 && m_selectedRow < static_cast<int>(m_entries.size()))
             ? m_entries[m_selectedRow].icon
             : QString();
}

bool QmlKeybindSettingsModel::hasSelection() const {
  return m_selectedRow >= 0 && m_selectedRow < static_cast<int>(m_entries.size());
}

void QmlKeybindSettingsModel::setFilter(const QString &text) { rebuild(text); }

void QmlKeybindSettingsModel::select(int row) {
  if (row == m_selectedRow) return;
  m_selectedRow = row;
  emit selectedChanged();
}

void QmlKeybindSettingsModel::moveUp() {
  if (m_selectedRow > 0) select(m_selectedRow - 1);
}

void QmlKeybindSettingsModel::moveDown() {
  if (m_selectedRow < rowCount() - 1) select(m_selectedRow + 1);
}

static QString shortcutString(const Keyboard::Shortcut &s) {
  QKeySequence seq(static_cast<int>(s.key()) | static_cast<int>(s.mods()));
  return seq.toString(QKeySequence::NativeText);
}

QString QmlKeybindSettingsModel::validateShortcut(int key, int modifiers) const {
  Keyboard::Shortcut shortcut(static_cast<Qt::Key>(key),
                              static_cast<Qt::KeyboardModifiers>(modifiers));
  if (!shortcut.hasMods() && !shortcut.isFunctionKey())
    return QStringLiteral("Modifier required");
  if (auto existing = KeybindManager::instance()->findBoundInfo(shortcut)) {
    return QStringLiteral("Already bound to \"%1\"").arg(existing->name);
  }
  return {};
}

void QmlKeybindSettingsModel::setShortcut(int row, int key, int modifiers) {
  if (row < 0 || row >= static_cast<int>(m_entries.size())) return;
  auto &e = m_entries[row];
  Keyboard::Shortcut shortcut(static_cast<Qt::Key>(key),
                              static_cast<Qt::KeyboardModifiers>(modifiers));
  KeybindManager::instance()->setKeybind(static_cast<Keybind>(e.keybindId), shortcut);
  e.shortcut = shortcutString(shortcut);
  auto idx = index(row);
  emit dataChanged(idx, idx, {ShortcutRole});
}

// clang-format off
static const std::unordered_map<Qt::Key, std::pair<QString, QString>> keyDisplayOverloads = {
    {Qt::Key_Meta,      {"icon", "command-symbol"}},
    {Qt::Key_Alt,       {"icon", "option-symbol"}},
    {Qt::Key_Control,   {"icon", "chevron-up"}},
    {Qt::Key_Shift,     {"icon", "keyboard-shift"}},
    {Qt::Key_Return,    {"icon", "enter-key"}},
    {Qt::Key_Left,      {"icon", "arrow-left"}},
    {Qt::Key_Right,     {"icon", "arrow-right"}},
    {Qt::Key_Up,        {"icon", "arrow-up"}},
    {Qt::Key_Down,      {"icon", "arrow-down"}},
    {Qt::Key_Backspace, {"icon", "arrow-counter-clockwise"}},
    {Qt::Key_Tab,       {"icon", "tab-key"}},
    {Qt::Key_Space,     {"icon", "space-key"}},
};
// clang-format on

QVariantList QmlKeybindSettingsModel::shortcutKeys(int key, int modifiers) const {
  Keyboard::Shortcut shortcut(static_cast<Qt::Key>(key),
                              static_cast<Qt::KeyboardModifiers>(modifiers));
  QVariantList result;
  for (const auto &k : shortcut.allKeys()) {
    QVariantMap entry;
    if (auto it = keyDisplayOverloads.find(k); it != keyDisplayOverloads.end()) {
      entry[QStringLiteral("type")] = it->second.first;
      entry[QStringLiteral("value")] = it->second.second;
    } else if (auto str = Keyboard::stringForKey(k)) {
      entry[QStringLiteral("type")] = QStringLiteral("text");
      entry[QStringLiteral("value")] = str->toUpper();
    } else {
      continue;
    }
    result.append(entry);
  }
  return result;
}

void QmlKeybindSettingsModel::rebuild(const QString &filter) {
  beginResetModel();
  m_entries.clear();
  for (const auto &[id, info] : KeybindManager::instance()->orderedInfoList()) {
    auto matches = filter.isEmpty() || info->name.contains(filter, Qt::CaseInsensitive) ||
                   info->description.contains(filter, Qt::CaseInsensitive);
    if (!matches) continue;
    Entry e;
    e.keybindId = static_cast<int>(id);
    e.name = info->name;
    e.icon = info->icon;
    e.description = info->description;
    if (auto s = KeybindManager::instance()->resolve(id); s.isValid())
      e.shortcut = shortcutString(s);
    m_entries.push_back(std::move(e));
  }
  endResetModel();
  m_selectedRow = -1;
  select(m_entries.empty() ? -1 : 0);
}
