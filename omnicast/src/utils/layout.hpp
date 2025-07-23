#pragma once
#include "omni-icon.hpp"
#include "theme.hpp"
#include "ui/image/omnimg.hpp"
#include "ui/typography/typography.hpp"
#include <qboxlayout.h>
#include <qlabel.h>
#include <qmargins.h>
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
};

using LayoutItem = std::variant<std::shared_ptr<Stack>, LayoutWidget, LayoutStretch>;

enum AlignStrategy { None, JustifyBetween };

template <typename T>
concept ConvertibleToLayoutItem =
    std::is_same_v<std::decay_t<T>, Stack> || std::is_same_v<std::decay_t<T>, VStack> ||
    std::is_same_v<std::decay_t<T>, HStack> || std::is_convertible_v<std::decay_t<T>, QWidget *>;

template <typename T>
concept StackBase = std::is_convertible_v<std::decay_t<T>, Stack>;

struct IconBuilder {};

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

  Stack &add(QWidget *widget, int stretch = 0) {
    m_items.emplace_back(LayoutWidget{.widget = widget, .stretch = stretch});
    return *this;
  }

  Stack &addText(const QString &text, SemanticColor color = SemanticColor::TextPrimary,
                 TextSize size = TextSize::TextRegular) {
    auto typo = new TypographyWidget;

    typo->setText(text);
    typo->setColor(color);
    typo->setSize(size);
    add(typo);

    return *this;
  }

  Stack &addTitle(const QString &title, SemanticColor color = SemanticColor::TextPrimary) {
    return addText(title, color, TextSize::TextTitle);
  }

  Stack &addIcon(const OmniIconUrl &url, QSize size = {20, 20}) {
    auto icon = new Omnimg::ImageWidget;

    if (!size.isEmpty()) icon->setFixedSize(size);
    icon->setUrl(url);
    add(icon);

    return *this;
  }

  Stack &addParagraph(const QString &text, SemanticColor color = SemanticColor::TextPrimary,
                      TextSize size = TextSize::TextRegular) {
    auto typo = new TypographyWidget;

    typo->setText(text);
    typo->setWordWrap(true);
    typo->setColor(color);
    typo->setSize(size);
    add(typo);

    return *this;
  }

  Stack &divided(int n) {
    m_divided = n;
    return *this;
  }

  int divided() const { return m_divided; }

  /**
   * Build the layout and set it as the activate layout for `target`.
   */
  void imbue(QWidget *target) { target->setLayout(buildLayout()); }

  /**
   * For each item in the range, add a new widget
   */
  template <std::ranges::sized_range T> Stack &map(const T &container, const auto &fn) {
    m_items.reserve(m_items.size() + container.size());

    for (const auto &item : container) {
      add(fn(item));
    }

    return *this;
  }

  const std::vector<LayoutItem> &items() const { return m_items; }

  int spacing() const { return m_spacing; }
  Direction direction() const { return m_direction; }

  Stack &direction(Direction direction) {
    m_direction = direction;
    return *this;
  }

  Stack &addIf(bool value, const auto &fn) {
    if (value) add(fn());
    return *this;
  }

  Stack &align(AlignStrategy strategy) {
    m_align = strategy;
    return *this;
  }
  AlignStrategy align() const { return m_align; }

  QMargins margins() const { return m_margins; }
  Stack &margins(int margin) {
    m_margins = {margin, margin, margin, margin};
    return *this;
  }
  Stack &margins(int left, int top, int right, int bottom) {
    m_margins = {left, top, right, bottom};
    return *this;
  }
  Stack &spacing(int spacing) {
    m_spacing = spacing;
    return *this;
  }

  Stack &justifyBetween() {
    align(AlignStrategy::JustifyBetween);
    return *this;
  }

  Stack &addStretch(int stretch = 0) {
    m_items.emplace_back(LayoutStretch{stretch});
    return *this;
  }

  QBoxLayout *buildLayout() const;
  QWidget *buildWidget() const;
};

class VStack : public Stack {
public:
  VStack() { direction(Vertical); }
};

class HStack : public Stack {
public:
  HStack() { direction(Horizontal); }
};
