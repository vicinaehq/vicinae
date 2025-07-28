#pragma once
#include "ui/button-base/button-base.hpp"

class TypographyWidget;
class KeyboardShortcutIndicatorWidget;
class KeyboardShortcutModel;

class ShortcutButton : public ButtonBase {
  Q_OBJECT

  TypographyWidget *_label;
  KeyboardShortcutIndicatorWidget *_shortcut_indicator;

public:
  void hoverChanged(bool hovered) override;
  void setText(const QString &text);
  void setTextColor(const QColor &color);
  void setShortcut(const std::optional<KeyboardShortcutModel> &model);
  KeyboardShortcutModel shortcut() const;
  void resetColor();

  ShortcutButton();
};
