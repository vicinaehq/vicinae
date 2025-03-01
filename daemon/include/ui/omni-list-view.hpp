#pragma once
#include "ui/omni-list.hpp"
#include "view.hpp"
#include <qnamespace.h>

class OmniListView : public View {
protected:
  OmniList *list;

  using ItemList = std::vector<std::unique_ptr<OmniList::AbstractVirtualItem>>;

  ItemList itemList;

public:
  class AbstractActionnableItem : public AbstractDefaultListItem {
  public:
    virtual QList<AbstractAction *> generateActions() const { return {}; };
  };

private:
  virtual void selectionChanged(const OmniList::AbstractVirtualItem *next,
                                const OmniList::AbstractVirtualItem *previous) {
    if (!next) return;

    qDebug() << "selected id" << next->id();

    auto nextItem = static_cast<const AbstractActionnableItem *>(next);

    setSignalActions(nextItem->generateActions());
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
      QApplication::sendEvent(list, event);
      return true;
    }

    return View::inputFilter(event);
  }

  virtual void executeSearch(ItemList &list, const QString &s) {}

  void onSearchChanged(const QString &s) override {
    executeSearch(itemList, s);
    list->updateFromList(itemList, OmniList::SelectFirst);
    itemList.clear();
  }

public:
  OmniListView(AppWindow &app) : View(app), list(new OmniList) {
    widget = list;
    connect(list, &OmniList::selectionChanged, this, &OmniListView::selectionChanged);
    connect(list, &OmniList::itemActivated, this, &OmniListView::itemActivated);
  }
};
