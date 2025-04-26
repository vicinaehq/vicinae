#pragma once
#include "app.hpp"
#include "extend/form-model.hpp"
#include "extend/grid-model.hpp"
#include "extend/model-parser.hpp"
#include "extend/model.hpp"
#include "extension/extension-command.hpp"
#include "extension/extension-grid-component.hpp"
#include "extension/extension-list-component.hpp"
#include "extension/extension-form-component.hpp"
#include "view.hpp"
#include <qboxlayout.h>
#include <qevent.h>
#include <qjsonobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class ExtensionView : public View {
  Q_OBJECT

  const ExtensionCommand &_command;
  QVBoxLayout *_layout;

  int _modelIndex = -1;
  AbstractExtensionRootComponent *_component;

  void resizeEvent(QResizeEvent *event) override {
    QWidget::resizeEvent(event);
    if (_component) _component->setFixedSize(event->size());
  }

  AbstractExtensionRootComponent *createRootComponent(const RenderModel &model, QWidget *parent = nullptr) {
    if (auto listModel = std::get_if<ListModel>(&model)) {
      showInput();
      return new ExtensionListComponent(app);
    } else if (auto gridModel = std::get_if<GridModel>(&model)) {
      showInput();
      return new ExtensionGridComponent(app);
    } else if (auto formModel = std::get_if<FormModel>(&model)) {
      hideInput();
      return new ExtensionFormComponent(app);
    }

    return nullptr;
  }

  bool inputFilter(QKeyEvent *event) override {
    if (_component) { return _component->inputFilter(event); }

    return false;
  }

public:
  ExtensionView(AppWindow &app, const ExtensionCommand &command)
      : View(app), _command(command), _layout(new QVBoxLayout), _component(nullptr) {}

  const ExtensionCommand &command() const { return _command; }

  void onSearchChanged(const QString &s) override {
    if (_component) { _component->onSearchChanged(s); }
  }

  bool submitForm(const EventHandler &callback) {
    if (_component && _modelIndex == RenderModel(FormModel()).index()) {
      qDebug() << "forwarding callback";
      static_cast<ExtensionFormComponent *>(_component)->handleSubmit(callback);
      return true;
    }

    return false;
  }

  void render(const RenderModel &model) {
    if (model.index() != _modelIndex) {
      qDebug() << "CREATING NEW COMPONENT";
      _layout->takeAt(0);

      if (_component) { _component->deleteLater(); }

      auto component = createRootComponent(model);

      component->setParent(this);
      component->setFixedSize(size());
      component->show();

      qDebug() << "set fixed size" << size();

      if (!component) {
        qDebug() << "No component could be created for model!";
        _component = nullptr;
        return;
      }

      connect(component, &AbstractExtensionRootComponent::notifyEvent, this, &ExtensionView::notifyEvent);
      connect(component, &AbstractExtensionRootComponent::updateActionPannel, this,
              &ExtensionView::updateActionPannel);

      _component = component;
      _component->onMount();
      _modelIndex = model.index();
      _layout->addWidget(_component);
    }

    QTimer::singleShot(0, [this, model = std::move(model)]() { _component->render(model); });
  }

signals:
  void notifyEvent(const QString &handler, const QJsonArray &args) const;
  void updateActionPannel(const ActionPannelModel &model) const;
};
