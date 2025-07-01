#pragma once
#include <qboxlayout.h>
#include <qlabel.h>
#include <qmargins.h>
#include <qwidget.h>
#include <type_traits>

class Stack;
class VStack;
class HStack;

class LayoutItemBase {
  virtual QWidget *createWidget() const = 0;
  virtual ~LayoutItemBase() = default;
};

using LayoutItem = std::variant<std::shared_ptr<Stack>, QWidget *>;

template <typename T>
concept ConvertibleToLayoutItem =
    std::is_same_v<std::decay_t<T>, Stack> || std::is_same_v<std::decay_t<T>, VStack> ||
    std::is_same_v<std::decay_t<T>, HStack> || std::is_convertible_v<std::decay_t<T>, QWidget *>;

template <typename T>
concept StackBase = std::is_convertible_v<std::decay_t<T>, Stack>;

class Stack {
public:
  enum Direction { Horizontal, Vertical };

  Direction m_direction;
  int m_spacing = 0;
  QMargins m_margins;
  std::vector<LayoutItem> m_items;

public:
  template <StackBase T> Stack &add(const T &stack) {
    m_items.emplace_back(std::make_unique<T>(stack));
    return *this;
  }

  Stack &add(QWidget *widget) {
    m_items.emplace_back(widget);
    return *this;
  }

  const std::vector<LayoutItem> &items() const { return m_items; }

  int spacing() const { return m_spacing; }
  Direction direction() const { return m_direction; }
  Stack &direction(Direction direction) {
    m_direction = direction;
    return *this;
  }
  QMargins margins() const { return m_margins; }
  Stack &margins(int margin) {
    m_margins = {margin, margin, margin, margin};
    return *this;
  }
  Stack &spacing(int spacing) {
    m_spacing = spacing;
    return *this;
  }

  QBoxLayout *buildLayout();
};

class VStack : public Stack {
public:
  VStack() { direction(Vertical); }
};

class HStack : public Stack {
public:
  HStack() { direction(Horizontal); }
};

struct StackVisitor {
  QWidget *operator()(QWidget *widget) { return widget; }
  QWidget *operator()(const std::shared_ptr<Stack> &stack) {
    auto container = new QWidget;
    QBoxLayout *layout = nullptr;

    switch (stack->direction()) {
    case Stack::Horizontal:
      layout = new QHBoxLayout;
      break;
    case Stack::Vertical:
      layout = new QVBoxLayout;
      break;
    }

    layout->setContentsMargins(stack->margins());
    layout->setSpacing(stack->spacing());

    for (const auto &item : stack->items()) {
      QWidget *child = std::visit(StackVisitor(), item);

      layout->addWidget(child);
    }

    layout->addStretch();
    container->setLayout(layout);

    return container;
  }

  StackVisitor() {}
};

/*
 *
 * Stack (
 * 	  QWidget,
 * 	  VStack() {}
 * )
 */
