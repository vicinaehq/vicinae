#pragma once
#include "fuzzy/fuzzy-searchable.hpp"
#include "section-source.hpp"
#include <QString>
#include <span>
#include <string>
#include <vector>

template <> struct fuzzy::FuzzySearchable<QString> {
  static int score(const QString &item, std::string_view query) {
    return fuzzy::scoreWeighted({{item.toStdString(), 1.0}}, query);
  }
};

template <typename T> class FuzzySection : public SectionSource {
public:
  void setItems(std::vector<T> items) {
    m_items = std::move(items);
    refilter();
    notifyItemsRefreshed();
  }

  void setFilter(std::string_view query) override {
    m_query = std::string(query);
    refilter();
  }

  int count() const override { return static_cast<int>(m_filtered.size()); }

protected:
  const T &at(int i) const { return m_items[m_filtered[i].data]; }

  virtual QString displayTitle(const T &item) const = 0;
  virtual QString displaySubtitle(const T &) const { return {}; }
  virtual QString displayIconSource(const T &item) const = 0;
  virtual QVariantList displayAccessories(const T &) const { return {}; }
  virtual QString displayId(const T &item) const { return displayTitle(item); }
  virtual std::unique_ptr<ActionPanelState> buildActionPanel(const T &item) const = 0;

  std::vector<T> m_items;
  std::vector<Scored<int>> m_filtered;
  std::string m_query;

private:
  void refilter() { fuzzy::fuzzyFilter<T>(std::span<const T>(m_items), m_query, m_filtered); }

  QString itemId(int i) const override { return displayId(at(i)); }
  QString itemTitle(int i) const override { return displayTitle(at(i)); }
  QString itemSubtitle(int i) const override { return displaySubtitle(at(i)); }
  QString itemIconSource(int i) const override { return displayIconSource(at(i)); }
  QVariantList itemAccessories(int i) const override { return displayAccessories(at(i)); }

  std::unique_ptr<ActionPanelState> actionPanel(int i) const override { return buildActionPanel(at(i)); }
};
