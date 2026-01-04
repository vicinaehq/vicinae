#pragma once
#include "ui/image/url.hpp"
#include "ui/vlist/common/vertical-list-model.hpp"

namespace vicinae::ui {

struct DropdownItem {
  QString id;
  QString title;
  std::optional<ImageURL> icon;
};

/**
 * A section list model specifically tailored to vertical list with simple items.
 * This takes care of the widget creation/refresh and only requires the implementation of
 * the section related methods.
 */
class DropdownModel : public VerticalListModel<DropdownItem, int> {
public:
  virtual ItemData createItemData(const Item &item) const override final {
    return {.title = item.title, .icon = item.icon};
  }

  virtual int sectionCount() const override { return 1; }

  virtual int sectionItemCount(int id) const override = 0;
  virtual Item sectionItemAt(int id, int itemIdx) const override = 0;
  virtual int sectionIdFromIndex(int idx) const override = 0;
  virtual std::string_view sectionName(int id) const override = 0;

  virtual VListModel::StableID stableId(const Item &item) const override {
    std::hash<QString> hash{};
    return hash(item.id);
  }

  virtual std::optional<Index> indexForId(const QString &id) {
    return findItemIf([&](auto &&opt) { return opt.id == id; });
  }
};

}; // namespace vicinae::ui
