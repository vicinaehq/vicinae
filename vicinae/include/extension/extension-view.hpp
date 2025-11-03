#pragma once
#include "navigation-controller.hpp"
#include "ui/views/base-view.hpp"
#include "extend/action-model.hpp"
#include "extend/model-parser.hpp"
#include "extension/extension-command-controller.hpp"
#include "ui/image/url.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/views/simple-view.hpp"
#include "common-actions.hpp"
#include "create-quicklink-command.hpp"
#include <qboxlayout.h>
#include <qevent.h>
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qlogging.h>
#include <qstackedlayout.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class ExtensionSimpleView : public SimpleView {
  Q_OBJECT

  ExtensionCommandController *m_controller;
  std::vector<Keyboard::Shortcut> m_defaultActionShortcuts;

  AbstractAction *createActionFromModel(const ActionModel &model) {
    if (model.type == "create-quicklink") {
      auto quicklinkObj = model.quicklink;
      QString name = quicklinkObj.value("name").toString();
      QString link = quicklinkObj.value("link").toString();
      QString application = quicklinkObj.value("application").toString();
      QString icon;

      if (quicklinkObj.contains("icon")) {
        auto iconValue = quicklinkObj.value("icon");

        if (iconValue.isString()) { icon = iconValue.toString(); }
      }

      auto view = new ShortcutFormView();
      view->setPrefilledValues(link, name, application, icon);
      ImageURL actionIcon;

      if (model.icon) {
        actionIcon = ImageURL(*model.icon);
      } else {
        actionIcon = ImageURL::builtin("link");
      }

      return new PushViewAction(model.title, view, actionIcon);
    }

    return new StaticAction(model.title, model.icon, [this, model]() {
      qDebug() << "notify action" << model.onAction;
      notify(model.onAction, {});

      if (auto handler = model.onSubmit) {
        auto res = submit();

        if (res.has_value()) {
          notify(*handler, {res.value()});
        } else {
          qCritical() << "Failed to submit action" << res.error();
        }
      }
    });
  }

  /**
   * Called when a submit action is executed.
   * This function should return the submission data that is going to be sent
   * to the submit handler.
   *
   * If for any reason the submission is not possible, an error should be returned.
   *
   * Note that only form views are expected to handle submit events, but for practical reasons every action
   * can technically have an optional submit handler.
   *
   * The onAction handler is fired before the submission one.
   */
  virtual tl::expected<QJsonObject, QString> submit() {
    return tl::unexpected("This view can't handle submit actions");
  }

public:
  virtual void render(const RenderModel &model) {}

  void setDefaultActionShortcuts(const std::vector<Keyboard::Shortcut> &models) {
    m_defaultActionShortcuts = models;
  }

  void setExtensionCommandController(ExtensionCommandController *controller) { m_controller = controller; }

  void setActionPanel(const ActionPannelModel &model) {
    auto panel = std::make_unique<ActionPanelState>();
    size_t idx = 0;
    ActionPanelSectionState *outsideSection = nullptr;

    for (const auto &item : model.children) {
      if (auto section = std::get_if<ActionPannelSectionModel>(&item)) {
        outsideSection = nullptr;

        auto sec = panel->createSection(section->title);

        for (const auto &model : section->actions) {
          auto action = createActionFromModel(model);

          if (idx == 0) { action->setPrimary(true); }

          if (idx < m_defaultActionShortcuts.size()) {
            action->addShortcut(m_defaultActionShortcuts.at(idx));
            ++idx;
          }

          if (model.shortcut) { action->addShortcut(model.shortcut.value()); }

          sec->addAction(action);
        }
      }

      if (auto actionModel = std::get_if<ActionModel>(&item)) {
        if (!outsideSection) { outsideSection = panel->createSection(); }

        auto action = createActionFromModel(*actionModel);

        if (idx == 0) { action->setPrimary(true); }

        if (idx < m_defaultActionShortcuts.size()) {
          action->setShortcut(m_defaultActionShortcuts.at(idx));
          ++idx;
        }

        if (actionModel->shortcut) { action->addShortcut(actionModel->shortcut.value()); }

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
