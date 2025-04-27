#include "ai/ai-provider.hpp"
#include "clipboard/clipboard-service.hpp"
#include "command.hpp"
#include "extension/extension-command-runtime.hpp"
#include "extension/extension-command.hpp"
#include "extension/extension-view.hpp"
#include "ui/toast.hpp"
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qnamespace.h>
#include <qtmetamacros.h>

class ExtensionNoViewCommandContext : public CommandContext {
  Q_OBJECT
  QString sessionId;
  const ExtensionCommand &command;

private slots:
  void commandLoaded(const LoadedCommand &cmd) {
    sessionId = cmd.sessionId;

    qDebug() << "Extension command loaded from new extension command" << sessionId;
  }

  void extensionRequest(const QString &sessionId, const QString &id, const QString &action,
                        const QJsonObject &payload) {
    if (this->sessionId != sessionId) return;

    qDebug() << "[ExtensionCommand] extension request" << action;

    if (action == "clear-search-bar") {
      app()->topBar->input->clear();
      emit app() -> topBar->input->textEdited("");
      app()->extensionManager->respond(id, {});
      return;
    }

    if (action == "push-view") {
      app()->extensionManager->respond(id, {});
      return;
    }

    if (action == "pop-view") {
      app()->extensionManager->respond(id, {});
      return;
    }

    QJsonObject errorRes;
    QJsonObject err;

    err["message"] = "Unknown command type";
    errorRes["error"] = err;

    app()->extensionManager->respond(id, errorRes);
  }

  void extensionEvent(const QString &sessionId, const QString &action, const QJsonObject &payload) {
    qDebug() << "event" << action << "for " << sessionId;
    if (this->sessionId != sessionId) return;
  }

  void handleNotifiedEvent(const QString &handlerId, const std::vector<QJsonValue> &args) {
    QJsonObject obj;
    QJsonArray arr;

    qDebug() << "send event to" << handlerId;

    for (const auto &arg : args) {
      arr.push_back(arg);
    }

    obj["args"] = arr;
    app()->extensionManager->emitExtensionEvent(this->sessionId, handlerId, obj);
  }

public:
  ExtensionNoViewCommandContext(AppWindow &app, const std::shared_ptr<AbstractCmd> &command)
      : CommandContext(&app, command), command(static_cast<ExtensionCommand &>(*command.get())) {}

  ~ExtensionNoViewCommandContext() {}

  void load(const LaunchProps &props) override {
    connect(app()->extensionManager.get(), &ExtensionManager::commandLoaded, this,
            &ExtensionNoViewCommandContext::commandLoaded);
    connect(app()->extensionManager.get(), &ExtensionManager::extensionEvent, this,
            &ExtensionNoViewCommandContext::extensionEvent);
    connect(app()->extensionManager.get(), &ExtensionManager::extensionRequest, this,
            &ExtensionNoViewCommandContext::extensionRequest);

    auto preferenceValues = app()->commandDb->getPreferenceValues(command.id());

    app()->extensionManager->loadCommand(command.extensionId(), command.id(), preferenceValues);
  }

  void unload() override {
    if (!sessionId.isEmpty()) app()->extensionManager->unloadCommand(sessionId);
  }
};

class ExtensionCommandContext : public CommandContext {
  Q_OBJECT

  std::vector<ExtensionView *> viewStack;
  QString sessionId;
  QFutureWatcher<std::vector<RenderModel>> modelWatcher;
  const ExtensionCommand &command;

private slots:
  void commandLoaded(const LoadedCommand &cmd) {
    sessionId = cmd.sessionId;

    qDebug() << "Extension command loaded from new extension command" << sessionId;
  }

