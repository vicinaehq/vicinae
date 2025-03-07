#include "ui/color-transform-widget.hpp"
#include "ui/color_circle.hpp"

void ColorTransformWidget::setColor(const QString &base, QColor color) {
  auto circle = new ColorCircle(base, {60, 60});

  circle->setStroke("#BBB", 3);
  setBase(base, "color");
  setResult(circle, color.name());
}

ColorTransformWidget::ColorTransformWidget() {}
