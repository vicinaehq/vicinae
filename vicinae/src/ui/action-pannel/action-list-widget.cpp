#include "ui/action-pannel/action-list-widget.hpp"
#include "theme.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/image/image.hpp"
#include "ui/typography/typography.hpp"
#include <qlogging.h>

ActionListWidget &ActionListWidget::setIconUrl(const ImageURL &url) {
  m_icon->setUrl(url);
  return *this;
}

ActionListWidget &ActionListWidget::setShortcut(const KeyboardShortcutModel &shortcut) {
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
  auto &theme = ThemeService::instance().theme();

  if (selected) {
    m_shortcut->setBackgroundColor(theme.resolveTint(SemanticColor::TertiaryBackground));
    if (m_actionStyle == AbstractAction::Style::Danger) {
      m_label->setColor(SemanticColor::Red);
    } else {
      m_label->setColor(SemanticColor::TextOnAccent);
    }
  } else {
    m_shortcut->setBackgroundColor(theme.resolveTint(SemanticColor::TertiaryBackground));
    if (m_actionStyle == AbstractAction::Style::Danger) {
      m_label->setColor(SemanticColor::Red);
    } else {
      m_label->setColor(SemanticColor::TextPrimary);
    }
  }
}

void ActionListWidget::setAction(const AbstractAction *action) {
  m_label->setText(action->title());
  m_actionStyle = action->style();

  switch (action->style()) {
  case AbstractAction::Style::Normal: {
    m_label->setColor(SemanticColor::TextPrimary);
    auto url = action->icon();
    url.setFill(SemanticColor::TextPrimary);
    m_icon->setUrl(url);
    break;
  }
  case AbstractAction::Style::Danger: {
    m_label->setColor(SemanticColor::Red);
    auto url = action->icon();

    url.setFill(SemanticColor::Red);
    m_icon->setUrl(url);
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
  auto layout = new QHBoxLayout;

  m_shortcut->hide();
  m_shortcut->setBackgroundColor(theme.resolveTint(SemanticColor::TertiaryBackground));

  m_icon->setFixedSize(22, 22);
  layout->setAlignment(Qt::AlignVCenter);
  layout->setSpacing(10);
  layout->addWidget(m_icon);
  layout->addWidget(m_label);
  layout->addWidget(m_shortcut, 0, Qt::AlignRight);
  layout->setContentsMargins(8, 8, 8, 8);

  setLayout(layout);
}
