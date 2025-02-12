#pragma once
#include "app.hpp"
#include "ui/list-view.hpp"
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

  void selectionChanged(const AbstractGridMember &member) {
    auto item = static_cast<const AbstractActionnableGridItem *>(&member);

    setSignalActions(item->createActions());
  }

public:
  GridView(AppWindow &app) : View(app), grid(new VirtualGridWidget) {
    connect(grid, &VirtualGridWidget::selectionChanged, this, &GridView::selectionChanged);
    widget = grid;
  }
};
