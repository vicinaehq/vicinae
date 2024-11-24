#include "ui/color_circle.hpp"

ColorCircle::ColorCircle(const QString &color, QSize size, QWidget *parent)
    : QWidget(parent), s(color), size(size) {
  setFixedSize(size);
  qDebug() << "constructed ColorCircle";
}

void ColorCircle::paintEvent(QPaintEvent *event) {
  QPainter painter(this);

  painter.setRenderHint(QPainter::Antialiasing);

  painter.setPen(Qt::NoPen);

  int w = width();
  int h = height();

  painter.setBrush(QColor("#BBBBBB"));
  painter.drawEllipse(0, 0, w, h);
  painter.setBrush(QColor(s));
  painter.drawEllipse(3, 3, w - 6, h - 6);
}

QSize ColorCircle::sizeHint() const { return size; }
