#include "layout.hpp"
#include "common.hpp"
#include <qboxlayout.h>
#include <qlayout.h>
#include <qnamespace.h>
#include <qwidget.h>
#include <variant>
#include "ui/image/omnimg.hpp"
#include "ui/typography/typography.hpp"
#include "omni-icon.hpp"

Stack &Stack::add(QWidget *widget, int stretch, Qt::Alignment align) {
  m_items.emplace_back(LayoutWidget{.widget = widget, .stretch = stretch, .align = align});
  return *this;
}

Stack &Stack::addText(const QString &text, SemanticColor color, TextSize size) {
  auto typo = new TypographyWidget;

  typo->setText(text);
  typo->setColor(color);
  typo->setSize(size);
  add(typo);

  return *this;
}
Stack &Stack::addTitle(const QString &title, SemanticColor color) {
  return addText(title, color, TextSize::TextTitle);
}

Stack &Stack::addIcon(const OmniIconUrl &url, QSize size) {
  auto icon = new Omnimg::ImageWidget;

  if (!size.isEmpty()) icon->setFixedSize(size);
  icon->setUrl(url);
  add(icon);

  return *this;
}

Stack &Stack::addParagraph(const QString &text, SemanticColor color, TextSize size) {
  auto typo = new TypographyWidget;

  typo->setText(text);
  typo->setWordWrap(true);
  typo->setColor(color);
  typo->setSize(size);
  add(typo);

  return *this;
}

Stack &Stack::justifyBetween() {
  align(AlignStrategy::JustifyBetween);
  return *this;
}

Stack &Stack::addStretch(int stretch) {
  m_items.emplace_back(LayoutStretch{stretch});
  return *this;
}

void Stack::imbue(QWidget *target) {
  if (auto layout = target->layout()) { QWidget().setLayout(target->layout()); }

  target->setLayout(buildLayout());
}

Stack &Stack::divided(int n) {
  m_divided = n;
  return *this;
}

int Stack::spacing() const { return m_spacing; }
Stack::Direction Stack::direction() const { return m_direction; }

Stack &Stack::direction(Direction direction) {
  m_direction = direction;
  return *this;
}

Stack &Stack::align(AlignStrategy strategy) {
  m_align = strategy;
  return *this;
}

AlignStrategy Stack::align() const { return m_align; }

QMargins Stack::margins() const { return m_margins; }

Stack &Stack::margins(int margin) {
  m_margins = {margin, margin, margin, margin};
  return *this;
}

Stack &Stack::margins(int left, int top, int right, int bottom) {
  m_margins = {left, top, right, bottom};
  return *this;
}
Stack &Stack::spacing(int spacing) {
  m_spacing = spacing;
  return *this;
}

int Stack::divided() const { return m_divided; }

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

  auto pushWidget = [&](QWidget *widget, int stretch = 0, Qt::Alignment alignment = {}) {
    if (widgetCount > 0) {
      if (align() == AlignStrategy::JustifyBetween) layout->addStretch();
      if (divided()) { layout->addWidget(makeDivider()); }
    }

    layout->addWidget(widget, stretch, alignment);

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
