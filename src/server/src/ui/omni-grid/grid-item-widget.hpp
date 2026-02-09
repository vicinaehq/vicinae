#pragma once
#include "grid-item-content-widget.hpp"
#include "ui/omni-list/omni-list-item-widget.hpp"
#include "ui/typography/typography.hpp"
#include <qboxlayout.h>
#include <qevent.h>
#include <qwidget.h>

class GridItemWidget : public OmniListItemWidget {
  // actual inset depends on the size of the main widget

  TypographyWidget *titleLabel = new TypographyWidget;
  TypographyWidget *subtitleLabel = new TypographyWidget;
  void resizeEvent(QResizeEvent *event) override;
  double m_aspectRatio = 1;

public:
  GridItemContentWidget *main;

  void enterEvent(QEnterEvent *event) override { OmniListItemWidget::enterEvent(event); }

  GridItemWidget(QWidget *parent = nullptr);
  void selectionChanged(bool selected) override;

  void setTitle(const QString &title);
  void setSubtitle(const QString &subtitle);
  void setTooltipText(const QString &tooltip);

  void clearTransientState() override;

  void setInset(GridItemContentWidget::Inset inset) { main->setInset(inset); }

  void setAspectRatio(double ratio) {
    if (ratio == m_aspectRatio) return;

    m_aspectRatio = ratio;
    main->setFixedSize(width(), width() / ratio);
  }
  void setWidget(QWidget *widget);
  QWidget *widget() const;
  size_t spacing();
};
