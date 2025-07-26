#pragma once
#include <algorithm>
#include <qboxlayout.h>
#include "omni-icon.hpp"
#include "theme.hpp"
#include <qlabel.h>
#include <qmargins.h>
#include <qnamespace.h>
#include <qwidget.h>
#include <ranges>
#include <type_traits>
#include <variant>

/**
 * Utility classes to help build more complex layouts using QT widgets. This is used
 * all across the app.
 */

class Stack;
class VStack;
class HStack;

class LayoutItemBase {
  virtual QWidget *createWidget() const = 0;
  virtual ~LayoutItemBase() = default;
};

struct LayoutStretch {
  int stretch = 0;
};

struct LayoutWidget {
  QWidget *widget = nullptr;
  int stretch = 0;
  Qt::Alignment align;
};

using LayoutItem = std::variant<std::shared_ptr<Stack>, LayoutWidget, LayoutStretch>;

enum AlignStrategy { None, JustifyBetween };

template <typename T>
concept StackBase = std::is_convertible_v<std::decay_t<T>, Stack>;

class Stack {
public:
  enum Direction { Horizontal, Vertical };

  Direction m_direction;
  AlignStrategy m_align = AlignStrategy::None;
  int m_spacing = 0;
  QMargins m_margins;
  std::vector<LayoutItem> m_items;
  int m_divided = 0;

public:
  template <StackBase T> Stack &add(const T &stack) {
    m_items.emplace_back(std::make_shared<T>(stack));
    return *this;
  }

  template <std::ranges::sized_range T> Stack &map(const T &ct, const auto &fn) {
    m_items.reserve(m_items.size() + ct.size());
    std::ranges::for_each(ct, [&](auto &&item) { add(fn(item)); });

    return *this;
  }

  Stack &add(QWidget *widget, int stretch = 0, Qt::Alignment align = {});
  Stack &addText(const QString &text, SemanticColor color = SemanticColor::TextPrimary,
                 TextSize size = TextSize::TextRegular, Qt::Alignment align = {});
  Stack &addTitle(const QString &title, SemanticColor color = SemanticColor::TextPrimary,
                  Qt::Alignment align = {});
  Stack &addIcon(const OmniIconUrl &url, QSize size = {20, 20}, Qt::Alignment align = {});
  Stack &addParagraph(const QString &text, SemanticColor color = SemanticColor::TextPrimary,
                      TextSize size = TextSize::TextRegular, Qt::Alignment align = {});

  Stack &divided(int n);
  int divided() const;

  const std::vector<LayoutItem> &items() const { return m_items; }

  int spacing() const;
  Direction direction() const;
  Stack &direction(Direction direction);

  Stack addIf(bool value, const auto &fn) {
    if (value) add(fn());
    return *this;
  }

  Stack &align(AlignStrategy strategy);
  AlignStrategy align() const;
  QMargins margins() const;
  Stack &margins(int margin);
  Stack &margins(int left, int top, int right, int bottom);
  Stack &spacing(int spacing);

  /**
   * Implement alignment behaviour similar to 'justify-between' in CSS
   */
  Stack &justifyBetween();
  Stack &addStretch(int stretch = 0);
  QBoxLayout *buildLayout() const;
  QWidget *buildWidget() const;

  /**
   * Build the layout and set it as the active layout for `target`.
   * If a layout is already set for that widget, it will be replaced.
   */
  void imbue(QWidget *target);
};

class VStack : public Stack {
public:
  VStack() { direction(Vertical); }
};

class HStack : public Stack {
public:
  HStack() { direction(Horizontal); }
};
