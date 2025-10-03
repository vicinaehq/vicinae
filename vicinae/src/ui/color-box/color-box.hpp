#pragma once
#include "theme.hpp"
#include <qpainterpath.h>
#include <qwidget.h>

class ColorBox : public QWidget {
public:
  void setColor(const ColorLike &color);
  void setBorderRadius(int radius);

protected:
  void paintEvent(QPaintEvent *event) override;
  QSize sizeHint() const override;

private:
  ColorLike m_color;
  int m_radius = 0;
};
