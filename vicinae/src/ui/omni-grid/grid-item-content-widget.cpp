#include "grid-item-content-widget.hpp"
#include "layout.hpp"
#include "theme.hpp"
#include "theme/colors.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include <absl/strings/internal/str_format/extension.h>
#include <qnamespace.h>
#include <qwidget.h>

int GridItemContentWidget::borderWidth() const { return 3; }

void GridItemContentWidget::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  recalculate();
}

void GridItemContentWidget::paintEvent(QPaintEvent *event) {
  int borderRadius = 10;
  OmniPainter painter(this);
  QPainterPath path;
  QColor backgroundColor = painter.resolveColor(SemanticColor::GridItemBackground);

  painter.setRenderHint(QPainter::Antialiasing, true);

  if (m_selected || underMouse()) {
    painter.setThemePen(m_selected ? SemanticColor::GridItemSelectionOutline
                                   : SemanticColor::GridItemHoverOutline,
                        borderWidth());
    path.addRoundedRect(rect(), borderRadius, borderRadius);
    painter.setClipPath(path);
    painter.drawPath(path);
  }

  painter.setPen(Qt::NoPen);
  path.clear();
  path.addRoundedRect(rect().adjusted(borderWidth(), borderWidth(), -borderWidth(), -borderWidth()),
                      borderRadius, borderRadius);
  painter.fillPath(path, backgroundColor);
  painter.setClipPath(path);
  painter.drawPath(path);
}

int GridItemContentWidget::insetForSize(Inset inset, QSize size) const {
  int base = std::min(width(), height());

  switch (inset) {
  case Inset::None:
    return borderWidth() + 2; // minimum gap between hover/selected border and content
  case Inset::Small:
    return base * 0.10;
  case Inset::Medium:
    return base * 0.15;
  case Inset::Large:
    return base * 0.25;
  }

  return 0;
}

bool GridItemContentWidget::event(QEvent *event) {
  if (event->type() == QEvent::HoverEnter || event->type() == QEvent::HoverLeave) { recalculate(); }

  return QWidget::event(event);
}

void GridItemContentWidget::recalculate() {
  if (!m_widget) return;

  int margin = insetForSize(m_inset, size());

  layout()->setContentsMargins(margin, margin, margin, margin);
  m_widget->setFixedSize(rect().marginsRemoved(layout()->contentsMargins()).size());
}

void GridItemContentWidget::mousePressEvent(QMouseEvent *event) { emit clicked(); }
void GridItemContentWidget::mouseDoubleClickEvent(QMouseEvent *event) { emit doubleClicked(); }

void GridItemContentWidget::hideEvent(QHideEvent *event) { m_tooltip->hide(); }

QSize GridItemContentWidget::innerWidgetSize() const {
  int inset = insetForSize(m_inset, size());

  return {width() - inset * 2, height() - inset * 2};
}

void GridItemContentWidget::repositionCenterWidget() {
  int inset = insetForSize(m_inset, size());

  m_widget->setFixedSize(innerWidgetSize());
  m_widget->move(inset, inset);
}

void GridItemContentWidget::setWidget(QWidget *widget) {
  // if (m_widget) { m_widget->deleteLater(); }

  HStack().add(widget).imbue(this);

  m_widget = widget;
  recalculate();
  // widget->setParent(this);
  // repositionCenterWidget();
}

QWidget *GridItemContentWidget::widget() const { return m_widget; }

void GridItemContentWidget::setSelected(bool selected) {
  m_selected = selected;
  recalculate();
  update();
}

void GridItemContentWidget::setInset(Inset inset) {
  m_inset = inset;
  recalculate();
  update();
}

void GridItemContentWidget::hideTooltip() { m_tooltip->hide(); }

void GridItemContentWidget::showTooltip() {}

void GridItemContentWidget::setTooltipText(const QString &text) { m_tooltip->setText(text); }

GridItemContentWidget::GridItemContentWidget()
    : m_widget(nullptr), m_selected(false), m_tooltip(new TooltipWidget(this)), m_inset(Inset::Small) {
  setAttribute(Qt::WA_Hover);
  m_tooltip->hide();
  m_tooltip->setTarget(this);
}

GridItemContentWidget::~GridItemContentWidget() { m_tooltip->deleteLater(); }
