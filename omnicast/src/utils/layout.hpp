#pragma once
#include <algorithm>
#include <qboxlayout.h>
#include "omni-icon.hpp"
#include "theme.hpp"
#include "ui/image/omnimg.hpp"
#include "ui/omni-button.hpp"
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
  Qt::Alignment m_align;

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

  Text &secondary() {
    color(SemanticColor::TextSecondary);
    return *this;
  }

  Text &text(const QString &text) {
    m_text = text;
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

    typo->setText(m_text);
    typo->setSize(m_size);
    typo->setAutoEllide(m_autoEllide);
    typo->setAlignment(m_align);

    if (m_color) typo->setColor(*m_color);

    return typo;
  }
};

class Button : public WidgetBuilder<OmniButtonWidget> {
  std::function<void(void)> m_onClick;
  QString m_text;

public:
  Button &onClick(const std::function<void(void)> &fn) {
    m_onClick = fn;
    return *this;
  }

  Button &text(const QString &text) {
    m_text = text;
    return *this;
  }

  OmniButtonWidget *create() const override {
    auto btn = new OmniButtonWidget;

    btn->setText(m_text);
    QObject::connect(btn, &OmniButtonWidget::clicked, [onClick = m_onClick]() {
      if (onClick) onClick();
    });

    return btn;
  }

  Button(const QString &text = "") : m_text(text) {}
};

class Icon : public WidgetBuilder<Omnimg::ImageWidget> {
  QString m_text;
  OmniIconUrl m_icon;

public:
  Icon(const OmniIconUrl &icon = {}) : m_icon(icon) {}

  virtual Omnimg::ImageWidget *create() const override {
    auto icon = new Omnimg::ImageWidget;

    icon->setUrl(m_icon);

    return icon;
  }
};

} // namespace UI

using LayoutItem = std::variant<std::shared_ptr<Stack>, LayoutWidget, LayoutStretch>;

enum AlignStrategy { None, JustifyBetween, Center };

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
