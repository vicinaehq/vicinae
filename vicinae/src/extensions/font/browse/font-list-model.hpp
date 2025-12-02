#pragma once
#include "fuzzy/weighted-fuzzy-scorer.hpp"
#include "ui/vlist/common/vertical-list-model.hpp"
#include <qfontdatabase.h>

class FontListModel : public vicinae::ui::VerticalListModel<QStringView> {
public:
  FontListModel() { m_families = QFontDatabase::families(); }

  void setFilter(QStringView query) {
    fuzzy::FuzzyScorer<QStringView> scorer;
    std::string q = QString(query).toStdString();

    scorer.score(
        m_families | std::views::transform([](const QString &str) { return QStringView(str); }), q,
        [](std::string_view query, const QStringView &s) {
          auto buf = s.toUtf8();
          std::string_view view{buf.data(), (size_t)buf.size()};
          int score = 0;
          fts::fuzzy_match(query, view, score);
          return score;
        },
        m_filteredItems);

    emit dataChanged();
  }

protected:
  int sectionCount() const override { return 1; }
  int sectionIdFromIndex(int idx) const override { return idx; }
  QStringView sectionItemAt(int id, int itemIdx) const override { return m_filteredItems[itemIdx].data; }
  int sectionItemCount(int id) const override { return m_filteredItems.size(); }
  std::string_view sectionName(int id) const override { return "Results"; }
  ItemData createItemData(const QStringView &item) const override {
    return ItemData{.title = QString(item), .icon = ImageURL::builtin("text")};
  }
  VListModel::StableID stableId(const QStringView &item) const override { return hash(item); }

private:
  QStringList m_families;
  std::vector<Scored<QStringView>> m_filteredItems;
};
