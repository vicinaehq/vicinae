#include "shortcut-button.hpp"
#include "theme.hpp"
#include "theme/colors.hpp"
#include "ui/button-base/button-base.hpp"
#include "ui/keyboard-shortcut-indicator/keyboard-shortcut-indicator.hpp"
#include "ui/typography/typography.hpp"
#include <qboxlayout.h>
#include <qcolor.h>
#include <qnamespace.h>

void ShortcutButton::hoverChanged(bool hovered) {
  //_shortcut_indicator->setBackgroundColor(SemanticColor::ListItemSelectionBackground);
  setBackgroundColor(hovered ? ColorLike(SemanticColor::ButtonPrimaryHoverBackground) : Qt::transparent);

  update();
}

void ShortcutButton::resetColor() {
  setBackgroundColor(Qt::transparent);
  setHoverBackgroundColor(SemanticColor::ButtonPrimaryHoverBackground);
  _shortcut_indicator->setColor(SemanticColor::ButtonPrimaryForeground);
  //_shortcut_indicator->setBackgroundColor(SemanticColor::ListItemSelectionBackground);
  update();
}

void ShortcutButton::setText(const QString &text) {
  _label->setText(text);
  updateGeometry();
}

void ShortcutButton::setTextColor(const QColor &color) {
  _label->setStyleSheet(QString("color: %1").arg(color.name()));
  updateGeometry();
}

void ShortcutButton::setShortcut(const std::optional<Keyboard::Shortcut> &model) {
  _shortcut_indicator->setVisible(model.has_value());

  if (model) { _shortcut_indicator->setShortcut(*model); }

  updateGeometry();
}

Keyboard::Shortcut ShortcutButton::shortcut() const { return _shortcut_indicator->shortcut(); }

ShortcutButton::ShortcutButton()
    : _label(new TypographyWidget), _shortcut_indicator(new KeyboardShortcutIndicatorWidget) {
  auto layout = new QHBoxLayout;

  setFocusPolicy(Qt::NoFocus);
  setColor(ButtonColor::Transparent);
  _shortcut_indicator->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  layout->setAlignment(Qt::AlignVCenter);
  layout->addWidget(_label, 0, Qt::AlignLeft);
  layout->addWidget(_shortcut_indicator, 0, Qt::AlignRight);
  layout->setContentsMargins(8, 4, 8, 4);
  resetColor();

  connect(&ThemeService::instance(), &ThemeService::themeChanged, this, [this]() { resetColor(); });
  connect(this, &ButtonBase::hoverChanged, this, &ShortcutButton::hoverChanged);

  setLayout(layout);
}
