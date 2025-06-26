#include "ui/typography/typography.hpp"
#include "ui/omni-painter.hpp"
#include <qboxlayout.h>
#include <qnamespace.h>
#include <qwidget.h>

void TypographyWidget::updateText() {
  if (m_label->wordWrap()) {
    m_label->setText(m_text);
    return;
  }

  QFontMetrics metrics = m_label->fontMetrics();
  QString text = metrics.elidedText(m_text, Qt::ElideRight, width());
  m_label->setText(text);
  updateGeometry();
}

void TypographyWidget::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  // qCritical() << "size" << event->size() << "hint" << sizeHint() << m_text;
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

QSize TypographyWidget::sizeHint() const {
  if (!m_label->wordWrap()) {
    QLabel ruler;

    ruler.setFont(m_label->font());
    ruler.setText(m_text);

    return ruler.sizeHint();
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
  updateGeometry();
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
  updateGeometry();
}

void TypographyWidget::setAlignment(Qt::Alignment align) { m_label->setAlignment(align); }

void TypographyWidget::setWordWrap(bool wrap) { m_label->setWordWrap(wrap); }

void TypographyWidget::setFontWeight(QFont::Weight weight) {
  QFont _font = font();

  _font.setPointSize(m_theme.pointSize(m_size));
  _font.setWeight(weight);
  m_label->setFont(_font);

  m_weight = weight;
  updateGeometry();
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
