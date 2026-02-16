#pragma once
#include "lib/fuzzy/fuzzy-searchable.hpp"
#include "navigation-controller.hpp"
#include "qml-command-list-model.hpp"
#include <memory>
#include <string>
#include <vector>

/// Template base for QML list models backed by an in-memory dataset with fuzzy filtering.
/// Concrete subclasses declare Q_OBJECT and override the typed virtual methods.
/// No Q_OBJECT on the template — same pattern as QmlBridgeView<T>.
template <typename T> class QmlFuzzyListModel : public QmlCommandListModel {
public:
  using QmlCommandListModel::QmlCommandListModel;

  /// Replace the full dataset and re-apply the current filter.
  void setItems(std::vector<T> items) {
    m_items = std::move(items);
    applyFilter();
  }

  void setFilter(const QString &text) override {
    m_query = text.toStdString();
    applyFilter();
  }

protected:
  // --- Typed interface: override in concrete models ---
  virtual QString displayTitle(const T &item) const = 0;
  virtual QString displaySubtitle(const T &) const { return {}; }
  virtual QString displayIconSource(const T &item) const = 0;
  virtual QString displayAccessory(const T &) const { return {}; }
  virtual std::unique_ptr<ActionPanelState> buildActionPanel(const T &item) const = 0;
  virtual void itemSelected(const T &) {}
  virtual QString sectionLabel() const { return {}; }

  /// Access item at a filtered index.
  const T &itemAt(int filteredIdx) const { return m_items[m_filtered[filteredIdx].data]; }

  const std::vector<T> &allItems() const { return m_items; }

private:
  // --- Sealed QmlCommandListModel overrides dispatching to typed virtuals ---
  QString itemTitle(int, int i) const final { return displayTitle(itemAt(i)); }
  QString itemSubtitle(int, int i) const final { return displaySubtitle(itemAt(i)); }
  QString itemIconSource(int, int i) const final { return displayIconSource(itemAt(i)); }
  QString itemAccessory(int, int i) const final { return displayAccessory(itemAt(i)); }

  std::unique_ptr<ActionPanelState> createActionPanel(int, int i) const final {
    return buildActionPanel(itemAt(i));
  }

  void onItemSelected(int, int i) final { itemSelected(itemAt(i)); }

  void applyFilter() {
    fuzzy::fuzzyFilter<T>(std::span<const T>(m_items), m_query, m_filtered);

    QString label = sectionLabel();
    if (!label.isEmpty()) {
      label = label.replace("{count}", QString::number(m_filtered.size()));
    }

    std::vector<SectionInfo> sections;
    if (!m_filtered.empty()) {
      sections.push_back({.name = label, .count = static_cast<int>(m_filtered.size())});
    }
    setSections(sections);
  }

  std::vector<T> m_items;
  std::vector<Scored<int>> m_filtered;
  std::string m_query;
};