  void modelCreated() {
    if (modelWatcher.isCanceled()) return;

    auto models = modelWatcher.result();

    for (int i = 0; i != models.size() && i != viewStack.size(); ++i) {
      auto view = viewStack.at(i);
      auto model = models.at(i);

      view->render(model);
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
    app()->pushView(view,
                    {.navigation = NavigationStatus{.title = command.name(), .iconUrl = command.iconUrl()}});
  }

  void handlePopViewRequest() {
    viewStack.pop_back();
    app()->popCurrentView();
  }

  void extensionRequest(const QString &sessionId, const QString &id, const QString &action,
                        const QJsonObject &payload) {
    if (this->sessionId != sessionId) return;

    qDebug() << "[ExtensionCommand] extension request" << action;

    if (action == "storage.clear") {
      app()->localStorage->clearNamespace(command.extensionId());
      app()->extensionManager->respond(id, {});
    }

    if (action == "storage.get") {
      auto key = payload.value("key").toString();
      QJsonValue value = app()->localStorage->getItem(command.extensionId(), key);
      QJsonObject response;

      response["value"] = value;

      app()->extensionManager->respond(id, response);
      return;
    }

    if (action == "storage.set") {
      auto key = payload.value("key").toString();
      auto value = payload.value("key");

      app()->localStorage->setItem(command.extensionId(), key, value);
      app()->extensionManager->respond(id, QJsonObject());
      return;
    }

    if (action == "storage.remove") {
      auto key = payload.value("key").toString();

      app()->localStorage->removeItem(command.extensionId(), key);
      app()->extensionManager->respond(id, QJsonObject());
      return;
    }

    if (action == "storage.list") {
      QJsonObject response;

      response["values"] = app()->localStorage->listNamespaceItems(command.extensionId());

      app()->extensionManager->respond(id, response);
      return;
    }

    if (action == "ai.get-models") {
      auto &provider = app()->aiProvider;
      QJsonArray items;

      for (const auto &model : provider->listModels()) {
        if (!(model.capabilities & AiModel::Capability::Reasoning)) continue;

        QJsonObject obj;

        obj["id"] = model.id;
        obj["name"] = model.displayName;
        obj["icon"] = model.iconName ? *model.iconName : QJsonValue();

        items.push_back(obj);
      }

      QJsonObject responseData;

      responseData["models"] = items;

      app()->extensionManager->respond(id, responseData);
      return;
    }

    if (action == "ai.create-completion") {
      auto &provider = app()->aiProvider;
      auto prompt = payload.value("prompt").toString();
      auto callback = payload.value("callback").toString();
      auto completion = provider->createCompletion(prompt);

      connect(completion, &StreamedChatCompletion::tokenReady, this,
              [this, sessionId, callback](const ChatCompletionToken &token) {
                QJsonObject payload;

                payload["token"] = token.message.content().toString();
                payload["done"] = false;

                handleNotifiedEvent(callback, {payload});
              });
      connect(completion, &StreamedChatCompletion::finished, this, [this, completion, callback]() {
        QJsonObject payload;

        payload["token"] = "";
        payload["done"] = true;
        handleNotifiedEvent(callback, {payload});
        completion->deleteLater();
      });
      connect(completion, &StreamedChatCompletion::errorOccured, this, [this, completion, callback]() {
        QJsonObject payload;

        payload["token"] = "";
        payload["done"] = true;
        handleNotifiedEvent(callback, {payload});
        completion->deleteLater();
      });

      QJsonObject res;

      res["started"] = true;

      app()->extensionManager->respond(id, res);
    }

    if (action == "apps.get") {
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
      return;
    }

    if (action == "toast.show") {
      auto title = payload["title"].toString();
      auto style = payload["style"].toString();
      ToastPriority priority;

      if (style == "success") {
        priority = ToastPriority::Success;
      } else if (style == "failure") {
        priority = ToastPriority::Danger;
      }

      app()->statusBar->setToast(title, priority);
      app()->extensionManager->respond(id, {});
      return;
    }

    if (action == "toast.hide") {
      // TODO: hide it for real!
      app()->extensionManager->respond(id, {});
      return;
    }

    if (action == "clear-search-bar") {
      app()->topBar->input->clear();
      emit app() -> topBar->input->textEdited("");
      app()->extensionManager->respond(id, {});
      return;
    }

    if (action == "clipboard.copy") {
      auto content = Clipboard::fromJson(payload.value("content").toObject());
      auto options = payload.value("options").toObject();
      Clipboard::CopyOptions copyActions;

      copyActions.concealed = options.value("concealed").toBool(false);
      app()->clipboardService->copyContent(content, copyActions);
      app()->statusBar->setToast("Copied into clipboard");
      app()->extensionManager->respond(id, {});
      return;
    }

    if (action == "push-view") {
      pushView(new ExtensionView(*app(), command));
      app()->extensionManager->respond(id, {});
      return;
    }

    if (action == "pop-view") {
      handlePopViewRequest();
      app()->extensionManager->respond(id, {});
      return;
    }

    QJsonObject errorRes;
    QJsonObject err;

    err["message"] = "Unknown command type";
    errorRes["error"] = err;

    app()->extensionManager->respond(id, errorRes);
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

      // std::cout << doc.toJson().toStdString();

      return handleRender(views);
    }
  }

  void handleNotifiedEvent(const QString &handlerId, const QJsonArray &args) {
    QJsonObject obj;
    QJsonArray arr;

    qDebug() << "send event to" << handlerId;

    for (const auto &arg : args) {
      arr.push_back(arg);
    }

    obj["args"] = arr;
    app()->extensionManager->emitExtensionEvent(this->sessionId, handlerId, obj);
  }

  void onActionExecuted(AbstractAction *action) override {
    auto extensionAction = static_cast<ExtensionAction *>(action);
    auto &model = extensionAction->model();

    if (auto handler = model.onAction; !handler.isEmpty()) { handleNotifiedEvent(handler, {}); }

    if (auto onSubmit = model.onSubmit) {
      if (!viewStack.empty()) {
        viewStack[viewStack.size() - 1]->submitForm(*onSubmit);
        return;
      }
    }
  }

public:
  ExtensionCommandContext(AppWindow &app, const std::shared_ptr<AbstractCmd> &command)
      : CommandContext(&app, command), command(static_cast<ExtensionCommand &>(*command.get())) {}

  ~ExtensionCommandContext() {}

  void load(const LaunchProps &props) override {
    connect(&modelWatcher, &QFutureWatcher<RenderModel>::finished, this,
            &ExtensionCommandContext::modelCreated);
    connect(app()->extensionManager.get(), &ExtensionManager::commandLoaded, this,
            &ExtensionCommandContext::commandLoaded);
    connect(app()->extensionManager.get(), &ExtensionManager::extensionEvent, this,
            &ExtensionCommandContext::extensionEvent);
    connect(app()->extensionManager.get(), &ExtensionManager::extensionRequest, this,
            &ExtensionCommandContext::extensionRequest);
    connect(app(), &AppWindow::currentViewPoped, this, [this]() {
      qDebug() << "curent view poped from extension";
      viewStack.pop_back();

      if (!viewStack.empty()) {
        auto top = viewStack.at(viewStack.size() - 1);

        connect(top, &ExtensionView::notifyEvent, this, &ExtensionCommandContext::handleNotifiedEvent);
        app()->extensionManager->emitExtensionEvent(sessionId, "pop-view", {});
      }
    });

    auto preferenceValues = app()->commandDb->getPreferenceValues(command.id());

    pushView(new ExtensionView(*app(), command));
    app()->extensionManager->loadCommand(command.extensionId(), command.id(), preferenceValues, props);
  }

  void unload() override {
    if (!sessionId.isEmpty()) app()->extensionManager->unloadCommand(sessionId);
  }
};
