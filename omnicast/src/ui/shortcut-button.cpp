#include "ui/shortcut-button.hpp"
#include "theme.hpp"
#include "ui/typography.hpp"
#include <qcolor.h>

void ShortcutButton::hoverChanged(bool hovered) {
  auto &theme = ThemeService::instance().theme();

  _shortcut_indicator->setBackgroundColor(hovered ? theme.colors.statusBackgroundLighter
                                                  : theme.colors.statusBackgroundLighter);
  setBackgroundColor(hovered ? theme.colors.statusBackgroundHover : QColor::Invalid);
  update();
}

void ShortcutButton::resetColor() {
  auto &theme = ThemeService::instance().theme();

  _shortcut_indicator->setBackgroundColor(hovered() ? theme.colors.statusBackgroundLighter
                                                    : theme.colors.statusBackgroundLighter);
  setBackgroundColor(hovered() ? theme.colors.statusBackgroundHover : QColor::Invalid);
  setTextColor(theme.colors.text);
}

void ShortcutButton::setText(const QString &text) {
  _label->setText(text);
  updateGeometry();
}

void ShortcutButton::setTextColor(const QColor &color) {
  _label->setStyleSheet(QString("color: %1").arg(color.name()));
  updateGeometry();
}

void ShortcutButton::setShortcut(const KeyboardShortcutModel &model) {
  _shortcut_indicator->setShortcut(model);
  updateGeometry();
}

ShortcutButton::ShortcutButton()
    : _label(new TypographyWidget), _shortcut_indicator(new KeyboardShortcutIndicatorWidget) {
  setAttribute(Qt::WA_Hover);
  auto layout = new QHBoxLayout;

  layout->setAlignment(Qt::AlignVCenter);
  layout->addWidget(_label, 0, Qt::AlignLeft);
  layout->addWidget(_shortcut_indicator, 0, Qt::AlignRight);
  layout->setContentsMargins(8, 4, 8, 4);

  setLayout(layout);
}
