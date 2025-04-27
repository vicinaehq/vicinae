#pragma once
#include "command-database.hpp"
#include "command.hpp"
#include "common.hpp"
#include "extension/extension-command.hpp"
#include "extension/extension-view.hpp"
#include "extension_manager.hpp"
#include "local-storage-service.hpp"
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qlogging.h>
#include <qobject.h>
#include <qthread.h>

class ExtensionCommandRuntime : public CommandContext {
  std::shared_ptr<ExtensionCommand> m_command;
  std::vector<ExtensionView *> m_viewStack;
  QFutureWatcher<std::vector<RenderModel>> m_modelWatcher;

  AppWindow *m_app;
  ExtensionManager *m_manager;
  QString m_sessionId;

  void sendEvent(const QString &handlerId, const QJsonArray &args) {
    QJsonObject payload;

    payload["args"] = args;
    m_manager->emitExtensionEvent(m_sessionId, handlerId, payload);
  }

  QJsonObject handleStorage(const QString &action, const QJsonObject &payload) {
    LocalStorageService *m_storage = m_app->localStorage.get();

    if (action == "storage.clear") {
      m_storage->clearNamespace(m_command->extensionId());

      return {};
    }

    if (action == "storage.get") {
      auto key = payload.value("key").toString();
      QJsonValue value = m_storage->getItem(m_command->extensionId(), key);
      QJsonObject response;

      response["value"] = value;

      return response;
    }

    if (action == "storage.set") {
      auto key = payload.value("key").toString();
      auto value = payload.value("value");

      m_storage->setItem(m_command->extensionId(), key, value);

      return {};
    }

    if (action == "storage.remove") {
      auto key = payload.value("key").toString();

      m_storage->removeItem(m_command->extensionId(), key);
      return {};
    }

    if (action == "storage.list") {
      QJsonObject response;

      response["values"] = m_storage->listNamespaceItems(m_command->extensionId());

      return response;
    }

    return {};
  }

  QJsonObject handleAI(const QString &action, const QJsonObject &payload) {
    auto manager = m_app->aiProvider.get();

    if (action == "ai.get-models") {
      QJsonArray items;

      for (const auto &model : manager->listModels()) {
        if (!(model.capabilities & AiModel::Capability::Reasoning)) continue;

        QJsonObject obj;

        obj["id"] = model.id;
        obj["name"] = model.displayName;
        obj["icon"] = model.iconName ? *model.iconName : QJsonValue();

        items.push_back(obj);
      }

      return {{"models", items}};
    }

    if (action == "ai.create-completion") {
      auto prompt = payload.value("prompt").toString();
      auto callback = payload.value("callback").toString();
      auto completion = manager->createCompletion(prompt);

      connect(completion, &StreamedChatCompletion::tokenReady, this,
              [this, callback](const ChatCompletionToken &token) {
                QJsonObject payload;

                payload["token"] = token.message.content().toString();
                payload["done"] = false;

                sendEvent(callback, {payload});
              });
      connect(completion, &StreamedChatCompletion::finished, this, [this, completion, callback]() {
        QJsonObject payload;

        payload["token"] = "";
        payload["done"] = true;
        sendEvent(callback, {payload});
        completion->deleteLater();
      });
      connect(completion, &StreamedChatCompletion::errorOccured, this, [this, completion, callback]() {
        QJsonObject payload;

        payload["token"] = "";
        payload["done"] = true;
        sendEvent(callback, {payload});
        completion->deleteLater();
      });

      return {{"started", true}};
    }

    return {};
  }

  QJsonObject handleAppRequest(const QString &action, const QJsonObject &payload) {
    AbstractAppDatabase *appDb = m_app->appDb.get();

    if (action == "apps.list") {
      QJsonArray apps;

      for (const auto &app : appDb->list()) {
        if (!app->displayable()) continue;

        QJsonObject appObj;

        appObj["id"] = app->id();
        appObj["name"] = app->name();
        appObj["icon"] = app->iconUrl().name();

        apps.push_back(appObj);
      }

      return {{"apps", apps}};
    }

    if (action == "apps.open") {
      auto target = payload.value("target").toString();

      if (payload.contains("appId")) {
        auto appId = payload.value("appId").toString();

        if (auto app = appDb->findById(appId)) {
          if (appDb->launch(*app, {target})) return {};
        }
      }

      if (auto opener = appDb->findBestOpener(target)) { appDb->launch(*opener, {target}); }

      return {};
    }

    return {};
  }

  QJsonObject handleToastRequest(const QString &action, const QJsonObject &payload) {
    if (action == "toast.show") {
      auto title = payload["title"].toString();
      auto style = payload["style"].toString();
      ToastPriority priority;

      if (style == "success") {
        priority = ToastPriority::Success;
      } else if (style == "failure") {
        priority = ToastPriority::Danger;
      }

      m_app->statusBar->setToast(title, priority);
      return {};
    }

    if (action == "toast.hide") {
      // XXX - Implement actual toast hide
      return {};
    }

    if (action == "toast.update") {
      // XXX - Implement actual toast update
      return {};
    }

    return {};
  }

  QJsonObject handleClipboard(const QString &action, const QJsonObject &payload) {
    auto clipboardService = m_app->clipboardService.get();

    if (action == "clipboard.copy") {
      auto content = Clipboard::fromJson(payload.value("content").toObject());
      auto options = payload.value("options").toObject();
      Clipboard::CopyOptions copyActions;

      copyActions.concealed = options.value("concealed").toBool(false);
      clipboardService->copyContent(content, copyActions);
      return {};
    }

    return {};
  }

