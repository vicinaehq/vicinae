#pragma once
#include "theme.hpp"
#include "ui/omni-painter.hpp"
#include <qevent.h>
#include <qfont.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qwidget.h>

class TypographyWidget : public QWidget {
  TextSize _size;
  Qt::Alignment _align = Qt::AlignLeft | Qt::AlignVCenter;
  QString _text;
  QPixmap _pixmap;
  ThemeService &theme = ThemeService::instance();
  ColorLike _color = theme.getTintColor(ColorTint::TextPrimary);

protected:
  void paintEvent(QPaintEvent *event) override {
    if (_pixmap.isNull()) { return; }

    QPainter painter(this);

    painter.drawPixmap(rect(), _pixmap);
  }

  QSize sizeHint() const override {
    auto fm = fontMetrics();

    return {fm.horizontalAdvance(_text), fm.height()};
  }

  void resizeEvent(QResizeEvent *event) override {
    QWidget::resizeEvent(event);
    recompute();
  }

  void recompute() {
    if (!size().isValid()) return;

    _pixmap = renderToPixmap();
    update();
  }

public:
  QPixmap renderToPixmap() {
    if (size().isEmpty()) return {};

    QPixmap pix(size());
    OmniPainter painter(&pix);

    qDebug() << "drawing pixmap of size" << pix.size();

    pix.fill(Qt::transparent);
    painter.setFont(font());
    painter.drawText(rect(), _align, _text);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.setPen(Qt::NoPen);
    painter.fillRect(rect(), _color);

    return pix;
  }

  void setText(const QString &text) {
    _text = text;
    recompute();
    updateGeometry();
  }

  void setColor(ColorTint color) {
    _color = theme.getTintColor(color);
    recompute();
  }

  void setColor(const ColorLike &color) {
    _color = color;
    recompute();
  }

  void setSize(TextSize size) {
    _size = size;
    recompute();
  }

  TypographyWidget(QWidget *parent = nullptr) : QWidget(parent), _size(TextSize::TextRegular) {
    auto f = font();

    f.setPointSize(theme.pointSize(_size));
    setFont(f);
  }
  TypographyWidget(TextSize size = TextSize::TextRegular, ColorTint color = ColorTint::TextPrimary,
                   QWidget *parent = nullptr)
      : QWidget(parent), _size(size), _color(color) {
    auto f = font();

    _color = theme.getTintColor(color);

    f.setPointSize(theme.pointSize(_size));
    setFont(f);
  }
};
