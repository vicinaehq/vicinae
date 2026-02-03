#pragma once
#include "common.hpp"
#include "lib/fzf.hpp"
#include "ui/default-list-item-widget/default-list-item-widget.hpp"
#include "ui/vlist/common/section-model.hpp"
#include "ui/vlist/vlist.hpp"

namespace vicinae::ui {

/**
 * A section list model specifically tailored to vertical list with simple items.
 * This takes care of the widget creation/refresh and only requires the implementation of
 * the section related methods.
 */
template <typename ItemType, typename SectionId = int>
class VerticalListModel : public SectionListModel<ItemType, SectionId> {
public:
  struct ItemData {
    QString title;
    std::optional<QString> subtitle;
    std::optional<ImageURL> icon;
    std::optional<QString> alias;
    AccessoryList accessories;
    bool isActive = false;
  };

  virtual ItemData createItemData(const ItemType &item) const = 0;

  virtual int sectionCount() const override = 0;
  virtual int sectionItemCount(SectionId id) const override = 0;
  virtual ItemType sectionItemAt(SectionId id, int itemIdx) const override = 0;
  virtual SectionId sectionIdFromIndex(int idx) const override = 0;
  virtual std::string_view sectionName(SectionId id) const override = 0;
  virtual VListModel::StableID stableId(const ItemType &item) const override = 0;

  virtual int sectionItemHeight(SectionId id) const override { return 41; }

  virtual VListModel::WidgetType *createItemWidget(const ItemType &item) const final override {
    return new DefaultListItemWidget;
  }

  virtual void refreshItemWidget(const ItemType &item, VListModel::WidgetType *widget) const final override {
    auto w = static_cast<DefaultListItemWidget *>(widget);
    ItemData data = createItemData(item);
    w->setName(data.title);
    w->setIconUrl(data.icon);
    w->setSubtitle(data.subtitle.value_or(""));
    w->setActive(data.isActive);
    w->setAccessories(data.accessories);
    w->setAlias(data.alias.value_or(""));
  }

  VListModel::WidgetTag widgetTag(const ItemType &item) const override {
    static const size_t tag = typeid(DefaultListItemWidget).hash_code();
    return tag;
  }
};

}; // namespace vicinae::ui

// namespace vicinae::ui
