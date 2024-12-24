#pragma once
#include "app-database.hpp"
#include "app.hpp"
#include "extension-view.hpp"
#include "extension_manager.hpp"
#include <qboxlayout.h>
#include <qjsonobject.h>
#include <qlabel.h>
#include <qlistwidget.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
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

class ExtensionCommand : public ViewCommand {
  Q_OBJECT

  QStack<ExtensionView *> viewStack;
  QString extensionId;
  QString commandName;
  QString sessionId;
  AppWindow &app;
  std::optional<QJsonObject> lastRender;
  bool popFlag = false;

private slots:
  void commandLoaded(const LoadedCommand &cmd) {
    sessionId = cmd.sessionId;

    qDebug() << "Extension command loaded" << sessionId;
  }

  void batchRender() {
    if (lastRender) {
      render(*lastRender);
      lastRender = std::nullopt;
    }
  }

  void forwardExtensionEvent(const QString &action, const QJsonObject &obj) {

    app.extensionManager->emitExtensionEvent(this->sessionId, action, obj);
  }

  void render(const QJsonObject &payload) {
    if (!viewStack.isEmpty()) {
      auto top = viewStack.top();

      top->render(payload);
    } else {
      /*
auto view = new ExtensionView(app);

connect(view, &ExtensionView::extensionEvent, this,
    &ExtensionCommand::forwardExtensionEvent);

viewStack.push(view);
app.pushView(view);
view->render(payload);
*/
      auto view = new ExtensionView(app);

      pushView(view);
      view->render(payload);
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
      render(payload);
    }

    app.extensionManager->flush();
  }

public:
  ExtensionCommand(AppWindow &app, const QString &extensionId,
                   const QString &commandName)
      : app(app), extensionId(extensionId), commandName(commandName) {}

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

    auto timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, &ExtensionCommand::batchRender);

    timer->start(10);

    app.extensionManager->loadCommand(extensionId, commandName);

    return nullptr;
  }

  void unload(AppWindow &app) override {
    if (!sessionId.isEmpty())
      app.extensionManager->unloadCommand(sessionId);
  }
};
