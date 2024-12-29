#pragma once
#include "extend/action-model.hpp"
#include "extend/list-model.hpp"
#include "extension.hpp"
#include "ui/list-view.hpp"
#include "view.hpp"
#include <QListWidget>
#include <QStackedLayout>
#include <QTextEdit>
#include <qboxlayout.h>
#include <qdir.h>
#include <qlabel.h>
#include <qlistwidget.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qobject.h>
#include <qsizepolicy.h>
#include <qstackedlayout.h>
#include <qtextedit.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class ExtensionList : public ExtensionComponent {
  Q_OBJECT

  View &parent;
  QVBoxLayout *layout;
  ListView *list;

private slots:
public:
  ListModel model;

  ExtensionList(const ListModel &model, View &parent)
      : parent(parent), layout(new QVBoxLayout), list(new ListView) {
    parent.forwardInputEvents(list->listWidget());

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(list);

    // connect(list, &ListView::setActions, &parent, &View::setActions);
    connect(list, &ListView::itemChanged, this, &ExtensionList::itemChanged);

    setLayout(layout);
    dispatchModel(model);
  }

  void dispatchModel(const ListModel &model) {
    parent.setSearchPlaceholderText(model.searchPlaceholderText);
    list->dispatchModel(model);
    this->model = model;
    qDebug() << "dispatching model update";
  }

  void onActionActivated(ActionModel model) {
    qDebug() << "activated" << model.title;
  }

  void itemChanged(const QString &id) { qDebug() << "item with id " << id; }

  void onSearchTextChanged(const QString &s) {
    qDebug() << "onSearchTextChange" << model.onSearchTextChange;
    if (!model.onSearchTextChange.isEmpty()) {
      QJsonObject payload;
      auto args = QJsonArray();

      args.push_back(s);
      payload["args"] = args;

      emit extensionEvent(model.onSearchTextChange, payload);
    }

    if (!model.onSearchTextChange.isEmpty())
      return;

    list->filterItems(s);
  }
};
