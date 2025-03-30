#pragma once
#include "app.hpp"
#include "extend/model-parser.hpp"
#include "extension/extension-list-component.hpp"
#include "view.hpp"
#include <qboxlayout.h>
#include <qjsonobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class ExtensionView : public View {
  Q_OBJECT

  QWidget *_container;
  QVBoxLayout *_layout;

  int _modelIndex = -1;
  AbstractExtensionRootComponent *_component;

  AbstractExtensionRootComponent *createRootComponent(const RenderModel &model, QWidget *parent = nullptr) {
    if (auto listModel = std::get_if<ListModel>(&model)) { return new ExtensionListComponent(app); }

    return nullptr;
  }

public:
  ExtensionView(AppWindow &app)
      : View(app), _container(new QWidget), _layout(new QVBoxLayout), _component(nullptr) {
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);
    _container->setLayout(_layout);
    widget = _container;
  }

  void onSearchChanged(const QString &s) override {
    if (_component) { _component->onSearchChanged(s); }
  }

  void render(const RenderModel &model) {
    if (model.index() != _modelIndex) {
      _layout->takeAt(0);

      if (_component) { _component->deleteLater(); }

      auto component = createRootComponent(model);

      if (!component) {
        qDebug() << "No component could be created for model!";
        _component = nullptr;
        return;
      }

      connect(component, &AbstractExtensionRootComponent::notifyEvent, this, &ExtensionView::notifyEvent);
      connect(component, &AbstractExtensionRootComponent::updateActionPannel, this,
              &ExtensionView::updateActionPannel);

      _component = component;
      _modelIndex = model.index();
      _layout->addWidget(_component);
    }

    _component->render(model);
  }

signals:
  void notifyEvent(const QString &handler, const std::vector<QJsonValue> &args) const;
  void updateActionPannel(const ActionPannelModel &model) const;
};
