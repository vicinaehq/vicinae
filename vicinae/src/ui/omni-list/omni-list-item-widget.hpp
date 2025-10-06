#pragma once
#include <qtmetamacros.h>
#include <qwidget.h>

class OmniListItemWidget : public QWidget {
  Q_OBJECT

signals:
  void clicked();
  void doubleClicked();
  void rightClicked();

public:
  virtual void selectionChanged(bool selected) { Q_UNUSED(selected); }
  virtual void activated() {}

  /**
   * Explicitly clears conditional state on the widget such as hover status
   * This is needed for some list widgets relying on complex hovering strategies to
   * work around some QT limitation not properly firing (hover)leave events when widgets are moved
   * programatically.
   */
  virtual void clearTransientState() {}

  OmniListItemWidget(QWidget *parent = nullptr) : QWidget(parent) {}
};
