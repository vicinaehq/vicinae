#pragma once
#include <QPainter>
#include "theme.hpp"
#include <cstdint>

class OmniPainter : public QPainter {
public:
  static QString serializeColor(const ColorLike &color);

  enum ImageMaskType : std::uint8_t { NoMask, CircleMask, RoundedRectangleMask };

  static ImageMaskType maskForName(const QString &name);

  void fillRect(QRect rect, const QColor &color, int radius = 0, float alpha = 1.0);
  void fillRect(QRect rect, const ColorLike &colorLike, int radius = 0, float alpha = 1.0);

  void drawPixmap(const QRect &rect, const QPixmap &pixmap, ImageMaskType mask);
  void drawPixmap(const QPoint &pos, const QPixmap &pixmap) { QPainter::drawPixmap(pos, pixmap); }
  void drawPixmap(const QRect &rect, const QPixmap &pixmap) { QPainter::drawPixmap(rect, pixmap); }

  void setThemePen(const ColorLike &color, int width = 1);
  void setThemeBrush(const ColorLike &color);

  static QColor resolveColor(const ColorLike &color);
  QBrush colorBrush(const ColorLike &color) const;

  using QPainter::QPainter;
};
