#include "ui/action-pannel/action-list-widget.hpp"

ActionListWidget &ActionListWidget::setIconUrl(const OmniIconUrl &url) {
  _icon->setUrl(url);
  return *this;
}

ActionListWidget &ActionListWidget::setShortcut(const KeyboardShortcutModel &shortcut) {
  _shortcut->setShortcut(shortcut);
  _shortcut->show();
  return *this;
}

ActionListWidget &ActionListWidget::clearShortcut() {
  _shortcut->hide();
  return *this;
}

ActionListWidget &ActionListWidget::setTitle(const QString &title) {
  _label->setText(title);
  return *this;
}

void ActionListWidget::selectionChanged(bool selected) {
  SelectableOmniListWidget::selectionChanged(selected);
  auto &theme = ThemeService::instance().theme();

  if (selected) {
    _shortcut->setBackgroundColor(theme.colors.statusBackground);
  } else {
    _shortcut->setBackgroundColor(theme.colors.statusBackground);
  }
}

ActionListWidget::ActionListWidget()
    : _icon(new OmniIcon), _label(new QLabel), _shortcut(new KeyboardShortcutIndicatorWidget) {
  auto &theme = ThemeService::instance().theme();
  auto layout = new QHBoxLayout;

  _shortcut->hide();
  _shortcut->setBackgroundColor(theme.colors.statusBackground);

  _icon->setFixedSize(22, 22);
  layout->setAlignment(Qt::AlignVCenter);
  layout->setSpacing(10);
  layout->addWidget(_icon);
  layout->addWidget(_label);
  layout->addWidget(_shortcut, 0, Qt::AlignRight);
  layout->setContentsMargins(8, 8, 8, 8);

  setLayout(layout);
}
