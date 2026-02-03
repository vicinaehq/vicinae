#include "grid-item-widget.hpp"
#include "grid-item-content-widget.hpp"
#include "ui/omni-list/omni-list-item-widget.hpp"
#include <qwidget.h>

void GridItemWidget::resizeEvent(QResizeEvent *event) {
  auto size = event->size();
  int height = size.width() / m_aspectRatio;

  main->setFixedSize({size.width(), height});
  OmniListItemWidget::resizeEvent(event);
}

GridItemWidget::GridItemWidget(QWidget *parent) : layout(new QVBoxLayout), main(new GridItemContentWidget) {
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->addWidget(main);
  layout->addSpacing(10);
  layout->addWidget(titleLabel);
  layout->addWidget(subtitleLabel);

  subtitleLabel->setColor(SemanticColor::TextMuted);

  setLayout(layout);
  connect(main, &GridItemContentWidget::clicked, this, &OmniListItemWidget::clicked);
  connect(main, &GridItemContentWidget::doubleClicked, this, &OmniListItemWidget::doubleClicked);
}

void GridItemWidget::setTitle(const QString &title) {
  titleLabel->setText(title);
  titleLabel->setVisible(!title.isEmpty());
}

void GridItemWidget::setSubtitle(const QString &subtitle) {
  subtitleLabel->setText(subtitle);
  subtitleLabel->setVisible(!subtitle.isEmpty());
}

void GridItemWidget::setTooltipText(const QString &tooltip) { main->setTooltipText(tooltip); }

void GridItemWidget::selectionChanged(bool selected) { main->setSelected(selected); }

void GridItemWidget::setWidget(QWidget *widget) { main->setWidget(widget); }
QWidget *GridItemWidget::widget() const { return main->widget(); }

size_t GridItemWidget::spacing() { return layout->spacing(); }

void GridItemWidget::clearTransientState() { update(); }
