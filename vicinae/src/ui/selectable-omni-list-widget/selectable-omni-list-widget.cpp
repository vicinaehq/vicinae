#include "ui/selectable-omni-list-widget/selectable-omni-list-widget.hpp"
#include "theme.hpp"
#include "theme/colors.hpp"
#include "ui/omni-list/omni-list-item-widget.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include <qevent.h>
#include <qnamespace.h>

void SelectableOmniListWidget::paintEvent(QPaintEvent *event) {
  OmniPainter painter(this);

  if (isSelected || isHovered) {
    int borderRadius = 10;

    painter.setRenderHint(QPainter::Antialiasing, true);

    QPainterPath path;
    path.addRoundedRect(rect(), borderRadius, borderRadius);

    painter.setClipPath(path);

    auto &theme = ThemeService::instance().theme();

    // selection should always take precedence

    painter.setThemeBrush(isSelected ? selectedColor() : hoverColor());
    painter.setPen(Qt::NoPen);
    painter.drawPath(path);
  }
}

void SelectableOmniListWidget::selectionChanged(bool selected) {
  this->isSelected = selected;
  update();
}

void SelectableOmniListWidget::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    emit clicked();
    return;
  }
  if (event->button() == Qt::RightButton) {
    emit rightClicked();
    return;
  }
  OmniListItemWidget::mousePressEvent(event);
}

void SelectableOmniListWidget::mouseDoubleClickEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    emit doubleClicked();
    return;
  }

  OmniListItemWidget::mouseDoubleClickEvent(event);
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
