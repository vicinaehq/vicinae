#pragma once
#include "ui/keyboard-shortcut-indicator.hpp"
#include <qevent.h>
#include <qwidget.h>

class AbsoluteStatusBar : public QWidget {
  KeyboardShortcutIndicatorWidget *_currentActionButton;
  KeyboardShortcutIndicatorWidget *_actionButton;

protected:
  void resizeEvent(QResizeEvent *event) override {
    auto drawableSize = rect().marginsAdded(contentsMargins());

    _actionButton->setFixedSize({_actionButton->sizeHint().width(), drawableSize.height()});
    _actionButton->move(width() - _actionButton->sizeHint().width(), 5);

    QWidget::resizeEvent(event);
  }

public:
  AbsoluteStatusBar()
      : _currentActionButton(new KeyboardShortcutIndicatorWidget(this)),
        _actionButton(new KeyboardShortcutIndicatorWidget(this)) {
    _actionButton->setShortcut({.key = "B", .modifiers = {"ctrl"}});
  }
};
