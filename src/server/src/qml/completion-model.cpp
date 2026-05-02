#include "completion-model.hpp"
#include "fuzzy/fuzzy-searchable.hpp"
#include <utility>

template <> struct fuzzy::FuzzySearchable<CompletionModel::Item> {
  static int score(const CompletionModel::Item &item, std::string_view query) {
    return fuzzy::scoreWeighted({{item.title, 1.0}}, query);
  }
};

CompletionModel::CompletionModel(QObject *parent) : QAbstractListModel(parent) {}

void CompletionModel::setItems(const QVariantList &items) {
  beginResetModel();
  m_sections.clear();
  m_filterQuery.clear();

  Section section;
  section.items.reserve(items.size());
  for (const auto &v : items) {
    const auto m = v.toMap();
    auto title = m.value(QStringLiteral("title")).toString();
    if (title.isEmpty()) title = m.value(QStringLiteral("displayName")).toString();
    section.items.push_back({
        .title = title.toStdString(),
        .iconSource = m.value(QStringLiteral("iconSource")).toString(),
        .data = m,
    });
  }
  m_sections.push_back(std::move(section));

  rebuildFlatList();
  endResetModel();
  emit countChanged();
}

void CompletionModel::setSections(const QVariantList &sections) {
  beginResetModel();
  m_sections.clear();
  m_filterQuery.clear();

  m_sections.reserve(sections.size());
  for (const auto &sv : sections) {
    const auto sm = sv.toMap();
    Section section;
    section.name = sm.value(QStringLiteral("title")).toString();
    const auto items = sm.value(QStringLiteral("items")).toList();
    section.items.reserve(items.size());
    for (const auto &v : items) {
      const auto m = v.toMap();
      auto title = m.value(QStringLiteral("title")).toString();
      if (title.isEmpty()) title = m.value(QStringLiteral("displayName")).toString();
      section.items.push_back({
          .title = title.toStdString(),
          .iconSource = m.value(QStringLiteral("iconSource")).toString(),
          .data = m,
      });
    }
    m_sections.push_back(std::move(section));
  }

  rebuildFlatList();
  endResetModel();
  emit countChanged();
}

void CompletionModel::setFilter(const QString &query) {
  auto q = query.toStdString();
  if (m_filterQuery == q) return;
  m_filterQuery = std::move(q);

  beginResetModel();
  rebuildFlatList();
  endResetModel();
  emit countChanged();
}

int CompletionModel::nextSelectableIndex(int from, int direction) const {
  const auto count = static_cast<int>(m_flat.size());
  if (count == 0) return from;

  int idx = from + direction;
  if (idx < 0)
    idx = count - 1;
  else if (idx >= count)
    idx = 0;

  while (idx != from) {
    if (m_flat[idx].kind == FlatItem::Entry) return idx;
    idx += direction;
    if (idx < 0)
      idx = count - 1;
    else if (idx >= count)
      idx = 0;
  }

  return from;
}

QVariantMap CompletionModel::itemDataAt(int index) const {
  if (index < 0 || std::cmp_greater_equal(index, m_flat.size())) return {};
  const auto &item = m_flat[index];
  if (item.kind != FlatItem::Entry) return {};
  return m_sections[item.sectionIdx].items[item.itemIdx].data;
}

int CompletionModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) return 0;
  return static_cast<int>(m_flat.size());
}

QVariant CompletionModel::data(const QModelIndex &index, int role) const {
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
    default:
      return {};
    }
  }

  if (item.sectionIdx < 0 || std::cmp_greater_equal(item.sectionIdx, m_sections.size())) return {};
  const auto &section = m_sections[item.sectionIdx];
  if (item.itemIdx < 0 || std::cmp_greater_equal(item.itemIdx, section.items.size())) return {};
  const auto &entry = section.items[item.itemIdx];

  switch (role) {
  case ItemType:
    return QStringLiteral("item");
  case Title:
    return QString::fromStdString(entry.title);
  case IconSource:
    return entry.iconSource;
  case ItemData:
    return entry.data;
  default:
    return {};
  }
}

QHash<int, QByteArray> CompletionModel::roleNames() const {
  return {
      {ItemType, "itemType"},
      {Title, "title"},
      {IconSource, "iconSource"},
      {ItemData, "itemData"},
  };
}

void CompletionModel::rebuildFlatList() {
  m_flat.clear();

  const bool showHeaders = m_sections.size() > 1;
  std::vector<Scored<int>> scored;

  for (int s = 0; std::cmp_less(s, m_sections.size()); ++s) {
    const auto &section = m_sections[s];
    fuzzy::fuzzyFilter<Item>(section.items, m_filterQuery, scored);

    if (scored.empty()) continue;

    if (showHeaders && !section.name.isEmpty()) {
      m_flat.push_back({.kind = FlatItem::SectionHeader, .sectionIdx = s});
    }

    for (const auto &entry : scored) {
      m_flat.push_back({.kind = FlatItem::Entry, .sectionIdx = s, .itemIdx = entry.data});
    }
  }
}
