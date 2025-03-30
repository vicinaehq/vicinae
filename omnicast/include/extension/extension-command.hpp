#pragma once
#include "command.hpp"
#include "extend/action-model.hpp"
#include "extend/model-parser.hpp"
#include "extension/extension-view.hpp"
#include "extension/extension.hpp"
#include "extension_manager.hpp"
#include "omni-icon.hpp"
#include "ui/action-pannel/action-item.hpp"
#include "ui/action-pannel/action.hpp"
#include <iostream>
#include <memory>
#include <qboxlayout.h>
#include <qfuturewatcher.h>
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qlabel.h>
#include <qlistwidget.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qthread.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class ExtensionAction : public AbstractAction {
  ActionModel _model;

public:
  void execute(AppWindow &app) override {}

  const ActionModel &model() const { return _model; }

  ExtensionAction(const ActionModel &model)
      : AbstractAction(model.title, model.icon ? OmniIconUrl(*model.icon) : BuiltinOmniIconUrl("pen")),
        _model(model) {}
};

class ExtensionCommandContext : public ViewCommandContext {
  Q_OBJECT

  std::vector<ExtensionView *> viewStack;
  Extension extension;
  QString sessionId;
  QFutureWatcher<std::vector<RenderModel>> modelWatcher;

private slots:
  void commandLoaded(const LoadedCommand &cmd) {
    sessionId = cmd.sessionId;

    qDebug() << "Extension command loaded from new extension command" << sessionId;
  }

  void modelCreated() {
    if (modelWatcher.isCanceled()) return;

    auto models = modelWatcher.result();

    for (int i = 0; i != models.size(); ++i) {
      auto &model = models.at(i);

      if (i < viewStack.size()) {
        viewStack.at(i)->render(model);
      } else {
        qDebug() << "creating new view";
        auto view = new ExtensionView(*app());

        pushView(view);
        view->render(model);
      }
    }
  }

  void updateActionPannel(const ActionPannelModel &model) {
    std::vector<ActionItem> items;

    items.reserve(model.children.size());

    for (const auto &item : model.children) {
      if (auto actionModel = std::get_if<ActionModel>(&item)) {
        items.push_back(std::make_unique<ExtensionAction>(*actionModel));
      }
    }

    app()->actionPannel->setActions(std::move(items));

    if (auto action = app()->actionPannel->primaryAction()) {
      app()->statusBar->setAction(*action);
    } else {
      app()->statusBar->clearAction();
    }
  }

  void pushView(ExtensionView *view) {
    if (!viewStack.empty()) {
      auto view = viewStack.at(viewStack.size() - 1);
      disconnect(view, &ExtensionView::notifyEvent, this, &ExtensionCommandContext::handleNotifiedEvent);
    }

    connect(view, &ExtensionView::notifyEvent, this, &ExtensionCommandContext::handleNotifiedEvent);
    connect(view, &ExtensionView::updateActionPannel, this, &ExtensionCommandContext::updateActionPannel);

    viewStack.push_back(view);
    app()->pushView(view, {.navigation = NavigationStatus{.title = "fixme", .iconUrl = extension.iconUrl()}});
  }

  void handlePopViewRequest() {
    viewStack.pop_back();
    app()->popCurrentView();
  }

  void extensionRequest(const QString &sessionId, const QString &id, const QString &action,
                        const QJsonObject &payload) {
    if (this->sessionId != sessionId) return;

    qDebug() << "[ExtensionCommand] extension request" << action;

    if (action == "list-applications") {
      QJsonArray apps;

      for (const auto &app : app()->appDb->list()) {
        if (!app->displayable()) continue;

        QJsonObject appObj;

        appObj["id"] = app->id();
        appObj["name"] = app->name();
        appObj["icon"] = app->iconUrl().name();

        apps.push_back(appObj);
      }

      QJsonObject responseData;

      responseData["apps"] = apps;

      app()->extensionManager->respond(id, responseData);
    }

    if (action == "clipboard-copy") {
      app()->clipboardService->copyText(payload.value("text").toString());
      app()->statusBar->setToast("Copied into clipboard");
      app()->extensionManager->respond(id, {});
    }

    if (action == "push-view") {
      pushView(new ExtensionView(*app()));
      app()->extensionManager->respond(id, {});
    }

    if (action == "pop-view") {
      handlePopViewRequest();
      app()->extensionManager->respond(id, {});
    }
  }

  void handleRender(const QJsonArray &views) {
    qDebug() << "handle render";

    if (modelWatcher.isRunning()) {
      modelWatcher.cancel();
      modelWatcher.waitForFinished();
    }

    modelWatcher.setFuture(QtConcurrent::run([views]() { return ModelParser().parse(views); }));
  }

  void extensionEvent(const QString &sessionId, const QString &action, const QJsonObject &payload) {
    qDebug() << "event" << action << "for " << sessionId;
    if (this->sessionId != sessionId) return;

    if (action == "render") {
      auto views = payload.value("views").toArray();
      QJsonDocument doc;

      doc.setObject(payload);

      std::cout << doc.toJson().toStdString();

      return handleRender(views);
    }
  }

  void handleNotifiedEvent(const QString &handlerId, const std::vector<QJsonValue> &args) {
    QJsonObject obj;
    QJsonArray arr;

    for (const auto &arg : args) {
      arr.push_back(arg);
    }

    obj["args"] = arr;
    app()->extensionManager->emitExtensionEvent(this->sessionId, handlerId, obj);
  }

  void actionExecuted(AbstractAction *action) {
    auto extensionAction = static_cast<ExtensionAction *>(action);

    if (auto handler = extensionAction->model().onAction; !handler.isEmpty()) {
      handleNotifiedEvent(handler, {});
    }
  }

public:
  ExtensionCommandContext(AppWindow &app, const std::shared_ptr<AbstractCommand> &command)
      : ViewCommandContext(&app, command), extension() {}

  ~ExtensionCommandContext() {}

  View *view() const override { return viewStack.at(0); }

  void load() override {
    viewStack.push_back(new ExtensionView(*app()));

    connect(&modelWatcher, &QFutureWatcher<RenderModel>::finished, this,
            &ExtensionCommandContext::modelCreated);
    connect(app()->extensionManager.get(), &ExtensionManager::commandLoaded, this,
            &ExtensionCommandContext::commandLoaded);
    connect(app()->extensionManager.get(), &ExtensionManager::extensionEvent, this,
            &ExtensionCommandContext::extensionEvent);
    connect(app()->extensionManager.get(), &ExtensionManager::extensionRequest, this,
            &ExtensionCommandContext::extensionRequest);
    connect(app(), &AppWindow::actionExecuted, this, &ExtensionCommandContext::actionExecuted);
    connect(app(), &AppWindow::currentViewPoped, this, [this]() {
      qDebug() << "curent view poped from extension";
      viewStack.pop_back();

      if (!viewStack.empty()) {
        auto top = viewStack.at(viewStack.size() - 1);

        connect(top, &ExtensionView::notifyEvent, this, &ExtensionCommandContext::handleNotifiedEvent);
      }

      app()->extensionManager->emitExtensionEvent(sessionId, "pop-view", {});
    });

    // TODO: fix this
    // app()->extensionManager->loadCommand(extension.id().toString(), command.name);
  }

  void unload() override {
    if (!sessionId.isEmpty()) app()->extensionManager->unloadCommand(sessionId);
  }
};

class ExtensionViewCommand : public AbstractViewCommand {
  ViewCommandContext *createContext(AppWindow &app, const std::shared_ptr<AbstractCommand> &command,
                                    const QString &query) const override {
    return new ExtensionCommandContext(app, command);
  }
};
