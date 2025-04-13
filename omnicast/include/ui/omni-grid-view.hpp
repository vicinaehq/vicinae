#pragma once
#include "app.hpp"
#include "ui/action-pannel/action-item.hpp"
#include "ui/omni-grid.hpp"
#include "view.hpp"
#include <qboxlayout.h>

class OmniGridView : public View {
  QString baseNavigationTitle;

protected:
  OmniGrid *grid;

public:
  class IActionnable {
  public:
    virtual QList<AbstractAction *> generateActions() const { return {}; }
    virtual std::vector<ActionItem> generateActionPannel() const { return {}; }
    virtual QString navigationTitle() const { return {}; }
  };

  virtual void selectionChanged(const OmniList::AbstractVirtualItem *next,
                                const OmniList::AbstractVirtualItem *previous) {
    if (!next) {
      setNavigationTitle(baseNavigationTitle);
      setSignalActions({});
      return;
    }

    if (auto nextItem = dynamic_cast<const IActionnable *>(next)) {
      if (auto title = nextItem->navigationTitle(); !title.isEmpty()) {
        setNavigationTitle(baseNavigationTitle + " - " + title);
      } else {
        setNavigationTitle(baseNavigationTitle);
      }

      auto actionPannel = nextItem->generateActionPannel();

      if (!actionPannel.empty()) {
        setActionPannel(std::move(actionPannel));
      } else {

        auto actions = nextItem->generateActions();

        if (!actions.isEmpty()) { actions.at(0)->setShortcut({.key = "return"}); }
        setSignalActions(actions);
      }

    } else {
      qDebug() << "fuck dynamic cast";
    }
  }

  virtual void itemActivated(const OmniList::AbstractVirtualItem &item) {
    qDebug() << "activated";
    emit activatePrimaryAction();
  }

  void onMount() override {
    qDebug() << "current navigation title" << navigationTitle();
    baseNavigationTitle = navigationTitle();
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
  OmniGridView(AppWindow &app) : View(app), grid(new OmniGrid) {
    connect(grid, &OmniGrid::selectionChanged, this, &OmniGridView::selectionChanged);
    connect(grid, &OmniGrid::itemActivated, this, &OmniGridView::itemActivated);

    auto layout = new QVBoxLayout;

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(grid);
    setLayout(layout);
  }
};
