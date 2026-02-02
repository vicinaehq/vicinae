#pragma once
#include "ui/image/image.hpp"
#include "ui/image/url.hpp"
#include "ui/omni-grid/grid-item-widget.hpp"
#include "ui/vlist/common/grid-model.hpp"
#include "ui/vlist/vlist.hpp"

namespace vicinae::ui {

template <typename ItemType, typename SectionId>
class SimpleGridModel : public vicinae::ui::GridModel<ItemType, SectionId> {
public:
  struct GridData {
    ImageURL icon;
    std::optional<QString> title;
    std::optional<QString> subtitle;
    std::optional<QString> tooltip;
  };

  virtual GridData createItemData(const ItemType &item) const = 0;

  virtual VListModel::StableID stableId(const ItemType &item, int sectionId) const override = 0;
  virtual int sectionCount() const override = 0;
  virtual int sectionItemCount(SectionId id) const override = 0;

  virtual ItemType sectionItemAt(SectionId id, int itemIdx) const override = 0;
  virtual double sectionAspectRatio(SectionId id) const override = 0;
  virtual int sectionColumns(SectionId id) const override = 0;

  virtual SectionId sectionIdFromIndex(int idx) const override = 0;
  virtual std::string_view sectionName(SectionId id) const override = 0;

  virtual int itemHeight(const ItemType &item, int width, double ratio) const override {
    static GridItemWidget ruler;
    auto fm = ruler.fontMetrics();
    auto spacing = 10;
    int height = width / ratio;
    auto data = createItemData(item);

    if (data.title) { height += 15 + spacing; }
    if (data.subtitle) { height += 15 + spacing; }

    return height;
  }

  virtual VListModel::WidgetTag widgetTag(const ItemType &item) const override { return 1; }

  virtual VListModel::WidgetType *createItemWidget(const ItemType &type) const override {
    auto grid = new GridItemWidget;
    auto img = new ImageWidget;

    grid->setWidget(img);

    return grid;
  }

  virtual void refreshItemWidget(const ItemType &type, VListModel::WidgetType *widget,
                                 SectionId id) const override {
    auto w = static_cast<GridItemWidget *>(widget);
    auto img = static_cast<ImageWidget *>(w->widget());
    auto data = createItemData(type);
    img->setUrl(data.icon);
    w->setInset(GridItemContentWidget::Inset::Large);
    w->setAspectRatio(1);
    w->setTitle(data.title.value_or(""));
    w->setSubtitle(data.subtitle.value_or(""));
    w->setTooltipText(data.tooltip.value_or(""));
  }
};
}; // namespace vicinae::ui
