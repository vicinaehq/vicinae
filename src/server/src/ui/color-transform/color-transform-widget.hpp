#pragma once
#include "ui/transform-result/transform-result.hpp"

class ColorTransformWidget : public TransformResult {
public:
  void setColor(const QString &base, QColor color);

  ColorTransformWidget();
};
