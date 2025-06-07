#include "ui/grid-item-widget.hpp"
#include "ui/grid-item-content-widget.hpp"
#include "ui/omni-list-item-widget.hpp"
#include <qwidget.h>

void GridItemWidget2::resizeEvent(QResizeEvent *event) {
  auto size = event->size();
  int height = size.width() / m_aspectRatio;

  main->setFixedSize({size.width(), height});
  OmniListItemWidget::resizeEvent(event);
}

GridItemWidget2::GridItemWidget2(QWidget *parent)
    : layout(new QVBoxLayout), main(new GridItemContentWidget), titleLabel(new EllidedLabel),
      subtitleLabel(new EllidedLabel) {
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(main);
  layout->addWidget(titleLabel);
  layout->addWidget(subtitleLabel);

  setLayout(layout);
  connect(main, &GridItemContentWidget::clicked, this, &OmniListItemWidget::clicked);
  connect(main, &GridItemContentWidget::doubleClicked, this, &OmniListItemWidget::doubleClicked);
}

void GridItemWidget2::setTitle(const QString &title) {
  titleLabel->setText(title);
  titleLabel->setVisible(!title.isEmpty());
}

void GridItemWidget2::setSubtitle(const QString &subtitle) {
  subtitleLabel->setText(subtitle);
  subtitleLabel->setVisible(!subtitle.isEmpty());
}

void GridItemWidget2::setTooltipText(const QString &tooltip) { main->setTooltipText(tooltip); }

void GridItemWidget2::selectionChanged(bool selected) { main->setSelected(selected); }

void GridItemWidget2::setWidget(QWidget *widget) { main->setWidget(widget); }
QWidget *GridItemWidget2::widget() const { return main->widget(); }

size_t GridItemWidget2::spacing() { return layout->spacing(); }
