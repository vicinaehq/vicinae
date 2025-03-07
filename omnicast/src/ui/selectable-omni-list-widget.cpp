#include "ui/selectable-omni-list-widget.hpp"
#include "ui/omni-list-item-widget.hpp"
#include <qevent.h>

void SelectableOmniListWidget::paintEvent(QPaintEvent *event) {
  QPainter painter(this);

  if (isSelected || isHovered) {
    int borderRadius = 10;
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing, true);

    QPainterPath path;
    path.addRoundedRect(rect(), borderRadius, borderRadius);

    painter.setClipPath(path);

    QColor backgroundColor("#282726");

    painter.fillPath(path, backgroundColor);
  }
}

void SelectableOmniListWidget::selectionChanged(bool selected) {
  this->isSelected = selected;
  update();
}

void SelectableOmniListWidget::mousePressEvent(QMouseEvent *event) {
  OmniListItemWidget::mousePressEvent(event);
  if (event->button() == Qt::LeftButton) { emit clicked(); }
}

void SelectableOmniListWidget::mouseDoubleClickEvent(QMouseEvent *event) {
  OmniListItemWidget::mouseDoubleClickEvent(event);
  if (event->button() == Qt::LeftButton) { emit doubleClicked(); }
}

void SelectableOmniListWidget::setHovered(bool hovered) {
  this->isHovered = hovered;
  update();
}

void SelectableOmniListWidget::enterEvent(QEnterEvent *event) {
  OmniListItemWidget::enterEvent(event);
  Q_UNUSED(event);
  setHovered(true);
}

void SelectableOmniListWidget::leaveEvent(QEvent *event) {
  OmniListItemWidget::leaveEvent(event);
  Q_UNUSED(event);
  setHovered(false);
}

SelectableOmniListWidget::SelectableOmniListWidget(QWidget *parent)
    : OmniListItemWidget(parent), isSelected(false), isHovered(false) {
  setAttribute(Qt::WA_Hover, true);
}

bool SelectableOmniListWidget::selected() const { return isSelected; }
bool SelectableOmniListWidget::hovered() const { return isHovered; }
