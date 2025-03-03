#pragma once
#include "ui/transform-result.hpp"
#include <qboxlayout.h>
#include <qlabel.h>
#include <qwidget.h>

struct CalculatorItem {
  QString expression;
  double result;
};

class CalculatorListItemWidget : public TransformResult {
  CalculatorItem item;
  TransformResult *_transform;

  void setupUi() {
    setBase(item.expression, "Expression");
    setResult(QString::number(item.result), "Answer");
  }

public:
  CalculatorListItemWidget(const CalculatorItem &item) : item(item) { setupUi(); }
};
