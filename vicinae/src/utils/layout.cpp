#include "layout.hpp"
#include "common.hpp"
#include <qboxlayout.h>
#include <qlayout.h>
#include <qnamespace.h>
#include <qwidget.h>
#include <variant>
#include "ui/flow-layout/flow-layout.hpp"
#include "ui/image/image.hpp"
#include "ui/markdown/markdown-renderer.hpp"
#include "ui/typography/typography.hpp"
#include "../ui/image/url.hpp"

Stack &Stack::add(QWidget *widget, int stretch, Qt::Alignment align) {
  m_items.emplace_back(LayoutWidget{.widget = widget, .stretch = stretch, .align = align});
  return *this;
}

Stack &Stack::addSpacer(int space) {
  m_items.emplace_back(LayoutSpacer{.space = space});
  return *this;
}

Stack &Stack::addText(const QString &text, SemanticColor color, TextSize size, Qt::Alignment align) {
  auto typo = new TypographyWidget;

  typo->setText(text);
  typo->setColor(color);
  typo->setSize(size);
  add(typo, 0, align);

  return *this;
}
Stack &Stack::addTitle(const QString &title, SemanticColor color, Qt::Alignment align) {
  return addText(title, color, TextSize::TextTitle, align);
}

Stack &Stack::addIcon(const ImageURL &url, QSize size, Qt::Alignment align) {
  auto icon = new ImageWidget;

  if (!size.isEmpty()) icon->setFixedSize(size);
  icon->setUrl(url);
  add(icon, 0, align);

  return *this;
}

Stack &Stack::markdown(const QString &text) {
  auto markdown = new MarkdownRenderer;
  markdown->setMarkdown(text);
  add(markdown, 1);
  return *this;
}

Stack &Stack::addParagraph(const QString &text, SemanticColor color, TextSize size, Qt::Alignment align) {
  auto typo = new TypographyWidget;

  typo->setText(text);
  typo->setWordWrap(true);
  typo->setColor(color);
  typo->setSize(size);
  add(typo, 0, align);

  return *this;
}

Stack &Stack::justifyBetween() {
  align(AlignStrategy::JustifyBetween);
  return *this;
}

Stack &Stack::center() {
  align(AlignStrategy::Center);
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

Stack &Stack::marginsX(int x) {
  m_margins = {x, m_margins.top(), x, m_margins.bottom()};
  return *this;
}

Stack &Stack::marginsY(int y) {
  m_margins = {m_margins.left(), y, m_margins.right(), y};
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
    layout->setAlignment(Qt::AlignVCenter);
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

  if (align() == AlignStrategy::Center) layout->addStretch();

  for (const auto &item : items()) {
    if (auto stretch = std::get_if<LayoutStretch>(&item)) {
      layout->addStretch(stretch->stretch);
    } else if (auto spacer = std::get_if<LayoutSpacer>(&item)) {
      layout->addSpacing(spacer->space);
    } else if (auto widget = std::get_if<LayoutWidget>(&item)) {
      pushWidget(widget->widget, widget->stretch, widget->align);
    } else if (auto stackPtr = std::get_if<std::shared_ptr<Stack>>(&item)) {
      QWidget *child = new QWidget;

      child->setLayout(stackPtr->get()->buildLayout());
      pushWidget(child);
    }
  }

  if (align() == AlignStrategy::Center) layout->addStretch();

  return layout;
}

QWidget *Stack::buildWidget() const {
  auto widget = new QWidget;

  widget->setLayout(buildLayout());

  return widget;
}

QWidget *Flow::buildWidget() const {
  auto layout = new FlowLayout;

  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(m_spacing);

  for (const auto &item : m_items) {
    if (auto widget = std::get_if<QWidget *>(&item)) {
      layout->addWidget(*widget);
    } else if (auto stackPtr = std::get_if<std::shared_ptr<Stack>>(&item)) {
      QWidget *child = new QWidget;

      child->setLayout(stackPtr->get()->buildLayout());
      layout->addWidget(child);
    }
  }

  auto widget = new QWidget;

  widget->setLayout(layout);

  return widget;
}
