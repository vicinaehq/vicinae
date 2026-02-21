#pragma once
#include "lib/fuzzy/fuzzy-searchable.hpp"
#include "navigation-controller.hpp"
#include "command-list-model.hpp"
#include <memory>
#include <numeric>
#include <string>
#include <vector>

/// Template base for QML list models backed by an in-memory dataset with fuzzy filtering.
/// Concrete subclasses declare Q_OBJECT and override the typed virtual methods.
/// No Q_OBJECT on the template — same pattern as BridgeView<T>.
template <typename T> class FuzzyListModel : public CommandListModel {
public:
  using CommandListModel::CommandListModel;

  /// Replace the full dataset and re-apply the current filter.
  void setItems(std::vector<T> items) {
    m_items = std::move(items);
    setSelectFirstOnReset(false);
    applyFilter();
    setSelectFirstOnReset(true);
    refreshActionPanel();
  }

  void setFilter(const QString &text) override {
    m_query = text.toStdString();
    applyFilter();
  }

protected:
  virtual QString displayTitle(const T &item) const = 0;
  virtual QString displaySubtitle(const T &) const { return {}; }
  virtual QString displayIconSource(const T &item) const = 0;
  virtual QVariant displayAccessory(const T &) const { return {}; }
  virtual std::unique_ptr<ActionPanelState> buildActionPanel(const T &item) const = 0;
  virtual void itemSelected(const T &) {}
  virtual QString sectionLabel() const { return {}; }

  /// Override to group filtered results into multiple sections.
  /// Default produces a single section. Subclasses can rearrange m_filtered
  /// and return custom sections (the sum of all section counts must equal m_filtered.size()).
  virtual void applyFilter() {
    fuzzy::fuzzyFilter<T>(std::span<const T>(m_items), m_query, m_filtered);

    QString label = sectionLabel();
    if (!label.isEmpty()) {
      label = label.replace("{count}", QString::number(m_filtered.size()));
    }

    std::vector<SectionInfo> sections;
    if (!m_filtered.empty()) {
      sections.push_back({.name = label, .count = static_cast<int>(m_filtered.size())});
    }
    commitSections(sections);
  }

  /// Access the underlying item for a filtered index.
  const T &filteredItem(int idx) const { return m_items[m_filtered[idx].data]; }

  /// Commit section layout after (re)arranging m_filtered.
  /// Builds the section-start offsets used by the dispatch methods.
  void commitSections(const std::vector<SectionInfo> &sections) {
    m_sectionStarts.clear();
    m_sectionStarts.reserve(sections.size());
    int offset = 0;
    for (const auto &s : sections) {
      m_sectionStarts.push_back(offset);
      offset += s.count;
    }
    setSections(sections);
  }

  std::vector<T> m_items;
  std::vector<Scored<int>> m_filtered;
  std::string m_query;

private:
  /// Resolve (section, itemWithinSection) to a global index in m_filtered.
  const T &resolveItem(int s, int i) const {
    int globalIdx = (s < static_cast<int>(m_sectionStarts.size()) ? m_sectionStarts[s] : 0) + i;
    return m_items[m_filtered[globalIdx].data];
  }

  QString itemTitle(int s, int i) const final { return displayTitle(resolveItem(s, i)); }
  QString itemSubtitle(int s, int i) const final { return displaySubtitle(resolveItem(s, i)); }
  QString itemIconSource(int s, int i) const final { return displayIconSource(resolveItem(s, i)); }
  QVariant itemAccessory(int s, int i) const final { return displayAccessory(resolveItem(s, i)); }

  std::unique_ptr<ActionPanelState> createActionPanel(int s, int i) const final {
    return buildActionPanel(resolveItem(s, i));
  }

  void onItemSelected(int s, int i) final { itemSelected(resolveItem(s, i)); }

  std::vector<int> m_sectionStarts;
};
