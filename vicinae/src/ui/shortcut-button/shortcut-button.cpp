#include "shortcut-button.hpp"
#include "theme.hpp"
#include "ui/keyboard-shortcut-indicator/keyboard-shortcut-indicator.hpp"
#include "ui/typography/typography.hpp"
#include <qboxlayout.h>
#include <qcolor.h>
#include <qnamespace.h>

void ShortcutButton::hoverChanged(bool hovered) {
  auto &theme = ThemeService::instance().theme();

  _shortcut_indicator->setBackgroundColor(hovered ? SemanticColor::SecondaryBackground
                                                  : SemanticColor::TertiaryBackground);
  setBackgroundColor(hovered ? theme.resolveTint(SemanticColor::ButtonSecondaryHover)
                             : QColor(Qt::transparent));
  update();
}

void ShortcutButton::resetColor() {
  auto &theme = ThemeService::instance().theme();

  _shortcut_indicator->setBackgroundColor(hovered() ? SemanticColor::SecondaryBackground
                                                    : SemanticColor::TertiaryBackground);
  setBackgroundColor(hovered() ? theme.resolveTint(SemanticColor::ButtonSecondaryHover)
                               : QColor(Qt::transparent));
  setTextColor(theme.resolveTint(SemanticColor::TextPrimary));
}

void ShortcutButton::setText(const QString &text) {
  _label->setText(text);
  updateGeometry();
}

void ShortcutButton::setTextColor(const QColor &color) {
  _label->setColor(color);
  updateGeometry();
}

void ShortcutButton::setShortcut(const std::optional<KeyboardShortcutModel> &model) {
  _shortcut_indicator->setVisible(model.has_value());

  if (model) { _shortcut_indicator->setShortcut(*model); }

  updateGeometry();
}

KeyboardShortcutModel ShortcutButton::shortcut() const { return _shortcut_indicator->shortcut(); }

ShortcutButton::ShortcutButton()
    : _label(new TypographyWidget), _shortcut_indicator(new KeyboardShortcutIndicatorWidget) {
  auto layout = new QHBoxLayout;

  layout->setAlignment(Qt::AlignVCenter);
  layout->addWidget(_label, 0, Qt::AlignLeft);
  layout->addWidget(_shortcut_indicator, 0, Qt::AlignRight);
  layout->setContentsMargins(8, 4, 8, 4);
  resetColor();

  connect(&ThemeService::instance(), &ThemeService::themeChanged, this, [this]() { resetColor(); });

  setLayout(layout);
}
