#pragma once
#include "extend/action-model.hpp"
#include "theme.hpp"
#include <qwidget.h>

class OmniPainter;

class KeyboardShortcutIndicatorWidget : public QWidget {
  KeyboardShortcutModel _shortcutModel;
  int _hspacing = 5;
  int _boxSize = 25;
  ColorLike _backgroundColor;

protected:
  void paintEvent(QPaintEvent *event) override;
  void drawKey(const QString &key, QRect rect, OmniPainter &painter);

public:
  QSize sizeHint() const override;
  void setShortcut(const KeyboardShortcutModel &model);
  void setBackgroundColor(ColorLike color);
  KeyboardShortcutModel shortcut() const { return _shortcutModel; }

  KeyboardShortcutIndicatorWidget(QWidget *parent = nullptr);
};
