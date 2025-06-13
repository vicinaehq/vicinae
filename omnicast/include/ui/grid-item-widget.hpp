#pragma once
#include "ui/ellided-label.hpp"
#include "ui/grid-item-content-widget.hpp"
#include "ui/omni-list-item-widget.hpp"
#include <qboxlayout.h>
#include <qevent.h>
#include <qwidget.h>

class GridItemWidget2 : public OmniListItemWidget {
  QVBoxLayout *layout;
  EllidedLabel *titleLabel;
  EllidedLabel *subtitleLabel;
  void resizeEvent(QResizeEvent *event) override;
  double m_aspectRatio = 1;

public:
  GridItemContentWidget *main;

  void enterEvent(QEnterEvent *event) override {
    qDebug() << "enter event";
    OmniListItemWidget::enterEvent(event);
  }

  GridItemWidget2(QWidget *parent = nullptr);
  void selectionChanged(bool selected) override;

  void setTitle(const QString &title);
  void setSubtitle(const QString &subtitle);
  void setTooltipText(const QString &tooltip);

  void clearTransientState() override;

  void setInset(int inset) { main->setInset(inset); }
  void setAspectRatio(double ratio) {
    if (ratio == m_aspectRatio) return;

    m_aspectRatio = ratio;
    main->setFixedSize(width(), width() / ratio);
  }
  void setWidget(QWidget *widget);
  QWidget *widget() const;
  size_t spacing();
};
