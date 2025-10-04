#pragma once
#include "extend/action-model.hpp"
#include "theme.hpp"
#include <qwidget.h>

class OmniPainter;

class KeyboardShortcutIndicatorWidget : public QWidget {
public:
  KeyboardShortcutIndicatorWidget(QWidget *parent = nullptr);

  QSize sizeHint() const override;
  void setShortcut(const KeyboardShortcutModel &model);
  void setBackgroundColor(ColorLike color);
  void setColor(ColorLike color);
  KeyboardShortcutModel shortcut() const { return _shortcutModel; }

protected:
  void paintEvent(QPaintEvent *event) override;
  void drawKey(const QString &key, QRect rect, OmniPainter &painter);

private:
  KeyboardShortcutModel _shortcutModel;
  int _hspacing = 5;
  int _boxSize = 25;
  ColorLike m_backgroundColor = SemanticColor::MainBackground;
  ColorLike m_color = SemanticColor::TextPrimary;
};
