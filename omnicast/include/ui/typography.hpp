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
    // if (_pixmap.isNull() || _pixmap.size() != size()) { _pixmap = renderToPixmap(); }

    OmniPainter painter(this);

    QPalette pal = palette();

    pal.setBrush(QPalette::WindowText, painter.colorBrush(_color));

    setPalette(pal);

    QLabel::paintEvent(event);

    // painter.drawPixmap(rect(), _pixmap);
  }

  /*
  QSize sizeHint() const override {
    auto fm = fontMetrics();

    return {fm.horizontalAdvance(_text), fm.height()};
  }

  void resizeEvent(QResizeEvent *event) override {
    QWidget::resizeEvent(event);

    recompute();
  }
  */

  void recompute() {
    if (!size().isValid()) return;

    auto f = font();

    f.setPointSize(theme.pointSize(_size));
    f.setWeight(_weight);
    setFont(f);

    _pixmap = renderToPixmap();
    update();
  }

public:
  QPixmap renderToPixmap() {
    if (size().isEmpty()) return {};

    QPixmap pix(size());
    OmniPainter painter(&pix);

    pix.fill(Qt::transparent);
    painter.setFont(font());
    painter.drawText(rect(), _align, _text);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.setPen(Qt::NoPen);
    painter.fillRect(rect(), _color);

    return pix;
  }

  /*
  void setText(const QString &text) {
    _text = text;
    updateGeometry();
  }
  */

  void setColor(const ColorLike &color) {
    _color = color;
    update();
  }

  void setFontWeight(QFont::Weight weight) {
    _weight = weight;
    recompute();
  }

  void setSize(TextSize size) {
    _size = size;
    updateGeometry();
    recompute();
  }

  TypographyWidget(TextSize size = TextSize::TextRegular, ColorTint color = ColorTint::TextPrimary,
                   QWidget *parent = nullptr)
      : QLabel(parent), _size(size), _color(color) {}
};
