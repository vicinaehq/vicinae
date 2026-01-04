#include "builtin_icon.hpp"
#include "ui/vlist/common/vertical-list-model.hpp"
#include <algorithm>
#include <ranges>

class BuiltinIconSelectorModel : public vicinae::ui::VerticalListModel<BuiltinIcon> {
public:
  BuiltinIconSelectorModel() {
    auto icons = BuiltinIconService::mapping() | std::ranges::to<std::vector>();
    m_icons.reserve(BuiltinIconService::mapping().size());

    for (const auto &pair : BuiltinIconService::mapping()) {
      m_icons.emplace_back(pair);
    }

    std::ranges::sort(m_icons, [](auto &&a, auto &&b) {
      return std::ranges::lexicographical_compare(std::string_view{a.second}, std::string_view{b.second}) > 0;
    });
  }

  virtual ItemData createItemData(const Item &item) const override {
    return {.title = BuiltinIconService::nameForIcon(item), .icon = item};
  }

  virtual int sectionCount() const override { return 1; }

  virtual int sectionItemCount(int id) const override { return m_icons.size(); };

  virtual Item sectionItemAt(int id, int itemIdx) const override { return m_icons.at(itemIdx).first; }

  virtual int sectionIdFromIndex(int idx) const override { return idx; }

  virtual std::string_view sectionName(int id) const override { return ""; }

  virtual VListModel::StableID stableId(const Item &item) const override = 0;

private:
  std::vector<std::pair<BuiltinIcon, const char *>> m_icons;
};
