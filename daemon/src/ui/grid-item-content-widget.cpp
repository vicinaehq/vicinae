#include "ui/grid-item-content-widget.hpp"
#include <qwidget.h>

int GridItemContentWidget::borderWidth() const { return 3; }

QColor GridItemContentWidget::borderColor() const {
  if (selected) return "#BBBBBB";
  if (hovered) return "#888888";

  return "#202020";
}

void GridItemContentWidget::paintEvent(QPaintEvent *event) {
  int borderRadius = 10;

  QPainter painter(this);

  painter.setRenderHint(QPainter::Antialiasing, true);

  QPainterPath path;
  path.addRoundedRect(rect(), borderRadius, borderRadius);

  painter.setClipPath(path);

  QColor backgroundColor("#202020");

  painter.fillPath(path, backgroundColor);

  QPen pen(borderColor(), 3);
  painter.setPen(pen);
  painter.drawPath(path);
}

void GridItemContentWidget::mousePressEvent(QMouseEvent *event) { emit clicked(); }
void GridItemContentWidget::mouseDoubleClickEvent(QMouseEvent *event) { emit doubleClicked(); }

void GridItemContentWidget::setWidget(QWidget *widget) {
  if (layout->count() > 0) {
    auto old = layout->itemAt(0)->widget();

    layout->replaceWidget(old, widget);
    old->deleteLater();
  } else {
    layout->addWidget(widget, 0, Qt::AlignCenter);
  }
}

void GridItemContentWidget::setSelected(bool selected) {
  this->selected = selected;
  update();
}

void GridItemContentWidget::setInset(int inset) { layout->setContentsMargins(inset, inset, inset, inset); }

void GridItemContentWidget::setHovered(bool hovered) {
  this->hovered = hovered;

  if (hovered && !tooltip->text().isEmpty()) {
    showTooltip();
  } else {
    hideTooltip();
  }

  update();
}

void GridItemContentWidget::hideTooltip() { tooltip->hide(); }

void GridItemContentWidget::showTooltip() {
  const QPoint globalPos = mapToGlobal(QPoint(0, height() + 5));

  tooltip->adjustSize();
  tooltip->move(globalPos);
  tooltip->show();
}

void GridItemContentWidget::setTooltipText(const QString &text) { tooltip->setText(text); }

GridItemContentWidget::GridItemContentWidget()
    : layout(new QVBoxLayout), selected(false), hovered(false), tooltip(new Tooltip) {
  setLayout(layout);

  tooltip->hide();
}

GridItemContentWidget::~GridItemContentWidget() { tooltip->deleteLater(); }
