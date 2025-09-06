#include "color-transform-widget.hpp"
#include "ui/color-circle/color_circle.hpp"

void ColorTransformWidget::setColor(const QString &base, QColor color) {
  auto circle = new ColorCircle({60, 60});

  circle->setColor(color);
  circle->setStroke(SemanticColor::Border, 3);
  setBase(base, "color");
  setResult(circle, color.name());
}

ColorTransformWidget::ColorTransformWidget() {}
