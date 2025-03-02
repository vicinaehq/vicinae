#pragma once
#include "ui/ellided-label.hpp"
#include "ui/grid-item-content-widget.hpp"
#include "ui/omni-list-item-widget.hpp"
#include <qboxlayout.h>
#include <qevent.h>

class GridItemWidget2 : public OmniListItemWidget {
  QVBoxLayout *layout;
  EllidedLabel *titleLabel;
  EllidedLabel *subtitleLabel;
  void resizeEvent(QResizeEvent *event) override;

public:
  GridItemContentWidget *main;

  GridItemWidget2(QWidget *parent = nullptr);
  void selectionChanged(bool selected) override;

  void setTitle(const QString &title);
  void setSubtitle(const QString &subtitle);
  void setTooltipText(const QString &tooltip);

  void setWidget(QWidget *widget);
  size_t spacing();
};
