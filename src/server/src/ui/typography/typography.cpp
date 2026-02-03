#include "ui/typography/typography.hpp"
#include "layout.hpp"
#include "theme.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include <qboxlayout.h>
#include <qevent.h>
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
  if (m_debugBackground) { painter.fillRect(rect(), ColorLike(SemanticColor::Red), 0, 1); }

  m_label->setPalette(pal);
  QWidget::paintEvent(event);
}

void TypographyWidget::setDebugHighlight() { m_debugBackground = true; }

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
  m_size = size;
  setFont(font());
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
  auto &theme = ThemeService::instance();

  font.setPointSizeF(theme.pointSize(m_size));
  font.setWeight(m_weight);
  m_label->setFont(font);
  updateGeometry();
}

void TypographyWidget::setAlignment(Qt::Alignment align) { m_label->setAlignment(align); }

void TypographyWidget::setWordWrap(bool wrap) { m_label->setWordWrap(wrap); }

void TypographyWidget::setFontWeight(QFont::Weight weight) {
  m_weight = weight;
  setFont(font());
}

void TypographyWidget::clear() { setText(""); }

TypographyWidget::TypographyWidget(QWidget *parent) : QWidget(parent), m_color(SemanticColor::TextPrimary) {
  m_label = new QLabel(this);
  m_label->setOpenExternalLinks(true);
  VStack().add(m_label).imbue(this);
  setSize(TextSize::TextRegular);

  connect(&ThemeService::instance(), &ThemeService::themeChanged, this, [this]() { setFont(font()); });
}
