#include "form-tag-picker-suggestions-model.hpp"
#include "image-url.hpp"
#include "view-utils.hpp"
#include <algorithm>

void FormTagPickerSuggestionsModel::setSourceItems(std::vector<TagPickerItemModel> items) {
  m_items = std::move(items);
  setSelectFirstOnReset(false);
  applyFilter();
  setSelectFirstOnReset(true);
}

void FormTagPickerSuggestionsModel::setPickedItems(const QStringList &pickedItems) {
  if (m_pickedItems == pickedItems) return;
  m_pickedItems = pickedItems;
  applyFilter();
}

QVariant FormTagPickerSuggestionsModel::data(const QModelIndex &index, int role) const {
  if (role == ValueRole) { return itemValueAt(index.row()); }
  return FuzzyListModel<TagPickerItemModel>::data(index, role);
}

QHash<int, QByteArray> FormTagPickerSuggestionsModel::roleNames() const {
  auto roles = FuzzyListModel<TagPickerItemModel>::roleNames();
  roles.insert(ValueRole, "value");
  return roles;
}

QString FormTagPickerSuggestionsModel::itemValueAt(int row) const {
  int section = 0;
  int item = 0;
  if (!dataItemAt(row, section, item)) return {};
  return filteredItem(item).value;
}

void FormTagPickerSuggestionsModel::applyFilter() {
  m_filtered.clear();
  m_filtered.reserve(m_items.size());

  if (m_query.empty()) {
    for (int i = 0; std::cmp_less(i, m_items.size()); ++i) {
      if (m_pickedItems.contains(m_items[i].value)) continue;
      m_filtered.push_back({.data = i, .score = 0});
    }
  } else {
    for (int i = 0; std::cmp_less(i, m_items.size()); ++i) {
      if (m_pickedItems.contains(m_items[i].value)) continue;

      int const score = fuzzy::FuzzySearchable<TagPickerItemModel>::score(m_items[i], m_query);
      if (score > 0) { m_filtered.push_back({.data = i, .score = score}); }
    }

    std::stable_sort(m_filtered.begin(), m_filtered.end(), std::greater{});
  }

  std::vector<SectionInfo> sections;
  if (!m_filtered.empty()) { sections.push_back({.name = {}, .count = static_cast<int>(m_filtered.size())}); }
  commitSections(sections);
}

QString FormTagPickerSuggestionsModel::displayTitle(const TagPickerItemModel &item) const {
  return item.title;
}

QString FormTagPickerSuggestionsModel::displayIconSource(const TagPickerItemModel &item) const {
  if (!item.icon) return {};
  return qml::imageSourceFor(ImageURL(*item.icon));
}

QString FormTagPickerSuggestionsModel::itemId(int, int item) const { return filteredItem(item).value; }

std::unique_ptr<ActionPanelState>
FormTagPickerSuggestionsModel::buildActionPanel(const TagPickerItemModel &) const {
  return nullptr;
}
