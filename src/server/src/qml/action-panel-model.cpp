#include "action-panel-model.hpp"
#include "lib/fuzzy/fuzzy-searchable.hpp"
#include "theme.hpp"
#include <QKeyEvent>
#include <utility>

template <> struct fuzzy::FuzzySearchable<std::shared_ptr<AbstractAction>> {
  static int score(const std::shared_ptr<AbstractAction> &action, std::string_view query) {
    auto title = action->title().toStdString();
    int s = 0;
    return fts::fuzzy_match(query, title, s) ? s : 0;
  }
};

ActionPanelModel::ActionPanelModel(QObject *parent) : QAbstractListModel(parent) {
  connect(&ThemeService::instance(), &ThemeService::themeChanged, this, [this]() {
    if (rowCount() > 0) emit dataChanged(index(0), index(rowCount() - 1), {IconSource});
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

void ActionPanelModel::setState(std::unique_ptr<ActionPanelState> state) { setStateFrom(state.get()); }

void ActionPanelModel::setStateFrom(const ActionPanelState *state) {
  beginResetModel();

  m_title = state ? state->title() : QString();
  m_allActions.clear();
  m_sections.clear();
  m_flat.clear();
  m_filterQuery.clear();

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
  if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(m_flat.size())) return {};

  const auto &item = m_flat[index.row()];

  if (item.kind == FlatItem::SectionHeader) {
    switch (role) {
    case ItemType:
      return QStringLiteral("section");
    case Title:
      return (item.sectionIdx >= 0 && std::cmp_less(item.sectionIdx, m_sections.size()))
                 ? m_sections[item.sectionIdx].name
                 : QString();
    case IconSource:
      return QString();
    case ShortcutLabel:
      return QString();
    case IsSubmenu:
      return false;
    case IsPrimary:
      return false;
    case IsDanger:
      return false;
    default:
      return {};
    }
  }

  if (item.kind == FlatItem::Divider) {
    switch (role) {
    case ItemType:
      return QStringLiteral("divider");
    default:
      return {};
    }
  }

  if (item.sectionIdx < 0 || std::cmp_greater_equal(item.sectionIdx, m_sections.size())) return {};
  const auto &section = m_sections[item.sectionIdx];
  if (item.actionIdx < 0 || std::cmp_greater_equal(item.actionIdx, section.actions.size())) return {};
  const auto &action = section.actions[item.actionIdx];

  switch (role) {
  case ItemType:
    return QStringLiteral("action");
  case Title:
    return action->title();
  case IconSource: {
    auto icon = action->icon();
    return icon ? qml::imageSourceFor(*icon) : QString();
  }
  case ShortcutLabel: {
    auto shortcut = action->shortcut();
    return shortcut ? shortcut->toDisplayString() : QString();
  }
  case IsSubmenu:
    return action->isSubmenu();
  case IsPrimary:
    return action->isPrimary();
  case IsDanger:
    return action->style() == AbstractAction::Style::Danger;
  default:
    return {};
  }
}

QHash<int, QByteArray> ActionPanelModel::roleNames() const {
  return {
      {ItemType, "itemType"},           {Title, "title"},         {IconSource, "iconSource"},
      {ShortcutLabel, "shortcutLabel"}, {IsSubmenu, "isSubmenu"}, {IsPrimary, "isPrimary"},
      {IsDanger, "isDanger"},
  };
}

void ActionPanelModel::activate(int index) {
  if (index < 0 || std::cmp_greater_equal(index, m_flat.size())) return;
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
  if (action->autoClose()) { emit closeRequested(); }
}

void ActionPanelModel::setFilter(const QString &text) {
  auto query = text.toStdString();
  if (m_filterQuery == query) return;
  m_filterQuery = std::move(query);

  beginResetModel();
  rebuildFlatList();
  endResetModel();
}

int ActionPanelModel::nextSelectableIndex(int from, int direction) const {
  int const count = static_cast<int>(m_flat.size());
  if (count == 0) return from;

  int idx = from + direction;
  if (idx < 0)
    idx = count - 1;
  else if (idx >= count)
    idx = 0;

  while (idx != from) {
    if (m_flat[idx].kind == FlatItem::ActionItem) return idx;
    idx += direction;
    if (idx < 0)
      idx = count - 1;
    else if (idx >= count)
      idx = 0;
  }

  return from;
}

void ActionPanelModel::rebuildFlatList() {
  m_flat.clear();
  m_flat.reserve(m_allActions.size() + m_sections.size() * 2);

  bool needsDivider = false;
  std::vector<Scored<int>> scored;

  for (int s = 0; std::cmp_less(s, m_sections.size()); ++s) {
    const auto &section = m_sections[s];
    fuzzy::fuzzyFilter<std::shared_ptr<AbstractAction>>(section.actions, m_filterQuery, scored);

    if (scored.empty()) continue;

    if (needsDivider) { m_flat.emplace_back(FlatItem::Divider); }

    if (!section.name.isEmpty()) { m_flat.emplace_back(FlatItem::SectionHeader, s); }

    for (const auto &entry : scored) {
      m_flat.emplace_back(FlatItem::ActionItem, s, entry.data);
    }

    needsDivider = true;
  }
}

bool ActionPanelModel::activateByShortcut(int key, int modifiers) {
  auto mods = static_cast<Qt::KeyboardModifiers>(modifiers);
  QKeyEvent const event(QEvent::KeyPress, key, mods);

  for (int i = 0; std::cmp_less(i, m_flat.size()); ++i) {
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
