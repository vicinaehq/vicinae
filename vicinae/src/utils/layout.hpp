#pragma once
#include <algorithm>
#include <qboxlayout.h>
#include "ui/image/url.hpp"
#include "theme.hpp"
#include "ui/image/image.hpp"
#include "ui/button/button.hpp"
#include "ui/typography/typography.hpp"
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
class Flow;

struct LayoutStretch {
  int stretch = 0;
};

struct LayoutSpacer {
  int space = 0;
};

struct LayoutWidget {
  QWidget *widget = nullptr;
  int stretch = 0;
  Qt::Alignment align;
};

template <typename T>
concept DerivedFromWidget = std::derived_from<T, QWidget>;

template <DerivedFromWidget T> struct WidgetBuilder {
  T **m_ref = nullptr;
  QSize m_size;
  std::optional<QColor> m_color;

  operator QWidget *() {
    if (auto widget = create()) {
      if (m_ref) *m_ref = widget;
      if (m_size.isValid()) widget->setFixedSize(m_size);

      return widget;
    }

    return nullptr;
  }

  /**
   * Assign this widget to the provided reference so that imperative
   * updates can be performed on it later on.
   * This is _not_ giving your ownership of the widget.
   */
  auto &ref(T *&slot) {
    m_ref = &slot;
    return *this;
  }

  auto &size(QSize size) {
    m_size = size;
    return *this;
  }

  virtual T *create() const = 0;
};

namespace UI {

class Text : public WidgetBuilder<TypographyWidget> {
  QString m_text;
  TextSize m_size = TextSize::TextRegular;
  std::optional<ColorLike> m_color;
  bool m_autoEllide = true;
  bool m_wordWrap = false;
  Qt::Alignment m_align;
  bool m_shrink = true;

public:
  Text(const QString &text) : m_text(text) {}

  Text &title() {
    size(TextSize::TextTitle);
    return *this;
  }

  Text &smaller() {
    size(TextSize::TextSmaller);
    return *this;
  }

  Text &autoEllide(bool value) {
    m_autoEllide = value;
    return *this;
  }

  Text &paragraph() {
    m_wordWrap = true;
    return *this;
  }

  Text &secondary() {
    color(SemanticColor::TextMuted);
    return *this;
  }

  Text &text(const QString &text) {
    m_text = text;
    return *this;
  }

  Text &fixed() {
    m_shrink = false;
    return *this;
  }

  Text &size(TextSize textSize) {
    m_size = textSize;
    return *this;
  }

  Text &align(Qt::Alignment align) {
    m_align = align;
    return *this;
  }

  Text &color(ColorLike color) {
    m_color = color;
    return *this;
  }

  virtual TypographyWidget *create() const override {
    auto typo = new TypographyWidget;

    if (!m_shrink) { typo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed); }

    typo->setText(m_text);
    typo->setSize(m_size);
    typo->setAutoEllide(m_autoEllide);
    typo->setAlignment(m_align);
    typo->setWordWrap(m_wordWrap);

    if (m_color) typo->setColor(*m_color);

    return typo;
  }
};

class Button : public WidgetBuilder<ButtonWidget> {
  std::function<void(void)> m_onClick;
  QString m_text;
  std::optional<ImageURL> m_leftIcon;
  ColorLike m_color = SemanticColor::Foreground;
  bool m_disabled = false;

public:
  Button &onClick(const std::function<void(void)> &fn) {
    m_onClick = fn;
    return *this;
  }

  Button &color(const ColorLike &color) {
    m_color = color;
    return *this;
  }

  Button &danger() { return color(SemanticColor::Red); }

  Button &disabled() {
    m_disabled = true;
    return *this;
  }

  Button &text(const QString &text) {
    m_text = text;
    return *this;
  }

  Button &leftIcon(const ImageURL &url) {
    m_leftIcon = url;
    return *this;
  }

  ButtonWidget *create() const override {
    auto btn = new ButtonWidget;

    btn->setText(m_text);
    btn->setTextColor(m_color);
    btn->setDisabled(m_disabled);

    if (m_leftIcon) { btn->setLeftIcon(*m_leftIcon, {16, 16}); }

    QObject::connect(btn, &ButtonWidget::clicked, [onClick = m_onClick]() {
      if (onClick) onClick();
    });

    return btn;
  }

  Button(const QString &text = "") : m_text(text) {}
};

class Icon : public WidgetBuilder<ImageWidget> {
  QString m_text;
  ImageURL m_icon;
  bool m_rounded = false;

public:
  Icon(const ImageURL &icon = {}) : m_icon(icon) {}

  Icon &rounded() {
    m_rounded = true;
    return *this;
  }

  virtual ImageWidget *create() const override {
    auto icon = new ImageWidget;
    ImageURL url(m_icon);

    if (m_rounded) { url.setMask(OmniPainter::CircleMask); }

    icon->setUrl(url);

    return icon;
  }
};

} // namespace UI

using LayoutItem = std::variant<std::shared_ptr<Stack>, LayoutWidget, LayoutStretch, LayoutSpacer>;

enum AlignStrategy { None, JustifyBetween, Center };

template <typename T>
concept StackBase = std::is_convertible_v<std::decay_t<T>, Stack>;

using FlowLayoutItem = std::variant<std::shared_ptr<Stack>, QWidget *>;

class Flow {
  std::vector<FlowLayoutItem> m_items;
  int m_spacing = 0;

public:
  Flow() {}

  Flow &spacing(int spacing) {
    m_spacing = spacing;
    return *this;
  }

  template <StackBase T> Flow &add(const T &stack) {
    m_items.emplace_back(std::make_shared<T>(stack));
    return *this;
  }

  Flow &add(QWidget *widget) {
    m_items.emplace_back(widget);
    return *this;
  }

  template <std::ranges::sized_range T> Flow &map(const T &ct, const auto &fn) {
    m_items.reserve(m_items.size() + ct.size());
    std::ranges::for_each(ct, [&](auto &&item) { add(fn(item)); });

    return *this;
  }

  QWidget *buildWidget() const;
};

class Stack {
public:
  enum Direction { Horizontal, Vertical };

  Direction m_direction;
  AlignStrategy m_align = AlignStrategy::None;
  int m_spacing = 0;
  bool m_wrapped = false;
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
  Stack &addSpacer(int space);
  Stack &addText(const QString &text, SemanticColor color = SemanticColor::Foreground,
                 TextSize size = TextSize::TextRegular, Qt::Alignment align = {});
  Stack &markdown(const QString &text);
  Stack &addTitle(const QString &title, SemanticColor color = SemanticColor::Foreground,
                  Qt::Alignment align = {});
  Stack &addIcon(const ImageURL &url, QSize size = {20, 20}, Qt::Alignment align = {});
  Stack &addParagraph(const QString &text, SemanticColor color = SemanticColor::Foreground,
                      TextSize size = TextSize::TextRegular, Qt::Alignment align = {});

  Stack &wrapped() {
    m_wrapped = true;
    return *this;
  }

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
  Stack &marginsX(int x);
  Stack &mx(int x) { return marginsX(x); }
  Stack &my(int y) { return marginsY(y); }
  Stack &marginsY(int y);
  Stack &margins(int margin);
  Stack &margins(int left, int top, int right, int bottom);
  Stack &spacing(int spacing);

  /**
   * Implement alignment behaviour similar to 'justify-between' in CSS
   */
  Stack &justifyBetween();
  Stack &center();
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
