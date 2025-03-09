#pragma once
#include "extend/action-model.hpp"
#include "ui/ellided-label.hpp"
#include "ui/keyboard-shortcut-indicator.hpp"
#include <qcoreevent.h>
#include <qevent.h>
#include "ui/button.hpp"
#include <qlabel.h>
#include <QHBoxLayout>
#include <qnamespace.h>
#include <qpainter.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class ShortcutButton : public Button {
  Q_OBJECT

  EllidedLabel *_label;
  KeyboardShortcutIndicatorWidget *_shortcut_indicator;

public:
  void hoverChanged(bool hovered) override;
  void setText(const QString &text);
  void setTextColor(const QColor &color);
  void setShortcut(const KeyboardShortcutModel &model);
  void resetColor();

  ShortcutButton();
};
