#pragma once
#include "action-panel/action-panel.hpp"
#include "app.hpp"
#include "base-view.hpp"
#include "extend/action-model.hpp"
#include "extend/model-parser.hpp"
#include "extend/model.hpp"
#include "extension/extension-command.hpp"
#include "extension/extension-form-component.hpp"
#include "omni-icon.hpp"
#include "ui/action-pannel/action.hpp"
#include <qboxlayout.h>
#include <qevent.h>
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qlogging.h>
#include <qstackedlayout.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class ExtensionActionV2 : public AbstractAction {
  ActionModel m_model;

  void execute() override {}

public:
  const ActionModel &model() const { return m_model; }
  ExtensionActionV2(const ActionModel &model)
      : AbstractAction(model.title, OmniIconUrl(model.icon.value_or(std::monostate{}))), m_model(model) {}
};

class ExtensionActionPanelView : public ActionPanelStaticListView {
  Q_OBJECT
  ActionPannelModel m_model;
  std::vector<KeyboardShortcutModel> m_defaultActionShortcuts;

  void render() {
    int idx = 0;

    clear();
    setTitle(m_model.title);

    for (const auto &item : m_model.children) {
      if (auto section = std::get_if<ActionPannelSectionModel>(&item)) {
        addSection(section->title);

        for (const auto &model : section->actions) {
          auto action = new ExtensionActionV2(model);

          if (idx == 0) { action->setPrimary(true); }

          if (idx < m_defaultActionShortcuts.size()) {
            action->setShortcut(m_defaultActionShortcuts.at(idx));
            ++idx;
          }

          addAction(action);
        }
      }

      if (auto actionModel = std::get_if<ActionModel>(&item)) {
        auto action = new ExtensionActionV2(*actionModel);

        if (idx == 0) { action->setPrimary(true); }

        if (idx < m_defaultActionShortcuts.size()) {
          action->setShortcut(m_defaultActionShortcuts.at(idx));
          ++idx;
        }

        addAction(action);
      }
    }

    qDebug() << "Rendered";
  }

public:
  void setDefaultActionShortcuts(const std::vector<KeyboardShortcutModel> &models) {
    m_defaultActionShortcuts = models;
  }
  void setModel(const ActionPannelModel &model) {
    m_model = model;
    render();
  }

  ExtensionActionPanelView() {}

signals:
  void textChanged(const QString &text) const;
};

class ExtensionSimpleView : public SimpleView {
  Q_OBJECT

  std::vector<ExtensionActionPanelView> m_actionPanelViewStack;
  std::vector<KeyboardShortcutModel> m_defaultActionShortcuts;

public:
  virtual void render(const RenderModel &model) {}

  void setDefaultActionShortcuts(const std::vector<KeyboardShortcutModel> &models) {
    m_defaultActionShortcuts = models;
  }

  void onActionExecuted(AbstractAction *action) override {
    if (auto extAction = dynamic_cast<const ExtensionActionV2 *>(action)) {
      qDebug() << "extension action!";
      notify(extAction->model().onAction, {});
    }
  }

  void setActionPanel(const ActionPannelModel &model) {
    auto panel = new ExtensionActionPanelView();

    panel->setDefaultActionShortcuts(m_defaultActionShortcuts);
    panel->setModel(model);

    m_actionPannelV2->setView(panel);

    auto actions = m_actionPannelV2->actions();
    auto primaryAction = m_actionPannelV2->primaryAction();

    m_statusBar->setActionButtonVisibility(!actions.empty() && (!primaryAction || actions.size() > 1));
    m_statusBar->setCurrentActionButtonVisibility(primaryAction);

    if (auto action = m_actionPannelV2->primaryAction()) {
      m_statusBar->setCurrentAction(action->title(),
                                    action->shortcut.value_or(KeyboardShortcutModel{.key = "return"}));
      m_statusBar->setActionButton("Actions", defaultActionPanelShortcut());
    } else {
      m_statusBar->setActionButton("Actions", KeyboardShortcutModel{.key = "return"});
    }
  }

  void clearActionPanel() {
    qCritical() << "clear action panel";
    m_actionPannelV2->popToRoot();
    m_actionPanelViewStack.clear();
    m_statusBar->clearAction();
  }

  /**
   * Send a notification to the extension.
   * The extension manager will forward the notification accordingly.
   */
  void notify(const QString &handler, const QJsonArray &args) const {
    emit notificationRequested(handler, args);
  }

signals:
  void notificationRequested(const QString &handler, const QJsonArray &args) const;
};

class ExtensionView : public SimpleView {
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
    /*
if (auto listModel = std::get_if<ListModel>(&model)) {
// showInput();
return new ExtensionListComponent(*this);
} else if (auto gridModel = std::get_if<GridModel>(&model)) {
// showInput();
return new ExtensionGridComponent(app);
} else if (auto formModel = std::get_if<FormModel>(&model)) {
// hideInput();
return new ExtensionFormComponent(app);
}
  */

    return nullptr;
  }

  bool inputFilter(QKeyEvent *event) override {
    if (_component) { return _component->inputFilter(event); }

    return false;
  }

public:
  ExtensionView(const ExtensionCommand &command)
      : SimpleView(), _command(command), _layout(new QVBoxLayout), _component(nullptr) {
    setNavigationTitle(_command.name());
    setNavigationIcon(_command.iconUrl());
    setupUI(new QWidget);
  }

  const ExtensionCommand &command() const { return _command; }

  void onSearchChanged(const QString &s) override {
    if (_component) { _component->onSearchChanged(s); }
  }

  bool submitForm(const EventHandler &callback) {
    if (auto form = dynamic_cast<ExtensionFormComponent *>(_component)) {
      form->handleSubmit(callback);
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
