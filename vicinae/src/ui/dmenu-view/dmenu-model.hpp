#pragma once
#include <filesystem>
#include "ui/vlist/common/vertical-list-model.hpp"
#include "common/scored.hpp"
#include "utils.hpp"

struct DMenuEntry {
  std::string_view label;
  std::string_view quickLookPath;
};

class DMenuModel : public vicinae::ui::VerticalListModel<DMenuEntry> {
public:
  DMenuModel(QObject *parent = nullptr, bool noIcon = false) : m_noIcon(noIcon) { setParent(parent); }

  void setSectionName(std::string_view name) { m_sectionName = name; }

  ItemData createItemData(const DMenuEntry &item) const override {
    if (!m_noIcon && isFile(item)) {
      auto path = item.quickLookPath.empty() ? item.label : item.quickLookPath;
      return ItemData{.title = QString::fromUtf8(item.label.data(), item.label.size()),
                      .icon = ImageURL::fileIcon(path)};
    }
    return ItemData{.title = QString::fromUtf8(item.label.data(), item.label.size())};
  }

  int sectionCount() const override { return 1; }

  VListModel::StableID stableId(const DMenuEntry &item) const override {
    auto h1 = hash(item.label);
    auto h2 = hash(item.quickLookPath);
    return h1 ^ (h2 << 1);
  }

  int sectionIdFromIndex(int idx) const override { return idx; }

  int sectionItemCount(int id) const override { return m_entries.size(); }

  std::string_view sectionName(int id) const override { return m_sectionName; }

  DMenuEntry sectionItemAt(int id, int itemIdx) const override { return m_entries[itemIdx].data; }

  void setEntries(std::span<Scored<DMenuEntry>> entries) {
    m_entries = entries;
    emit dataChanged();
  }

private:
  static bool isFile(const DMenuEntry &entry) {
    std::string_view path = entry.quickLookPath.empty() ? entry.label : entry.quickLookPath;
    if (!path.starts_with('/')) return false; // avoid unnecessary stat
    std::error_code ec;
    return std::filesystem::exists(path, ec);
  }

  bool m_noIcon = false;
  std::string_view m_sectionName;
  std::span<Scored<DMenuEntry>> m_entries;
};
