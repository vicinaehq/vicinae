#pragma once
#include "fuzzy/weighted-fuzzy-scorer.hpp"
#include "ui/vlist/common/vertical-list-model.hpp"
#include "utils.hpp"
#include <qfontdatabase.h>

class FontListModel : public vicinae::ui::VerticalListModel<std::string_view> {
public:
  FontListModel() {
    m_families = QFontDatabase::families() |
                 std::views::transform([](const QString &str) { return str.toStdString(); }) |
                 std::ranges::to<std::vector>();
  }

  void setFilter(QStringView query) {
    fuzzy::FuzzyScorer<std::string_view> scorer;
    std::string q = QString(query).toStdString();

    scorer.score(
        m_families | std::views::transform([](const std::string &s) { return std::string_view{s}; }), q,
        [](std::string_view query, std::string_view s) {
          int score = 0;
          fts::fuzzy_match(query, s, score);
          return score;
        },
        m_filteredItems);

    emit dataChanged();
  }

protected:
  int sectionCount() const override { return 1; }
  int sectionIdFromIndex(int idx) const override { return idx; }
  std::string_view sectionItemAt(int id, int itemIdx) const override { return m_filteredItems[itemIdx].data; }
  int sectionItemCount(int id) const override { return m_filteredItems.size(); }
  std::string_view sectionName(int id) const override { return "Results ({count})"; }
  ItemData createItemData(const std::string_view &item) const override {
    return ItemData{.title = qStringFromStdView(item)};
  }
  VListModel::StableID stableId(const std::string_view &item) const override { return hash(item); }

private:
  std::vector<std::string> m_families;
  std::vector<Scored<std::string_view>> m_filteredItems;
};
