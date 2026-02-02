#pragma once
#include "ui/transform-result/transform-result.hpp"
#include <qboxlayout.h>
#include <qlabel.h>
#include <qwidget.h>

struct CalculatorItem {
  QString expression;
  QString result;
};

class CalculatorListItemWidget : public TransformResult {
  CalculatorItem item;
  TransformResult *_transform;

  void setupUi() {
    setBase(item.expression, "Expression");
    setResult(item.result, "Answer");
  }

public:
  CalculatorListItemWidget(const CalculatorItem &item) : item(item) { setupUi(); }
};
