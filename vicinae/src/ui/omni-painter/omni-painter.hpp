#pragma once
#include "theme.hpp"
#include <qbrush.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qpoint.h>
#include <qwindowdefs.h>

class OmniPainter : public QPainter {
public:
  enum ImageMaskType { NoMask, CircleMask, RoundedRectangleMask };

  static ImageMaskType maskForName(const QString &name);

  void fillRect(QRect rect, const QColor &color, int radius = 0, float alpha = 1.0);
  void fillRect(QRect rect, const ThemeLinearGradient &lgrad, int radius = 0, float alpha = 1.0);
  void fillRect(QRect rect, const ThemeRadialGradient &rgrad, int radius = 0, float alpha = 1.0);
  void fillRect(QRect rect, const ColorLike &colorLike, int radius = 0, float alpha = 1.0);

  void drawPixmap(const QRect &rect, const QPixmap &pixmap, ImageMaskType mask);
  void drawPixmap(const QPoint &pos, const QPixmap &pixmap) { QPainter::drawPixmap(pos, pixmap); }
  void drawPixmap(const QRect &rect, const QPixmap &pixmap) { QPainter::drawPixmap(rect, pixmap); }
  void drawBlurredPixmap(const QPixmap &pixmap, int blurRadius = 10);

  void setThemePen(const ColorLike &color, int width = 1);
  void setThemeBrush(const ColorLike &color);

  QColor resolveColor(SemanticColor color) const;

  QBrush colorBrush(const ColorLike &color) const;

  using QPainter::QPainter;
};
