#pragma once
#include "calculator.hpp"
#include "omnicast.hpp"
#include <qboxlayout.h>
#include <qlabel.h>
#include <qwidget.h>

struct CalculatorItem {
  QString expression;
  double result;
  std::optional<Unit> unit;
};

class CalculatorListItemWidget : public QWidget {
  CalculatorItem item;

  void setupUi() {
    auto exprLabel = new QLabel(item.expression);

    exprLabel->setProperty("class", "transform-left");

    auto answerLabel = new QLabel(QString::number(item.result));
    answerLabel->setProperty("class", "transform-left");

    auto left = new VStack(exprLabel, new Chip("Expression"));
    auto right =
        new VStack(answerLabel, new Chip(item.unit ? QString(item.unit->displayName.data()) : "Answer"));

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(new TransformResult(left, right));

    setLayout(layout);
  }

public:
  CalculatorListItemWidget(const CalculatorItem &item) : item(item) { setupUi(); }
};
