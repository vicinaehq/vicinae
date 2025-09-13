#include "ui/selectable-omni-list-widget/selectable-omni-list-widget.hpp"
#include "theme.hpp"
#include "ui/omni-list/omni-list-item-widget.hpp"
#include <qevent.h>
#include <qnamespace.h>

void SelectableOmniListWidget::paintEvent(QPaintEvent *event) {
  QPainter painter(this);

  if (isSelected || isHovered) {
    int borderRadius = 10;
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing, true);

    QPainterPath path;
    path.addRoundedRect(rect(), borderRadius, borderRadius);

    painter.setClipPath(path);

    auto &theme = ThemeService::instance().theme();

    // selection should always take precedence
    QColor backgroundColor(isSelected ? theme.resolveTint(SemanticColor::MainSelectedBackground)
                                      : theme.resolveTint(SemanticColor::MainHoverBackground));
    painter.fillPath(path, backgroundColor);
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
