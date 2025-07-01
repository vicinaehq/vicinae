#include "layout.hpp"
#include <qboxlayout.h>

QBoxLayout *Stack::buildLayout() {
  QBoxLayout *layout = nullptr;

  switch (direction()) {
  case Stack::Horizontal:
    layout = new QHBoxLayout;
    break;
  case Stack::Vertical:
    layout = new QVBoxLayout;
    break;
  }

  layout->setContentsMargins(margins());
  layout->setSpacing(spacing());

  for (const auto &item : items()) {
    QWidget *child = std::visit(StackVisitor(), item);

    layout->addWidget(child);
  }

  layout->addStretch();

  return layout;
}
