#include "ui/omni-painter.hpp"
#include "ui/text-widget.hpp"

void TextWidget::paintEvent(QPaintEvent *event) {
  if (_color) {
    QPixmap pixmap(size());
    pixmap.fill(Qt::transparent);

    {
      OmniPainter painter(&pixmap);

      painter.setFont(font());
      painter.drawText(rect(), alignment(), text());
      painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
      painter.setPen(Qt::NoPen);
      painter.fillRect(rect(), *_color);
    }

    QPainter painter(this);

    painter.drawPixmap(0, 0, pixmap);
  } else {
    QLabel::paintEvent(event);
  }
}

TextWidget &TextWidget::setColor(const std::optional<ColorLike> &color) {
  _color = color;
  update();

  return *this;
}

TextWidget::TextWidget(const QString &s) : _text(s) {}
