#pragma once
#include "app.hpp"
#include "components/list-component.hpp"

struct ListItemViewModel {
  QString id;
  QString title;
  QString subtitle;
};

struct ListViewModel {
  QString navigationTitle;
  bool isLoading;
  bool isFiltering;
  bool isShowingDetail;
  QList<ListItemViewModel> items;
};

class ListExtensionComponent : public ListComponent {
public:
  ListViewModel model;
  ListExtensionComponent(AppWindow *app) : ListComponent(app, {}) {}

  QList<ListDisplayable> onSearchTextChanged(const QString &s) override {
    QList<ListDisplayable> items;

    for (const auto &item : model.items) {
      items.push_back(BasicListItem{item.id, item.title, "firefox"});
    }

    return items;
  }

  void update(ListViewModel model) { this->model = model; }
};
