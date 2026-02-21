#include "action-panel-model.hpp"
#include "theme.hpp"
#include <QKeyEvent>

ActionPanelModel::ActionPanelModel(QObject *parent) : QAbstractListModel(parent) {
  connect(&ThemeService::instance(), &ThemeService::themeChanged, this, [this]() {
    if (rowCount() > 0)
      emit dataChanged(index(0), index(rowCount() - 1), {IconSource});
  });
}

ActionPanelModel::ActionPanelModel(std::unique_ptr<ActionPanelState> state, QObject *parent)
    : ActionPanelModel(parent) {
  setState(std::move(state));
}

ActionPanelModel::ActionPanelModel(const ActionPanelState *state, QObject *parent)
    : ActionPanelModel(parent) {
  setStateFrom(state);
}

void ActionPanelModel::setState(std::unique_ptr<ActionPanelState> state) {
  setStateFrom(state.get());
  // The unique_ptr is released here — actions are kept alive by shared_ptrs in m_allActions
}

void ActionPanelModel::setStateFrom(const ActionPanelState *state) {
  beginResetModel();

  m_title = state ? state->title() : QString();
  m_allActions.clear();
  m_sections.clear();
  m_flat.clear();
  m_filter.clear();
  m_filterWords.clear();

  if (state) {
    for (const auto &section : state->sections()) {
      SectionInfo info;
      info.name = section->name();
      for (const auto &action : section->actions()) {
        info.actions.push_back(action);
        m_allActions.push_back(action);
      }
      m_sections.push_back(std::move(info));
    }
  }

  rebuildFlatList();
  endResetModel();
  emit titleChanged();
}

int ActionPanelModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) return 0;
  return static_cast<int>(m_flat.size());
}

QVariant ActionPanelModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(m_flat.size()))
    return {};

  const auto &item = m_flat[index.row()];

  if (item.kind == FlatItem::SectionHeader) {
    switch (role) {
    case ItemType: return QStringLiteral("section");
    case Title: return (item.sectionIdx >= 0 && item.sectionIdx < static_cast<int>(m_sections.size()))
                           ? m_sections[item.sectionIdx].name : QString();
    case IconSource: return QString();
    case ShortcutLabel: return QString();
    case IsSubmenu: return false;
    case IsPrimary: return false;
    case IsDanger: return false;
    default: return {};
    }
  }

  if (item.kind == FlatItem::Divider) {
    switch (role) {
    case ItemType: return QStringLiteral("divider");
    default: return {};
    }
  }

  if (item.sectionIdx < 0 || item.sectionIdx >= static_cast<int>(m_sections.size()))
    return {};
  const auto &section = m_sections[item.sectionIdx];
  if (item.actionIdx < 0 || item.actionIdx >= static_cast<int>(section.actions.size()))
    return {};
  const auto &action = section.actions[item.actionIdx];

  switch (role) {
  case ItemType: return QStringLiteral("action");
  case Title: return action->title();
  case IconSource: {
    auto icon = action->icon();
    return icon ? qml::imageSourceFor(*icon) : QString();
  }
  case ShortcutLabel: {
    auto shortcut = action->shortcut();
    return shortcut ? shortcut->toDisplayString() : QString();
  }
  case IsSubmenu: return action->isSubmenu();
  case IsPrimary: return action->isPrimary();
  case IsDanger: return action->style() == AbstractAction::Style::Danger;
  default: return {};
  }
}

QHash<int, QByteArray> ActionPanelModel::roleNames() const {
  return {
      {ItemType, "itemType"},
      {Title, "title"},
      {IconSource, "iconSource"},
      {ShortcutLabel, "shortcutLabel"},
      {IsSubmenu, "isSubmenu"},
      {IsPrimary, "isPrimary"},
      {IsDanger, "isDanger"},
  };
}

void ActionPanelModel::activate(int index) {
  if (index < 0 || index >= static_cast<int>(m_flat.size())) return;
  const auto &item = m_flat[index];
  if (item.kind != FlatItem::ActionItem) return;

  const auto &action = m_sections[item.sectionIdx].actions[item.actionIdx];

  if (action->isSubmenu()) {
    auto *submenuAction = dynamic_cast<SubmenuAction *>(action.get());
    if (!submenuAction) return;

    auto subState = submenuAction->createSubmenuState();
    if (!subState) return;

    auto *subModel = new ActionPanelModel(std::move(subState), this);
    emit submenuRequested(subModel);
    return;
  }

  if (action->isPushView()) {
    emit actionExecuted(action.get());
    return;
  }

  emit actionExecuted(action.get());
  if (action->autoClose()) {
    emit closeRequested();
  }
}

void ActionPanelModel::setFilter(const QString &text) {
  if (m_filter == text) return;
  m_filter = text;
  m_filterWords = text.split(' ', Qt::SkipEmptyParts);

  beginResetModel();
  rebuildFlatList();
  endResetModel();
}

int ActionPanelModel::nextSelectableIndex(int from, int direction) const {
  int idx = from + direction;
  int count = static_cast<int>(m_flat.size());

  while (idx >= 0 && idx < count) {
    if (m_flat[idx].kind == FlatItem::ActionItem) return idx;
    idx += direction;
  }

  return from;
}

void ActionPanelModel::rebuildFlatList() {
  m_flat.clear();

  bool needsDivider = false;

  for (int s = 0; s < static_cast<int>(m_sections.size()); ++s) {
    const auto &section = m_sections[s];

    // Collect matching actions for this section
    std::vector<int> matchingActions;
    for (int a = 0; a < static_cast<int>(section.actions.size()); ++a) {
      if (m_filterWords.isEmpty() || matchesFilter(section.actions[a]->title())) {
        matchingActions.push_back(a);
      }
    }

    if (matchingActions.empty()) continue;

    // Add divider between sections (except before the first)
    if (needsDivider) {
      m_flat.push_back({.kind = FlatItem::Divider});
    }

    // Add section header if it has a name
    if (!section.name.isEmpty()) {
      m_flat.push_back({.kind = FlatItem::SectionHeader, .sectionIdx = s});
    }

    for (int a : matchingActions) {
      m_flat.push_back({.kind = FlatItem::ActionItem, .sectionIdx = s, .actionIdx = a});
    }

    needsDivider = true;
  }
}

bool ActionPanelModel::activateByShortcut(int key, int modifiers) {
  auto mods = static_cast<Qt::KeyboardModifiers>(modifiers);
  QKeyEvent event(QEvent::KeyPress, key, mods);

  for (int i = 0; i < static_cast<int>(m_flat.size()); ++i) {
    const auto &item = m_flat[i];
    if (item.kind != FlatItem::ActionItem) continue;
    auto &action = m_sections[item.sectionIdx].actions[item.actionIdx];
    if (action->isBoundTo(&event)) {
      activate(i);
      return true;
    }
  }
  return false;
}

bool ActionPanelModel::matchesFilter(const QString &title) const {
  for (const auto &word : m_filterWords) {
    bool found = false;
    for (const auto &titleWord : title.split(' ', Qt::SkipEmptyParts)) {
      if (titleWord.startsWith(word, Qt::CaseInsensitive)) {
        found = true;
        break;
      }
    }
    if (!found) return false;
  }
  return true;
}
