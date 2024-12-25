#pragma once
#include "extend/list-view.hpp"
#include "extension.hpp"
#include "view.hpp"
#include <qboxlayout.h>
#include <qjsonobject.h>
#include <qtmetamacros.h>

class ExtensionView : public View {
  Q_OBJECT

  ExtensionComponent *component;
  QString componentType;
  QVBoxLayout *layout;

  void setRootComponent(const QString &type, ExtensionComponent *ui) {
    if (layout->count() > 0) {
      layout->replaceWidget(layout->itemAt(0)->widget(), ui);
    } else {
      layout->addWidget(ui);
    }

    connect(ui, &ExtensionComponent::extensionEvent, this,
            [this](const QString &action, const QJsonObject &payload) {
              emit extensionEvent(action, payload);
            });

    component = ui;
    componentType = type;
  }

public:
signals:
  void extensionEvent(const QString &action, const QJsonObject &payload);

public slots:
  void render(RenderModel model) {
    if (auto listModel = std::get_if<ListModel>(&model)) {
      if (componentType != "list") {
        setRootComponent("list", new ExtensionList(*listModel, *this));
      } else {
        static_cast<ExtensionList *>(component)->dispatchModel(*listModel);
      }
    }
  }

  void onSearchChanged(const QString &s) override {
    qDebug() << "on search changed";
    if (componentType == "list") {
      static_cast<ExtensionList *>(component)->onSearchTextChanged(s);
    }
  }

  void onActionActivated(ActionModel model) override {
    if (!model.onAction.isEmpty()) {
      emit extensionEvent(model.onAction, {});
    }

    if (componentType == "list") {
      static_cast<ExtensionList *>(component)->onActionActivated(model);
    }
  }

public:
  ExtensionView(AppWindow &app) : View(app) {
    widget = new QWidget();
    layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    widget->setLayout(layout);
  }

  ~ExtensionView() { qDebug() << "Destroy extension view"; }
};
