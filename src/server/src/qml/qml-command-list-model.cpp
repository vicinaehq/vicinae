#include "qml-command-list-model.hpp"
#include "navigation-controller.hpp"

void QmlCommandListModel::initialize(ApplicationContext *ctx) { m_ctx = ctx; }

int QmlCommandListModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) return 0;
  return static_cast<int>(m_flat.size());
}

QVariant QmlCommandListModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(m_flat.size()))
    return {};

  const auto &flat = m_flat[index.row()];

  if (flat.kind == FlatItem::SectionHeader) {
    switch (role) {
    case IsSection: return true;
    case IsSelectable: return false;
    case SectionName: return m_sectionInfos[flat.sectionIdx].name;
    case Title: return QString();
    case Subtitle: return QString();
    case IconSource: return QString();
    case Accessory: return QString();
    default: return {};
    }
  }

  switch (role) {
  case IsSection: return false;
  case IsSelectable: return true;
  case SectionName: return QString();
  case Title: return itemTitle(flat.sectionIdx, flat.itemIdx);
  case Subtitle: return itemSubtitle(flat.sectionIdx, flat.itemIdx);
  case IconSource: return itemIconSource(flat.sectionIdx, flat.itemIdx);
  case Accessory: return itemAccessory(flat.sectionIdx, flat.itemIdx);
  default: return {};
  }
}

QHash<int, QByteArray> QmlCommandListModel::roleNames() const {
  return {
    {IsSection, "isSection"},
    {IsSelectable, "isSelectable"},
    {SectionName, "sectionName"},
    {Title, "title"},
    {Subtitle, "subtitle"},
    {IconSource, "iconSource"},
    {Accessory, "itemAccessory"},
  };
}

void QmlCommandListModel::setSelectedIndex(int index) {
  if (index == m_selectedIndex) return;
  m_selectedIndex = index;

  if (index < 0 || index >= static_cast<int>(m_flat.size())) {
    if (m_ctx) m_ctx->navigation->clearActions();
    return;
  }

  const auto &flat = m_flat[index];
  if (flat.kind == FlatItem::SectionHeader) {
    if (m_ctx) m_ctx->navigation->clearActions();
    return;
  }

  auto panel = createActionPanel(flat.sectionIdx, flat.itemIdx);
  if (panel) {
    panel->finalize();
    if (m_ctx) m_ctx->navigation->setActions(std::move(panel));
  }
}

void QmlCommandListModel::refreshActionPanel() {
  int idx = m_selectedIndex;
  m_selectedIndex = -1;
  setSelectedIndex(idx);
}

void QmlCommandListModel::activateSelected() {
  if (!m_ctx) return;
  m_ctx->navigation->executePrimaryAction();
}

int QmlCommandListModel::nextSelectableIndex(int from, int direction) const {
  int idx = from + direction;
  int count = static_cast<int>(m_flat.size());

  while (idx >= 0 && idx < count) {
    if (m_flat[idx].kind != FlatItem::SectionHeader) return idx;
    idx += direction;
  }

  return from;
}

void QmlCommandListModel::setSections(const std::vector<SectionInfo> &sections) {
  beginResetModel();
  m_sectionInfos = sections;
  m_selectedIndex = -1;
  rebuildFlatList();
  endResetModel();
}

void QmlCommandListModel::rebuildFlatList() {
  m_flat.clear();

  for (int s = 0; s < static_cast<int>(m_sectionInfos.size()); ++s) {
    const auto &sec = m_sectionInfos[s];
    if (sec.count == 0) continue;

    m_flat.push_back({.kind = FlatItem::SectionHeader, .sectionIdx = s, .itemIdx = -1});

    for (int i = 0; i < sec.count; ++i) {
      m_flat.push_back({.kind = FlatItem::DataItem, .sectionIdx = s, .itemIdx = i});
    }
  }
}

QString QmlCommandListModel::imageSourceFor(const ImageURL &url) const {
  switch (url.type()) {
  case ImageURLType::Builtin: {
    QString base = QString("image://vicinae/builtin:%1").arg(url.name());
    if (auto bg = url.backgroundTint())
      base += QString("?bg=%1").arg(static_cast<int>(*bg));
    if (auto fg = url.foregroundTint())
      base += QString(base.contains('?') ? "&fg=%1" : "?fg=%1").arg(static_cast<int>(*fg));
    return base;
  }
  case ImageURLType::System:
    return QString("image://vicinae/system:%1").arg(url.name());
  case ImageURLType::Local:
    return QString("image://vicinae/local:%1").arg(url.name());
  case ImageURLType::Emoji:
    return QString("image://vicinae/emoji:%1").arg(url.name());
  case ImageURLType::Http:
  case ImageURLType::Https:
    return QStringLiteral("image://vicinae/builtin:globe-01");
  case ImageURLType::Favicon:
    return QStringLiteral("image://vicinae/builtin:globe-01");
  default:
    return QStringLiteral("image://vicinae/builtin:question-mark-circle");
  }
}
