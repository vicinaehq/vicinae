#include "ui/typography/typography.hpp"
#include "ui/omni-painter.hpp"
#include <qboxlayout.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qwidget.h>

void TypographyWidget::updateText() {
  if (m_label->wordWrap() || !m_autoEllide) {
    m_label->setText(m_text);
    updateGeometry();
    return;
  }

  QFontMetrics metrics = m_label->fontMetrics();
  QString text = metrics.elidedText(m_text, m_elideMode, width());
  m_label->setText(text);
  updateGeometry();
}

void TypographyWidget::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  // qCritical() << "size" << event->size() << "hint" << sizeHint() << m_text;
  updateText();
}

void TypographyWidget::setEllideMode(Qt::TextElideMode mode) {
  m_elideMode = mode;
  updateText();
}

void TypographyWidget::paintEvent(QPaintEvent *event) {
  OmniPainter painter(this);
  QPalette pal = palette();

  pal.setBrush(QPalette::WindowText, painter.colorBrush(m_color));

  m_label->setPalette(pal);
  QWidget::paintEvent(event);
}

QSize TypographyWidget::minimumSizeHint() const {
  if (!m_label->wordWrap()) { return QSize(fontMetrics().horizontalAdvance("..."), fontMetrics().height()); }
  return m_label->minimumSizeHint();
}

QLabel *TypographyWidget::measurementLabel() const {
  static QLabel *label = nullptr;

  if (!label) {
    label = new QLabel;
    label->hide();
    label->blockSignals(true);
    label->setUpdatesEnabled(false);
  }

  return label;
}

void TypographyWidget::setAutoEllide(bool autoEllide) {
  m_autoEllide = autoEllide;
  updateText();
}

QSize TypographyWidget::sizeHint() const {
  if (!m_label->wordWrap() && m_autoEllide) {
    auto ruler = measurementLabel();

    ruler->setFont(m_label->font());
    ruler->setText(m_text);
    ruler->setAlignment(m_label->alignment());
    ruler->setContentsMargins(contentsMargins());

    auto hint = ruler->sizeHint();

    return hint;
  }

  return m_label->sizeHint();
}

void TypographyWidget::setSize(TextSize size) {
  QFont _font = font();

  _font.setPointSize(m_theme.pointSize(size));
  _font.setWeight(m_weight);

  m_size = size;
  m_label->setFont(_font);
  updateGeometry();
}

void TypographyWidget::setText(const QString &text) {
  m_text = text;
  updateText();
}

QString TypographyWidget::text() const { return m_text; }

void TypographyWidget::setColor(const ColorLike &color) {
  m_color = color;
  update();
}

void TypographyWidget::setFont(const QFont &f) {
  QFont font(f);

  font.setPointSize(m_theme.pointSize(m_size));
  font.setWeight(m_weight);
  m_label->setFont(font);
  updateText();
}

void TypographyWidget::setAlignment(Qt::Alignment align) { m_label->setAlignment(align); }

void TypographyWidget::setWordWrap(bool wrap) { m_label->setWordWrap(wrap); }

void TypographyWidget::setFontWeight(QFont::Weight weight) {
  QFont _font = font();

  _font.setPointSize(m_theme.pointSize(m_size));
  _font.setWeight(weight);
  m_label->setFont(_font);

  m_weight = weight;
  updateText();
}

void TypographyWidget::clear() { setText(""); }

TypographyWidget::TypographyWidget(QWidget *parent) : QWidget(parent) {
  QVBoxLayout *layout = new QVBoxLayout;

  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(m_label);
  setLayout(layout);
  setSize(TextSize::TextRegular);
}
