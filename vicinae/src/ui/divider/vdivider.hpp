#include "theme/colors.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include <qframe.h>

class VDivider : public QFrame {
  QColor _color;
  size_t _width;

  void paintEvent(QPaintEvent *event) override {
    OmniPainter painter(this);
    auto margins = contentsMargins();

    painter.setThemeBrush(SemanticColor::BackgroundBorder);
    painter.setThemePen(SemanticColor::BackgroundBorder);
    painter.drawRect(0, margins.top(), _width, height() - margins.top() - margins.bottom());
  }

public:
  void setWidth(int width) {
    _width = width;
    setFixedWidth(width);
    updateGeometry();
  }

  void setColor(QColor color) {
    _color = color;
    update();
  }

  VDivider(QWidget *parent = nullptr) : QFrame(parent), _width(1), _color("#222222") {
    setFixedWidth(_width);
  }
};
