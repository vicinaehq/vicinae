#include "grid-item-content-widget.hpp"
#include "theme.hpp"
#include <qnamespace.h>
#include <qwidget.h>

int GridItemContentWidget::borderWidth() const { return 3; }

void GridItemContentWidget::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);

  if (m_widget) repositionCenterWidget();
}

void GridItemContentWidget::paintEvent(QPaintEvent *event) {
  auto &theme = ThemeService::instance().theme();
  int borderRadius = 10;

  QPainter painter(this);

  painter.setRenderHint(QPainter::Antialiasing, true);

  QPainterPath path;
  path.addRoundedRect(rect(), borderRadius, borderRadius);

  painter.setClipPath(path);

  QColor backgroundColor(theme.resolveTint(SemanticColor::MainHoverBackground));

  painter.fillPath(path, backgroundColor);

  if (m_selected || underMouse()) {
    QPen pen(m_selected ? theme.resolveTint(SemanticColor::TextPrimary)
                        : theme.resolveTint(SemanticColor::TextSecondary),
             3);
    painter.setPen(pen);
  } else {
    painter.setPen(Qt::NoPen);
  }

  painter.drawPath(path);
}

int GridItemContentWidget::insetForSize(Inset inset, QSize size) const {
  switch (inset) {
  case Inset::Small:
    return size.width() * 0.10;
  case Inset::Medium:
    return size.width() * 0.20;
  case Inset::Large:
    return size.width() * 0.30;
  }

  return 0;
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
  if (m_widget) { m_widget->deleteLater(); }

  m_widget = widget;
  widget->setParent(this);
  repositionCenterWidget();
}

QWidget *GridItemContentWidget::widget() const { return m_widget; }

void GridItemContentWidget::setSelected(bool selected) {
  this->m_selected = selected;
  update();
}

void GridItemContentWidget::setInset(Inset inset) {
  m_inset = inset;
  if (m_widget) repositionCenterWidget();
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
