#pragma once
#include "app-database.hpp"
#include "app.hpp"
#include "extend/model-parser.hpp"
#include "extension-view.hpp"
#include "extension_manager.hpp"
#include <qboxlayout.h>
#include <qjsonobject.h>
#include <qlabel.h>
#include <qlistwidget.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qthread.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class AppWindow;

class Command : public QObject {};

class ViewCommand : public Command {
public:
  ViewCommand() {}

  virtual View *load(AppWindow &) = 0;
  virtual void unload(AppWindow &) {}

  ~ViewCommand() { qDebug() << "destroyed view"; }
};

class HeadlessCommand : public Command {
  virtual void load() = 0;
};

class RenderModeler : public QObject {
  Q_OBJECT

public slots:
  void createModel(const QJsonObject &payload) {
    auto tree = payload.value("root").toObject();

    emit modelCreated(ModelParser().parse(tree));
  }

signals:
  void modelCreated(RenderModel model);
};

class ExtensionCommand : public ViewCommand {
  Q_OBJECT

  QStack<ExtensionView *> viewStack;
  QString extensionId;
  QString commandName;
  QString sessionId;
  AppWindow &app;
  std::optional<QJsonObject> lastRender;
  bool popFlag = false;

  QThread *modelerThread;
  RenderModeler *modeler;

signals:
  void createModelRenderTask(const QJsonObject &payload);

private slots:
  void commandLoaded(const LoadedCommand &cmd) {
    sessionId = cmd.sessionId;

    qDebug() << "Extension command loaded" << sessionId;
  }

  void forwardExtensionEvent(const QString &action, const QJsonObject &obj) {

    app.extensionManager->emitExtensionEvent(this->sessionId, action, obj);
  }

  void modelCreated(RenderModel model) {
    qDebug() << "model created";
    if (!viewStack.isEmpty()) {
      auto top = viewStack.top();

      top->render(model);
    } else {

      auto view = new ExtensionView(app);

      pushView(view);
      view->render(model);
    }
  }

  void pushView(ExtensionView *view) {
    if (!viewStack.isEmpty()) {
      auto view = viewStack.top();
      disconnect(view, &ExtensionView::extensionEvent, this,
                 &ExtensionCommand::forwardExtensionEvent);
    }

    connect(view, &ExtensionView::extensionEvent, this,
            &ExtensionCommand::forwardExtensionEvent);

    viewStack.push(view);
    app.pushView(view);
  }

  void popView() {
    auto old = viewStack.top();
    viewStack.pop();

    old->deleteLater();
    app.popCurrentView();
  }

  void extensionRequest(const QString &sessionId, const QString &id,
                        const QString &action, const QJsonObject &payload) {
    if (this->sessionId != sessionId)
      return;

    qDebug() << "[ExtensionCommand] extension request" << action;

    if (action == "list-applications") {
      QJsonArray apps;

      for (const auto &app : app.appDb->apps) {
        if (!app->displayable())
          continue;

        QJsonObject appObj;

        appObj["id"] = app->id;
        appObj["name"] = app->name;
        appObj["icon"] = app->iconName();

        apps.push_back(appObj);
      }

      QJsonObject responseData;

      responseData["apps"] = apps;

      app.extensionManager->respond(id, responseData);
    }

    if (action == "push-view") {
      pushView(new ExtensionView(app));
      app.extensionManager->respond(id, {});
    }

    if (action == "pop-view") {
      popView();
      app.extensionManager->respond(id, {});
    }
  }

  void extensionEvent(const QString &sessionId, const QString &action,
                      const QJsonObject &payload) {
    if (this->sessionId != sessionId)
      return;

    if (action == "render") {
      emit createModelRenderTask(payload);
    }
  }

public:
  ExtensionCommand(AppWindow &app, const QString &extensionId,
                   const QString &commandName)
      : app(app), extensionId(extensionId), commandName(commandName),
        modelerThread(new QThread), modeler(new RenderModeler) {
    modeler->moveToThread(modelerThread);

    connect(this, &ExtensionCommand::createModelRenderTask, modeler,
            &RenderModeler::createModel);
    connect(modeler, &RenderModeler::modelCreated, this,
            &ExtensionCommand::modelCreated);

    modelerThread->start();
  }

  ~ExtensionCommand() {
    modelerThread->quit();
    modelerThread->deleteLater();
    modeler->deleteLater();
  }

  View *load(AppWindow &app) override {
    connect(app.extensionManager.get(), &ExtensionManager::commandLoaded, this,
            &ExtensionCommand::commandLoaded);
    connect(app.extensionManager.get(), &ExtensionManager::extensionEvent, this,
            &ExtensionCommand::extensionEvent);
    connect(app.extensionManager.get(), &ExtensionManager::extensionRequest,
            this, &ExtensionCommand::extensionRequest);

    connect(&app, &AppWindow::currentViewPoped, this, [this, &app]() {
      qDebug() << "curent view poped from extension";

      popFlag = true;

      auto old = viewStack.top();

      viewStack.pop();
      old->deleteLater();

      if (!viewStack.isEmpty()) {
        connect(viewStack.top(), &ExtensionView::extensionEvent, this,
                &ExtensionCommand::forwardExtensionEvent);
      }

      app.extensionManager->emitExtensionEvent(sessionId, "pop-view", {});
    });

    app.extensionManager->loadCommand(extensionId, commandName);

    return nullptr;
  }

  void unload(AppWindow &app) override {
    if (!sessionId.isEmpty())
      app.extensionManager->unloadCommand(sessionId);
  }
};
