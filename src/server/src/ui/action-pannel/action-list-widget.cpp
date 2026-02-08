#include "ui/action-pannel/action-list-widget.hpp"
#include "layout.hpp"
#include "theme.hpp"
#include "theme/colors.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/image/image.hpp"
#include "ui/typography/typography.hpp"
#include <qlogging.h>

ActionListWidget &ActionListWidget::setIconUrl(const std::optional<ImageURL> &url) {
  if (url) { m_icon->setUrl(url.value()); }
  m_icon->setVisible(url.has_value());
  return *this;
}

ActionListWidget &ActionListWidget::setShortcut(const Keyboard::Shortcut &shortcut) {
  m_shortcut->setShortcut(shortcut);
  m_shortcut->show();
  return *this;
}

ActionListWidget &ActionListWidget::clearShortcut() {
  m_shortcut->hide();
  return *this;
}

ActionListWidget &ActionListWidget::setTitle(const QString &title) {
  m_label->setText(title);
  return *this;
}

void ActionListWidget::selectionChanged(bool selected) {
  SelectableOmniListWidget::selectionChanged(selected);
}

void ActionListWidget::setAction(const AbstractAction *action) {
  m_label->setText(action->title());

  switch (action->style()) {
  case AbstractAction::Style::Normal:
    m_label->setColor(SemanticColor::Foreground);
    setIconUrl(action->icon());
    break;
  case AbstractAction::Style::Danger: {
    m_label->setColor(SemanticColor::Red);
    if (auto url = action->icon()) {
      url->setFill(SemanticColor::Red);
      setIconUrl(url);
    }
    break;
  }
  }

  if (auto shortcut = action->shortcut()) {
    m_shortcut->setShortcut(*shortcut);
    m_shortcut->show();
  } else {
    m_shortcut->hide();
  }
}

ActionListWidget::ActionListWidget()
    : m_icon(new ImageWidget), m_label(new TypographyWidget),
      m_shortcut(new KeyboardShortcutIndicatorWidget) {
  auto &theme = ThemeService::instance().theme();

  setSelectedColor(SemanticColor::ListItemSecondarySelectionBackground);
  setHoverColor(SemanticColor::ListItemSecondaryHoverBackground);
  setFixedHeight(45);
  m_shortcut->hide();
  m_icon->hide();
  m_icon->setFixedSize(22, 22);

  HStack()
      .margins(8)
      .spacing(10)
      .add(HStack().spacing(10).add(m_icon).add(m_label))
      .add(m_shortcut)
      .justifyBetween()
      .imbue(this);
}
