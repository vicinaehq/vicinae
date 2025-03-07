#pragma once
#include "extend/action-model.hpp"
#include <qwidget.h>

class KeyboardShortcutIndicatorWidget : public QWidget {
  KeyboardShortcutModel _shortcutModel;
  int _hspacing = 5;
  int _boxSize = 25;
  QColor _backgroundColor;

protected:
  void paintEvent(QPaintEvent *event) override;
  void drawKey(const QString &key, QRect rect, QPainter &painter);

public:
  QSize sizeHint() const override;
  void setShortcut(const KeyboardShortcutModel &model);
  void setBackgroundColor(QColor color);

  KeyboardShortcutIndicatorWidget(QWidget *parent = nullptr);
};
