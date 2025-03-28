#pragma once
#include "command.hpp"
#include "extend/model-parser.hpp"
#include "extension/extension-view.hpp"
#include "extension_manager.hpp"
#include <iostream>
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

class ExtensionCommand : public ViewCommand {
  Q_OBJECT

  std::vector<ExtensionView *> viewStack;
  QString extensionId;
  QString commandName;
  QString sessionId;
  AppWindow &app;
  QFutureWatcher<std::vector<RenderModel>> modelWatcher;

private slots:
  void commandLoaded(const LoadedCommand &cmd) {
    sessionId = cmd.sessionId;

    qDebug() << "Extension command loaded from new extension command" << sessionId;
  }

  void forwardExtensionEvent(const QString &action, const QJsonObject &obj) {

    qDebug() << "forward action" << action;
    app.extensionManager->emitExtensionEvent(this->sessionId, action, obj);
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
        auto view = new ExtensionView(app);

        pushView(view);
        view->render(model);
      }
    }
  }

  void pushView(ExtensionView *view) {
    if (!viewStack.empty()) {
      auto view = viewStack.at(viewStack.size() - 1);
      disconnect(view, &ExtensionView::notifyEvent, this, &ExtensionCommand::forwardExtensionEvent);
    }

    connect(view, &ExtensionView::notifyEvent, this, &ExtensionCommand::forwardExtensionEvent);

    viewStack.push_back(view);
    app.pushView(view);
  }

  void handlePopViewRequest() {
    viewStack.pop_back();
    app.popCurrentView();
  }

  void extensionRequest(const QString &sessionId, const QString &id, const QString &action,
                        const QJsonObject &payload) {
    if (this->sessionId != sessionId) return;

    qDebug() << "[ExtensionCommand] extension request" << action;

    if (action == "list-applications") {
      QJsonArray apps;

      for (const auto &app : app.appDb->list()) {
        if (!app->displayable()) continue;

        QJsonObject appObj;

        appObj["id"] = app->id();
        appObj["name"] = app->name();
        appObj["icon"] = app->iconUrl().toString();

        apps.push_back(appObj);
      }

      QJsonObject responseData;

      responseData["apps"] = apps;

      app.extensionManager->respond(id, responseData);
    }

    if (action == "clipboard-copy") {
      app.clipboardService->copyText(payload.value("text").toString());
      app.statusBar->setToast("Copied into clipboard");
      app.extensionManager->respond(id, {});
    }

    if (action == "push-view") {
      pushView(new ExtensionView(app));
      app.extensionManager->respond(id, {});
    }

    if (action == "pop-view") {
      handlePopViewRequest();
      app.extensionManager->respond(id, {});
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

public:
  ExtensionCommand(AppWindow &app, const QString &extensionId, const QString &commandName)
      : app(app), extensionId(extensionId), commandName(commandName) {}

  ~ExtensionCommand() {}

  View *load(AppWindow &app) override {
    connect(&modelWatcher, &QFutureWatcher<RenderModel>::finished, this, &ExtensionCommand::modelCreated);
    connect(app.extensionManager.get(), &ExtensionManager::commandLoaded, this,
            &ExtensionCommand::commandLoaded);
    connect(app.extensionManager.get(), &ExtensionManager::extensionEvent, this,
            &ExtensionCommand::extensionEvent);
    connect(app.extensionManager.get(), &ExtensionManager::extensionRequest, this,
            &ExtensionCommand::extensionRequest);

    connect(&app, &AppWindow::currentViewPoped, this, [this, &app]() {
      qDebug() << "curent view poped from extension";
      viewStack.pop_back();

      if (!viewStack.empty()) {
        auto top = viewStack.at(viewStack.size() - 1);

        connect(top, &ExtensionView::notifyEvent, this, &ExtensionCommand::forwardExtensionEvent);
      }

      app.extensionManager->emitExtensionEvent(sessionId, "pop-view", {});
    });

    app.extensionManager->loadCommand(extensionId, commandName);

    return nullptr;
  }

  void unload(AppWindow &app) override {
    if (!sessionId.isEmpty()) app.extensionManager->unloadCommand(sessionId);
  }
};
