#include "layout.hpp"
#include "common.hpp"
#include <qboxlayout.h>
#include <qlayout.h>
#include <qwidget.h>
#include <variant>

QBoxLayout *Stack::buildLayout() const {
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

  auto makeDivider = [&]() -> QWidget * {
    switch (direction()) {
    case Stack::Horizontal:
      return new VDivider;
    case Stack::Vertical:
      return new HDivider;
    }
    return new QWidget;
  };

  size_t widgetCount = 0;

  auto pushWidget = [&](QWidget *widget, int stretch = 0) {
    if (widgetCount > 0) {
      if (align() == AlignStrategy::JustifyBetween) layout->addStretch();
      if (divided()) { layout->addWidget(makeDivider()); }
    }

    layout->addWidget(widget, stretch);

    ++widgetCount;
  };

  for (const auto &item : items()) {
    if (auto stretch = std::get_if<LayoutStretch>(&item)) {
      layout->addStretch(stretch->stretch);
    } else if (auto widget = std::get_if<LayoutWidget>(&item)) {
      pushWidget(widget->widget, widget->stretch);
    } else if (auto stackPtr = std::get_if<std::shared_ptr<Stack>>(&item)) {
      QWidget *child = new QWidget;

      child->setLayout(stackPtr->get()->buildLayout());
      pushWidget(child);
    }
  }

  return layout;
}

QWidget *Stack::buildWidget() const {
  auto widget = new QWidget;

  widget->setLayout(buildLayout());

  return widget;
}
