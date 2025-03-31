#pragma once
#include "theme.hpp"
#include <qpainter.h>
#include <qpixmap.h>
#include <qwindowdefs.h>

class OmniPainter : public QPainter {
public:
  enum ImageMaskType { NoMask, CircleMask, RoundedRectangleMask };

  void fillRect(QRect rect, const QColor &color, int radius = 0, float alpha = 1.0);
  void fillRect(QRect rect, const ThemeLinearGradient &lgrad, int radius = 0, float alpha = 1.0);
  void fillRect(QRect rect, const ThemeRadialGradient &rgrad, int radius = 0, float alpha = 1.0);
  void fillRect(QRect rect, const ColorLike &colorLike, int radius = 0, float alpha = 1.0);

  void drawPixmap(const QRect &rect, const QPixmap &pixmap, ImageMaskType mask = NoMask);
  void drawBlurredPixmap(const QPixmap &pixmap, int blurRadius = 10);

  using QPainter::QPainter;
};
