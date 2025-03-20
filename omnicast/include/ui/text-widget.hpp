#pragma once
#include "theme.hpp"
#include "ui/omni-painter.hpp"
#include <optional>
#include <qlabel.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qwidget.h>

class TextWidget : public QLabel {
  QString _text;
  std::optional<ColorLike> _color;

  void paintEvent(QPaintEvent *event) override {
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

public:
  TextWidget(const QString &s = "") : _text(s) { setAttribute(Qt::WA_TranslucentBackground, true); }

  TextWidget &setColor(const std::optional<ColorLike> &color) {
    _color = color;
    update();

    return *this;
  }
};
