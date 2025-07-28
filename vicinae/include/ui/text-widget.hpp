#pragma once
#include "theme.hpp"
#include <optional>
#include <qlabel.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qwidget.h>

class TextWidget : public QLabel {
  QString _text;
  std::optional<ColorLike> _color;

  void paintEvent(QPaintEvent *event) override;

public:
  TextWidget(const QString &s = "");
  TextWidget &setColor(const std::optional<ColorLike> &color);
};
