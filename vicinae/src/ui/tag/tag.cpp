#include "ui/tag/tag.hpp"
#include "omni-icon.hpp"
#include "theme.hpp"
#include "ui/image/omnimg.hpp"
#include "ui/typography/typography.hpp"
#include "utils/layout.hpp"
#include <qnamespace.h>
#include <qpainter.h>
#include <qwidget.h>

void TagWidget::setupUI() {
  m_text = new TypographyWidget;
  m_image = new Omnimg::ImageWidget;

  m_image->setFixedSize(16, 16);
  m_image->hide();
  HStack().margins(5).add(m_image).add(m_text).spacing(10).imbue(this);
}

void TagWidget::paintEvent(QPaintEvent *event) {
  OmniPainter painter(this);

  if (m_color) {
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.fillRect(rect(), *m_color, 4, 0.2);
  }

  QWidget::paintEvent(event);
}

void TagWidget::setColor(const std::optional<ColorLike> &color) {
  m_text->setColor(color.value_or(SemanticColor::TextPrimary));
  m_image->setUrl(OmniIconUrl(m_image->url()).setFill(color));
  m_color = color;
  update();
}

void TagWidget::setIcon(const OmniIconUrl &icon) {
  m_image->setUrl(OmniIconUrl(icon).setFill(m_color.value_or(SemanticColor::TextPrimary)));
  m_image->show();
}

void TagWidget::setText(const QString &text) { m_text->setText(text); }

TagWidget::TagWidget() { setupUI(); }
