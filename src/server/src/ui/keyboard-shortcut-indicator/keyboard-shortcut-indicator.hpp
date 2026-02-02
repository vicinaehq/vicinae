#pragma once
#include "lib/keyboard/keyboard.hpp"
#include "theme.hpp"
#include <qnamespace.h>
#include <qwidget.h>

class OmniPainter;

class KeyboardShortcutIndicatorWidget : public QWidget {
public:
  KeyboardShortcutIndicatorWidget(QWidget *parent = nullptr);

  QSize sizeHint() const override;
  void setShortcut(const Keyboard::Shortcut &shortcut);
  void setBackgroundColor(ColorLike color);
  void setColor(ColorLike color);
  Keyboard::Shortcut shortcut() const { return m_shortcut; }

protected:
  void paintEvent(QPaintEvent *event) override;
  void drawKey(Qt::Key key, QRect rect, OmniPainter &painter);
  void drawModifier(Qt::KeyboardModifier, QRect rect, OmniPainter &painter);

  QSize sizeForKey(Qt::Key key) const;

private:
  Keyboard::Shortcut m_shortcut;
  int _hspacing = 5;
  int _boxSize = 25;
  ColorLike m_backgroundColor = SemanticColor::Background;
  ColorLike m_color = SemanticColor::Foreground;
};
