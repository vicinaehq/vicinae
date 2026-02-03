#pragma once
#include "services/calculator-service/calculator-service.hpp"
#include "ui/vlist/common/vertical-list-model.hpp"
#include <ranges>

class CalculatorHistoryModel : public vicinae::ui::VerticalListModel<CalculatorService::CalculatorRecord> {
public:
  void setGroupedRecords(CalculatorService::GroupedRecordList records) {
    m_groupedRecords = records;
    m_sectionNames = m_groupedRecords |
                     std::views::transform([](auto &&p) { return p.first.toStdString(); }) |
                     std::ranges::to<std::vector>();
    emit dataChanged();
  }

protected:
  ImageURL icon(const Item &record) const {
    switch (record.typeHint) {
    case AbstractCalculatorBackend::NORMAL:
      return ImageURL::builtin("calculator");
    case AbstractCalculatorBackend::CONVERSION:
      return ImageURL::builtin("switch");
    default:
      return ImageURL::builtin("calculator");
    }
  }

  ItemData createItemData(const Item &item) const override {
    return ItemData{.title = item.question, .icon = icon(item), .accessories = {{.text = item.answer}}};
  }

  VListModel::StableID stableId(const Item &item) const override { return hash(item.id); }

  int sectionCount() const override { return m_groupedRecords.size(); }
  int sectionItemCount(int id) const override { return m_groupedRecords[id].second.size(); }
  int sectionIdFromIndex(int idx) const override { return idx; }

  CalculatorService::CalculatorRecord sectionItemAt(int id, int itemIdx) const override {
    return m_groupedRecords[id].second[itemIdx];
  }

  std::string_view sectionName(int id) const override { return m_sectionNames[id]; }

private:
  CalculatorService::GroupedRecordList m_groupedRecords;
  std::vector<std::string> m_sectionNames; // FIXME: we need a better solution
};
