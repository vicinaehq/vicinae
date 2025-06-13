#include "ui/grid-item-content-widget.hpp"
#include "theme.hpp"
#include <qnamespace.h>
#include <qwidget.h>

int GridItemContentWidget::borderWidth() const { return 3; }

void GridItemContentWidget::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);

  if (_widget) {
    _widget->setFixedSize(innerWidgetSize());
    _widget->move(_inset, _inset);
  }
}

void GridItemContentWidget::paintEvent(QPaintEvent *event) {
  auto &theme = ThemeService::instance().theme();
  int borderRadius = 10;

  QPainter painter(this);

  painter.setRenderHint(QPainter::Antialiasing, true);

  QPainterPath path;
  path.addRoundedRect(rect(), borderRadius, borderRadius);

  painter.setClipPath(path);

  QColor backgroundColor(theme.colors.mainHoveredBackground);

  painter.fillPath(path, backgroundColor);

  if (selected || hovered) {
    QPen pen(selected ? theme.colors.text : theme.colors.subtext, 3);
    painter.setPen(pen);
  } else {
    painter.setPen(Qt::NoPen);
  }

  painter.drawPath(path);
}

void GridItemContentWidget::mousePressEvent(QMouseEvent *event) { emit clicked(); }
void GridItemContentWidget::mouseDoubleClickEvent(QMouseEvent *event) { emit doubleClicked(); }

void GridItemContentWidget::hideEvent(QHideEvent *event) { tooltip->hide(); }

QSize GridItemContentWidget::innerWidgetSize() const { return {width() - _inset * 2, height() - _inset * 2}; }

void GridItemContentWidget::setWidget(QWidget *widget) {
  if (_widget) { _widget->deleteLater(); }

  _widget = widget;
  widget->setParent(this);
  widget->setFixedSize(innerWidgetSize());
  widget->move(_inset, _inset);
}

QWidget *GridItemContentWidget::widget() const { return _widget; }

void GridItemContentWidget::setSelected(bool selected) {
  this->selected = selected;
  update();
}

void GridItemContentWidget::setInset(int inset) {
  _inset = inset;
  update();
}

void GridItemContentWidget::setHovered(bool hovered) {
  this->hovered = hovered;

  if (hovered) {
    showTooltip();
  } else {
    hideTooltip();
  }

  update();
}

void GridItemContentWidget::hideTooltip() { tooltip->hide(); }

void GridItemContentWidget::showTooltip() {}

void GridItemContentWidget::setTooltipText(const QString &text) { tooltip->setText(text); }

bool GridItemContentWidget::event(QEvent *event) {
  if (event->type() == QEvent::HoverEnter) {
    setHovered(true);
  } else if (event->type() == QEvent::HoverLeave) {
    setHovered(false);
  }

  return QWidget::event(event);
}

GridItemContentWidget::GridItemContentWidget()
    : _widget(nullptr), selected(false), hovered(false), tooltip(new Tooltip(this)), _inset(10) {
  setAttribute(Qt::WA_Hover);
  tooltip->hide();
  tooltip->setTarget(this);
}

GridItemContentWidget::~GridItemContentWidget() { tooltip->deleteLater(); }
