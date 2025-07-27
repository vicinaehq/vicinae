#pragma once
#include "base-view.hpp"
#include "extend/action-model.hpp"
#include "extend/model-parser.hpp"
#include "extension/extension-command-controller.hpp"
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
  ExtensionCommandController &m_controller;
  ActionModel m_model;

  void execute() override {}

  void execute(ApplicationContext *ctx) override { m_controller.notify(m_model.onAction, {}); }

public:
  const ActionModel &model() const { return m_model; }
  ExtensionActionV2(const ActionModel &model, ExtensionCommandController &controller)
      : AbstractAction(model.title, OmniIconUrl(model.icon.value_or(std::monostate{}))), m_model(model),
        m_controller(controller) {}
};

class ExtensionSimpleView : public SimpleView {
  Q_OBJECT

  ExtensionCommandController *m_controller;
  std::vector<KeyboardShortcutModel> m_defaultActionShortcuts;

public:
  virtual void render(const RenderModel &model) {}

  void setDefaultActionShortcuts(const std::vector<KeyboardShortcutModel> &models) {
    m_defaultActionShortcuts = models;
  }

  void setExtensionCommandController(ExtensionCommandController *controller) { m_controller = controller; }

  void onActionExecuted(AbstractAction *action) override {
    if (auto extAction = dynamic_cast<const ExtensionActionV2 *>(action)) {
      qDebug() << "extension action!";
      notify(extAction->model().onAction, {});
    }
  }

  void setActionPanel(const ActionPannelModel &model) {
    auto panel = std::make_unique<ActionPanelState>();
    size_t idx = 0;
    ActionPanelSectionState *outsideSection = nullptr;

    for (const auto &item : model.children) {
      if (auto section = std::get_if<ActionPannelSectionModel>(&item)) {
        outsideSection = nullptr;

        auto sec = panel->createSection(section->title);

        for (const auto &model : section->actions) {
          auto action = new ExtensionActionV2(model, *m_controller);

          if (idx == 0) { action->setPrimary(true); }

          if (idx < m_defaultActionShortcuts.size()) {
            action->setShortcut(m_defaultActionShortcuts.at(idx));
            ++idx;
          }

          sec->addAction(action);
        }
      }

      if (auto actionModel = std::get_if<ActionModel>(&item)) {
        if (!outsideSection) { outsideSection = panel->createSection(); }

        auto action = new ExtensionActionV2(*actionModel, *m_controller);

        if (idx == 0) { action->setPrimary(true); }

        if (idx < m_defaultActionShortcuts.size()) {
          action->setShortcut(m_defaultActionShortcuts.at(idx));
          ++idx;
        }

        outsideSection->addAction(action);
      }
    }

    setActions(std::move(panel));
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
