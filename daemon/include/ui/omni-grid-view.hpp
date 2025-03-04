#pragma once
#include "app.hpp"
#include "ui/omni-grid.hpp"
#include "view.hpp"

class OmniGridView : public View {
protected:
  OmniGrid *grid;

  class IActionnable {
  public:
    QList<AbstractAction *> generateActions() const { return {}; }
  };

  virtual void selectionChanged(const OmniList::AbstractVirtualItem *next,
                                const OmniList::AbstractVirtualItem *previous) {
    if (!next) { return; }

    if (auto nextItem = dynamic_cast<const IActionnable *>(next)) {
      setSignalActions(nextItem->generateActions());
    }
  }

  virtual void itemActivated(const OmniList::AbstractVirtualItem &item) {
    qDebug() << "activated";
    emit activatePrimaryAction();
  }

  bool inputFilter(QKeyEvent *event) override {
    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Return:
      QApplication::sendEvent(grid, event);
      return true;
    }

    return View::inputFilter(event);
  }

public:
  OmniGridView(AppWindow &app) : View(app), grid(new OmniGrid) { widget = grid; }
};
