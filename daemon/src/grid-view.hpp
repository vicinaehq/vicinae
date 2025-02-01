#pragma once
#include "app.hpp"
#include "ui/virtual-grid.hpp"
#include "view.hpp"
#include <qnamespace.h>

class GridView : public View {
protected:
  VirtualGridWidget *grid;

  bool inputFilter(QKeyEvent *event) override {
    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
      QApplication::sendEvent(grid, event);
      return true;
    }

    return View::inputFilter(event);
  }

public:
  GridView(AppWindow &app) : View(app), grid(new VirtualGridWidget) { widget = grid; }
};
