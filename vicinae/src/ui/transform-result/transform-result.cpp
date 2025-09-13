#include "ui/transform-result/transform-result.hpp"
#include "../image/url.hpp"
#include "theme.hpp"
#include "ui/selectable-omni-list-widget/selectable-omni-list-widget.hpp"
#include "ui/typography/typography.hpp"
#include <qboxlayout.h>
#include <qfont.h>
#include <qlabel.h>
#include <qregion.h>
#include <qtextformat.h>
#include <qwidget.h>

int TransformResult::availableHeight() const {
  auto margins = contentsMargins();

  return height() - margins.top() - margins.bottom();
}

QWidget *TransformResult::createVContainer(QWidget *left, QWidget *right) const {
  auto container = new QWidget;
  auto layout = new QVBoxLayout();

  layout->setSpacing(10);
  layout->addWidget(left, 1, Qt::AlignCenter);
  layout->addWidget(right, 1, Qt::AlignBottom | Qt::AlignCenter);
  container->setLayout(layout);

  return container;
}

TypographyWidget *TransformResult::createLabel(const QString &text, QWidget *parent) const {
  auto label = new TypographyWidget(parent);

  label->setText(text);
  label->setAlignment(Qt::AlignCenter);
  label->setStyleSheet("font-size: 16pt; font-weight: bold;");

  return label;
}

QSize TransformResult::sizeHint() const {
  QSize size{0, 0};

  if (parentWidget()) { size.setWidth(parentWidget()->width()); }

  if (m_base) { size.setHeight(m_base->sizeHint().height()); }

  if (m_result) {
    int hint = m_result->sizeHint().height();

    if (hint > size.height()) size.setHeight(hint);
  }

  auto margins = contentsMargins();

  return {size.width(), size.height() + margins.top() + margins.bottom()};
}

void TransformResult::selectionChanged(bool value) {
  SelectableOmniListWidget::selectionChanged(value);
  setDividerVisible(value);
}

void TransformResult::paintEvent(QPaintEvent *event) {
  auto &theme = ThemeService::instance().theme();
  SelectableOmniListWidget::paintEvent(event);
  QPainter painter(this);
  int midW = width() / 2;
  int midH = height() / 2;
  auto margins = contentsMargins();

  painter.setPen(Qt::NoPen);
  painter.setBrush(theme.resolveTint(SemanticColor::Border));

  m_base->setFixedSize({midW, availableHeight()});
  m_base->move(0, margins.top());

  m_result->setFixedSize({midW, availableHeight()});
  m_result->move(midW, margins.top());

  if (selected() || hovered()) {
    painter.drawRect(midW, 0, 1, midH - m_arrowMid.height());
    painter.drawRect(midW, midH + m_arrowMid.width(), 1, midH - m_arrowMid.height());
  }

  m_arrowIcon->move(midW - m_arrowMid.width(), midH - m_arrowMid.height());
}

void TransformResult::setDividerVisible(bool visible) {
  m_isDividerVisible = visible;
  update();
}

void TransformResult::setBase(const QString &text, const QString &chip) { setBase(createLabel(text), chip); }

void TransformResult::setResult(const QString &text, const QString &chip) {
  setResult(createLabel(text), chip);
}

void TransformResult::setBase(QWidget *widget, const QString &chip) {
  auto label = new TypographyWidget;

  if (m_base) { m_base->deleteLater(); }

  label->setText(chip);
  m_base = createVContainer(widget, label);
  m_base->setParent(this);
  update();
}

void TransformResult::setResult(QWidget *widget, const QString &chip) {
  auto label = new TypographyWidget;

  if (m_result) { m_result->deleteLater(); }

  label->setText(chip);
  m_result = createVContainer(widget, label);
  m_result->setParent(this);
  update();
}

TransformResult::TransformResult() : m_isDividerVisible(true), m_base(nullptr), m_result(nullptr) {
  m_arrowIcon->setUrl(ImageURL::builtin("arrow-right"));
  m_arrowIcon->setFixedSize(25, 25);
  setContentsMargins(10, 10, 10, 10);
  m_arrowMid = {m_arrowIcon->width() / 2, m_arrowIcon->height() / 2};
}
