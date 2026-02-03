#pragma once
#include "ui/button-base/button-base.hpp"
#include "lib/keyboard/keyboard.hpp"

class TypographyWidget;
class KeyboardShortcutIndicatorWidget;

class ShortcutButton : public ButtonBase {
public:
  ShortcutButton();

  void hoverChanged(bool hovered);
  void setText(const QString &text);
  void setTextColor(const QColor &color);
  void setShortcut(const std::optional<Keyboard::Shortcut> &model);
  Keyboard::Shortcut shortcut() const;
  void resetColor();

private:
  TypographyWidget *_label;
  KeyboardShortcutIndicatorWidget *_shortcut_indicator;
};