  QJsonObject handleUI(const QString &action, const QJsonObject &payload) {
    if (m_command->isView() && action == "ui.push-view") {
      pushView(new ExtensionView(*m_app, *m_command.get()));
      return {};
    }

    if (m_command->isView() && action == "ui.pop-view") {
      handlePopViewRequest();
      return {};
    }

    if (action == "ui.show-hud") {
      m_app->close();
      m_app->popToRoot();
      return {};
    }

    if (action == "ui.close-main-window") {
      m_app->close();
      return {};
    }

    if (action == "ui.clear-search-bar") {
      m_app->topBar->input->clear();
      emit m_app->topBar->input->textEdited("");

      return {};
    }

    return {};
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
    if (!m_viewStack.empty()) {
      auto view = m_viewStack.at(m_viewStack.size() - 1);
      disconnect(view, &ExtensionView::notifyEvent, this, &ExtensionCommandRuntime::sendEvent);
    }

    connect(view, &ExtensionView::notifyEvent, this, &ExtensionCommandRuntime::sendEvent);
    connect(view, &ExtensionView::updateActionPannel, this, &ExtensionCommandRuntime::updateActionPannel);

    m_viewStack.push_back(view);
    m_app->pushView(
        view, {.navigation = NavigationStatus{.title = m_command->name(), .iconUrl = m_command->iconUrl()}});
  }

  void handlePopViewRequest() {
    m_viewStack.pop_back();
    m_app->popCurrentView();
  }

  void modelCreated() {
    if (m_modelWatcher.isCanceled()) return;

    auto models = m_modelWatcher.result();

    for (int i = 0; i != models.size() && i != m_viewStack.size(); ++i) {
      auto view = m_viewStack.at(i);
      auto model = models.at(i);

      view->render(model);
    }
  }

  void handleRender(const QJsonArray &views) {
    if (m_modelWatcher.isRunning()) {
      m_modelWatcher.cancel();
      m_modelWatcher.waitForFinished();
    }

    m_modelWatcher.setFuture(QtConcurrent::run([views]() { return ModelParser().parse(views); }));
  }

  void handleRequest(const QString &sessionId, const QString &requestId, const QString &action,
                     const QJsonObject &payload) {
    if (sessionId != m_sessionId) return;

    qDebug() << "extension request" << requestId;

    if (action.startsWith("storage.")) {
      m_manager->respond(requestId, handleStorage(action, payload));
      return;
    }

    if (action.startsWith("ai.")) {
      m_manager->respond(requestId, handleAI(action, payload));
      return;
    }

    if (action.startsWith("apps.")) {
      m_manager->respond(requestId, handleAppRequest(action, payload));
      return;
    }

    if (action.startsWith("toast.")) {
      m_manager->respond(requestId, handleToastRequest(action, payload));
      return;
    }

    if (action.startsWith("clipboard.")) {
      m_manager->respond(requestId, handleClipboard(action, payload));
      return;
    }

    if (action.startsWith("ui.")) {
      m_manager->respond(requestId, handleUI(action, payload));
      return;
    }
  }

  void onActionExecuted(AbstractAction *action) override {
    auto extensionAction = static_cast<ExtensionAction *>(action);
    auto &model = extensionAction->model();

    if (auto handler = model.onAction; !handler.isEmpty()) { sendEvent(handler, {}); }

    if (auto onSubmit = model.onSubmit) {
      if (!m_viewStack.empty()) {
        m_viewStack[m_viewStack.size() - 1]->submitForm(*onSubmit);
        return;
      }
    }
  }

  void handleEvent(const QString &sessionId, const QString &action, const QJsonObject &payload) {
    qDebug() << "event" << action << "for " << sessionId;
    if (m_sessionId != sessionId) return;

    if (action == "unload") {}

    if (action == "ui.render") {
      auto views = payload.value("views").toArray();
      QJsonDocument doc;

      doc.setObject(payload);

      // std::cout << doc.toJson().toStdString();

      return handleRender(views);
    }
  }

  void commandLoaded(const LoadedCommand &command) { m_sessionId = command.sessionId; }

public:
  void load(const LaunchProps &props) override {
    auto preferenceValues = m_app->commandDb->getPreferenceValues(m_command->id());

    if (m_command->mode() == CommandModeView) {
      // We push the first view immediately, waiting for the initial render to come
      // in and "hydrate" it.
      pushView(new ExtensionView(*m_app, *m_command.get()));
      connect(m_app, &AppWindow::currentViewPoped, this, [this]() {
        qDebug() << "curent view poped from extension";
        m_viewStack.pop_back();

        if (!m_viewStack.empty()) {
          auto top = m_viewStack.at(m_viewStack.size() - 1);

          connect(top, &ExtensionView::notifyEvent, this, &ExtensionCommandRuntime::sendEvent);
          sendEvent("pop-view", {});
        }
      });
    }

    m_manager->loadCommand(m_command->extensionId(), m_command->id(), preferenceValues, props);
  }

  ExtensionCommandRuntime(AppWindow &app, const std::shared_ptr<ExtensionCommand> &command)
      : CommandContext(&app, command), m_command(command), m_app(&app),
        m_manager(app.extensionManager.get()) {
    connect(app.extensionManager.get(), &ExtensionManager::extensionRequest, this,
            &ExtensionCommandRuntime::handleRequest);
    connect(app.extensionManager.get(), &ExtensionManager::extensionEvent, this,
            &ExtensionCommandRuntime::handleEvent);
    connect(app.extensionManager.get(), &ExtensionManager::commandLoaded, this,
            &ExtensionCommandRuntime::commandLoaded);
    connect(&m_modelWatcher, &QFutureWatcher<RenderModel>::finished, this,
            &ExtensionCommandRuntime::modelCreated);
  }
};
