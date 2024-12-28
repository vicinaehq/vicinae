#pragma once
#include "ui/list-view.hpp"
#include "view.hpp"

template <class T> class CustomList : public View {
  ListView *list;
  QHash<QString, T> actionMap;

  virtual ListModel search(const QString &s) = 0;
  virtual void action(const T &action) = 0;

  void onSearchChanged(const QString &s) override {
    qDebug() << "searchChanged" << s;
    actionMap.clear();
    ListModel model = search(s);

    if (!model.searchPlaceholderText.isEmpty()) {
      setSearchPlaceholderText(model.searchPlaceholderText);
    }

    list->dispatchModel(model);
  }

  void onActionActivated(ActionModel model) override {
    if (auto it = actionMap.find(model.onAction); it != actionMap.end()) {
      action(*it);
    }
  }

  void itemChanged(const QString &id) {
    qDebug() << "CustomList: itemChanged" << id;
  }

  void itemActivated(const QString &id) { emit activatePrimaryAction(); }

  void onAttach() override {}

protected:
  void addAction(const QString &id, const T &action) {
    actionMap.insert(id, action);
  }

public:
  CustomList(AppWindow &app) : View(app), list(new ListView) {
    addInputHandler(list);

    connect(list, &ListView::itemChanged, this, &CustomList::itemChanged);
    connect(list, &ListView::itemActivated, this, &CustomList::itemActivated);
    connect(list, &ListView::setActions, this, &CustomList::setActions);

    widget = list;
  }
};
