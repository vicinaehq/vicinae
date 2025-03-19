#pragma once
#include "theme.hpp"
#include <qpainter.h>

class OmniPainter : public QPainter {
public:
  void fillRect(QRect rect, const QColor &color, int radius = 0, float alpha = 1.0);
  void fillRect(QRect rect, const ThemeLinearGradient &lgrad, int radius = 0, float alpha = 1.0);
  void fillRect(QRect rect, const ThemeRadialGradient &rgrad, int radius = 0, float alpha = 1.0);
  void fillRect(QRect rect, const ColorLike &colorLike, int radius = 0, float alpha = 1.0);

  using QPainter::QPainter;
};
