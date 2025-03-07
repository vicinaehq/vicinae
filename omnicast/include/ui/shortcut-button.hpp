#pragma once
#include "extend/action-model.hpp"
#include "ui/ellided-label.hpp"
#include "ui/keyboard-shortcut-indicator.hpp"
#include <qcoreevent.h>
#include <qevent.h>
#include <qlabel.h>
#include <QHBoxLayout>
#include <qnamespace.h>
#include <qpainter.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class ShortcutButton : public QWidget {
  Q_OBJECT

  EllidedLabel *_label;
  KeyboardShortcutIndicatorWidget *_shortcut_indicator;
  bool _hovered;

protected:
  bool event(QEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void paintEvent(QPaintEvent *event) override;

public:
  void setHovered(bool hovered);
  void setText(const QString &text);
  void setTextColor(const QColor &color);
  void setShortcut(const KeyboardShortcutModel &model);

  ShortcutButton();

signals:
  void clicked() const;
};
