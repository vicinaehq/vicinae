#pragma once
#include <filesystem>
#include "ui/vlist/common/vertical-list-model.hpp"
#include "common/scored.hpp"
#include "utils.hpp"

class DMenuModel : public vicinae::ui::VerticalListModel<std::string_view> {
public:
  DMenuModel(QObject *parent = nullptr) { setParent(parent); }

  void setSectionName(std::string_view name) { m_sectionName = name; }

  ItemData createItemData(const std::string_view &item) const override {
    if (isFile(item)) {
      return ItemData{.title = getLastPathComponent(item).c_str(), .icon = ImageURL::fileIcon(item)};
    }
    return ItemData{.title = QString::fromUtf8(item.data(), item.size())};
  }

  int sectionCount() const override { return 1; }

  VListModel::StableID stableId(const std::string_view &item) const override { return hash(item); }

  int sectionIdFromIndex(int idx) const override { return idx; }

  int sectionItemCount(int id) const override { return m_entries.size(); }

  std::string_view sectionName(int id) const override { return m_sectionName; }

  std::string_view sectionItemAt(int id, int itemIdx) const override { return m_entries[itemIdx].data; }

  void setEntries(std::span<Scored<std::string_view>> entries) {
    m_entries = entries;
    emit dataChanged();
  }

private:
  static bool isFile(std::string_view entry) {
    if (!entry.starts_with('/')) return false; // avoid unnecessary stat
    std::error_code ec;
    return std::filesystem::exists(entry, ec);
  }

  std::string_view m_sectionName;
  std::span<Scored<std::string_view>> m_entries;
};
