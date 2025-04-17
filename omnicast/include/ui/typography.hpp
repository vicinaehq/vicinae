#pragma once
#include "theme.hpp"
#include "ui/omni-painter.hpp"
#include <qevent.h>
#include <qfont.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qtextformat.h>
#include <qwidget.h>

class TypographyWidget : public QLabel {
  TextSize _size;
  Qt::Alignment _align = Qt::AlignLeft | Qt::AlignVCenter;
  QString _text;
  QPixmap _pixmap;
  QFont::Weight _weight = QFont::Weight::Normal;
  QFont _font;
  ThemeService &theme = ThemeService::instance();
  ColorLike _color = theme.getTintColor(ColorTint::TextPrimary);

protected:
  void paintEvent(QPaintEvent *event) override {
    OmniPainter painter(this);
    QPalette pal = palette();

    pal.setBrush(QPalette::WindowText, painter.colorBrush(_color));

    setPalette(pal);
    QLabel::paintEvent(event);
  }

public:
  void setColor(const ColorLike &color) {
    _color = color;
    update();
  }

  void setFontWeight(QFont::Weight weight) {
    QFont _font = font();

    _font.setPointSize(theme.pointSize(_size));
    _font.setWeight(weight);
    setFont(_font);

    _weight = weight;
    updateGeometry();
  }

  void setSize(TextSize size) {
    QFont _font = font();

    _font.setPointSize(theme.pointSize(size));
    _font.setWeight(_weight);

    setFont(_font);

    _size = size;
    updateGeometry();
  }

  bool hasHeightForWidth() const override { return false; }

  TypographyWidget(TextSize size = TextSize::TextRegular, ColorTint color = ColorTint::TextPrimary,
                   QWidget *parent = nullptr)
      : QLabel(parent), _size(size), _color(color) {
    setSize(size);
  }
};
