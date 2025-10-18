#pragma once
#include "theme/colors.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include <qframe.h>

class HDivider : public QFrame {
  QColor _color;
  size_t _height;

  void paintEvent(QPaintEvent *event) override {
    OmniPainter painter(this);
    auto margins = contentsMargins();
    auto &theme = ThemeService::instance().theme();

    painter.setThemeBrush(SemanticColor::BackgroundBorder);
    painter.setThemePen(SemanticColor::BackgroundBorder);
    painter.drawRect(0, margins.top(), width(), _height);
  }

public:
  void setHeight(int height) {
    _height = height;
    setFixedHeight(height);
    updateGeometry();
  }

  void setColor(QColor color) {
    _color = color;
    update();
  }

  HDivider(QWidget *parent = nullptr) : _height(1), _color("#222222") { setFixedHeight(_height); }
};
