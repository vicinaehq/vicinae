#pragma once
#include "base-view.hpp"
#include "command.hpp"
#include "common.hpp"
#include "extension-error-view.hpp"
#include "extend/model-parser.hpp"
#include "extension/extension-command-controller.hpp"
#include "extension/extension-command.hpp"
#include "extension/extension-view-wrapper.hpp"
#include "extension/extension-view.hpp"
#include "extension/manager/extension-manager.hpp"
#include "proto/application.pb.h"
#include "proto/extension.pb.h"
#include "proto/ipc.pb.h"
#include "proto/manager.pb.h"
#include "proto/storage.pb.h"
#include "proto/ui.pb.h"
#include "services/local-storage/local-storage-service.hpp"
#include "service-registry.hpp"
#include "timer.hpp"
#include <algorithm>
#include <functional>
#include <memory>
#include <optional>
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qjsonparseerror.h>
#include <qjsonvalue.h>
#include <qlogging.h>
#include <qobject.h>
#include <qwidget.h>
#include <ranges>

struct ExtensionViewInfo {
  size_t index;
  ExtensionSimpleView *view;
};

class RequestDispatcher {
public:
  using HandlerCallback =
      std::function<std::optional<QJsonObject>(const QString &action, const QJsonObject &data)>;
  struct HandlerData {
    QString prefix;
    HandlerCallback callback;
  };

private:
  std::vector<HandlerData> m_handlers;

public:
  std::optional<QJsonObject> dispatch(const QString &action, const QJsonObject &data) const {
    auto it = std::ranges::find_if(m_handlers, [&](auto &&lhs) { return action.startsWith(lhs.prefix); });

    if (it == m_handlers.end()) return std::nullopt;

    return it->callback(action, data);
  }

  void registerHandler(const QString &prefix, const HandlerCallback &handler) {
    m_handlers.emplace_back(HandlerData{prefix, handler});
  }
};

struct PushedViewInfo {
  ExtensionSimpleView *view = nullptr;
  size_t index = -1;
};

class ExtensionCommandRuntime : public CommandContext {
  std::shared_ptr<ExtensionCommand> m_command;
  std::vector<ExtensionViewWrapper *> m_viewStack;
  QFutureWatcher<ParsedRenderData> m_modelWatcher;
  RequestDispatcher m_actionDispatcher;
  ExtensionCommandController *m_controller;
  Timer m_timer;

  QString m_sessionId;

  void notify(const QString &handlerId, const QJsonArray &args) {
    auto manager = ServiceRegistry::instance()->extensionManager();

    manager->emitGenericExtensionEvent(m_sessionId, handlerId, args);
  }

  QJsonObject handleStorage(const QString &action, const QJsonObject &payload) {
    auto storage = context()->services->localStorage();

    if (action == "storage.clear") {
      storage->clearNamespace(m_command->extensionId());

      return {};
    }

    if (action == "storage.get") {
      auto key = payload.value("key").toString();
      QJsonValue value = storage->getItem(m_command->extensionId(), key);
      QJsonObject response;

      response["value"] = value;

      return response;
    }

    if (action == "storage.set") {
      auto key = payload.value("key").toString();
      auto value = payload.value("value");

      storage->setItem(m_command->extensionId(), key, value);

      return {};
    }

    if (action == "storage.remove") {
      auto key = payload.value("key").toString();

      storage->removeItem(m_command->extensionId(), key);
      return {};
    }

    if (action == "storage.list") {
      QJsonObject response;

      response["values"] = storage->listNamespaceItems(m_command->extensionId());

      return response;
    }

    return {};
  }

  QJsonObject handleAI(const QString &action, const QJsonObject &payload) {
    auto manager = context()->services->AI();

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

                notify(callback, {payload});
              });
      connect(completion, &StreamedChatCompletion::finished, this, [this, completion, callback]() {
        QJsonObject payload;

        payload["token"] = "";
        payload["done"] = true;
        notify(callback, {payload});
        completion->deleteLater();
      });
      connect(completion, &StreamedChatCompletion::errorOccured, this, [this, completion, callback]() {
        QJsonObject payload;

        payload["token"] = "";
        payload["done"] = true;
        notify(callback, {payload});
        completion->deleteLater();
      });

      return {{"started", true}};
    }

    return {};
  }

  QJsonObject handleAppRequest(const QString &action, const QJsonObject &payload) {
    auto appDb = context()->services->appDb();

    if (action == "apps.list") {
      auto baseApps = appDb->list();
      QJsonArray apps;

      auto serializedApps = baseApps |
                            std::views::filter([](const auto &app) { return app->displayable(); }) |
                            std::views::transform([](const auto &app) {
                              QJsonObject appObj;

                              appObj["id"] = app->id();
                              appObj["name"] = app->name();
                              appObj["icon"] = app->iconUrl().name();

                              return appObj;
                            });

      std::ranges::for_each(serializedApps, [&apps](const auto &a) { apps.push_back(a); });

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
    auto &nav = context()->navigation;
    auto toast = context()->services->toastService();

    if (action == "toast.show") {
      auto title = payload["title"].toString();
      auto style = payload["style"].toString();
      ToastPriority priority;

      if (style == "success") {
        priority = ToastPriority::Success;
      } else if (style == "failure") {
        priority = ToastPriority::Danger;
      }

      toast->setToast(title, priority);
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
    auto clipman = ServiceRegistry::instance()->clipman();

    if (action == "clipboard.copy") {
      auto content = Clipboard::fromJson(payload.value("content").toObject());
      auto options = payload.value("options").toObject();
      Clipboard::CopyOptions copyActions;

      copyActions.concealed = options.value("concealed").toBool(false);
      clipman->copyContent(content, copyActions);
      return {};
    }

    return {};
  }

  QJsonObject handleUI(const QString &action, const QJsonObject &payload) {
    auto &nav = context()->navigation;

    if (m_command->isView() && action == "ui.push-view") {
      pushView();
      return {};
    }

    if (m_command->isView() && action == "ui.pop-view") {
      handlePopViewRequest();
      return {};
    }

    if (action == "ui.show-hud") {
      // TODO: implement hud
      return {};
    }

    if (action == "ui.close-main-window") {
      nav->closeWindow();
      return {};
    }

    if (action == "ui.clear-search-bar") {
      nav->clearSearchText();
      return {};
    }

    return {};
  }

  void handlePopViewRequest() {
    auto ui = ServiceRegistry::instance()->UI();

    m_viewStack.pop_back();
    ui->popView();
  }

  void pushView() {
    auto &nav = context()->navigation;
    auto view = new ExtensionViewWrapper(m_controller);

    connect(view, &ExtensionViewWrapper::notificationRequested, this, &ExtensionCommandRuntime::notify);

    nav->pushView(view);
    nav->setNavigationTitle(m_command->name());
    nav->setNavigationIcon(m_command->iconUrl());
    m_viewStack.emplace_back(view);
  }

  void modelCreated() {
    if (m_modelWatcher.isCanceled()) return;

    auto models = m_modelWatcher.result();
    auto items = models.items | std::views::take(m_viewStack.size()) | std::views::enumerate;

    for (const auto &[n, model] : items) {
      auto view = m_viewStack.at(n);
      bool shouldSkipRender = !model.dirty && !model.propsDirty;

      if (shouldSkipRender) {
        qDebug() << "view" << n << "is not dirty, skipping render";
        continue;
      }

      view->render(model.root);
    }
  }

  void handleRender(const QJsonArray &views) {
    if (m_viewStack.empty()) { m_timer.time("Got Initial render"); }

    if (m_modelWatcher.isRunning()) {
      m_modelWatcher.cancel();
      m_modelWatcher.waitForFinished();
    }

    m_modelWatcher.setFuture(QtConcurrent::run([views]() {
      Timer timer;
      auto model = ModelParser().parse(views);

      timer.time("Model parsed");
      return model;
    }));
  }

  proto::ext::extension::ResponseData *handleRender(const proto::ext::ui::RenderRequest &request) {
    /**
     * For now, we still process the render tree as JSON. Maybe later we can move that to protobuf as well,
     * but that will require writing more serialization code in the reconciler.
     */
    QJsonParseError parseError;
    auto doc = QJsonDocument::fromJson(request.json().c_str(), &parseError);

    if (parseError.error) {
      qCritical() << "Failed to parse render tree";
      return {};
    }

    auto views = doc.object().value("views").toArray();

    if (m_viewStack.empty()) { m_timer.time("Got Initial render"); }

    if (m_modelWatcher.isRunning()) {
      m_modelWatcher.cancel();
      m_modelWatcher.waitForFinished();
    }

    m_modelWatcher.setFuture(QtConcurrent::run([views]() {
      Timer timer;
      auto model = ModelParser().parse(views);

      timer.time("Model parsed");
      return model;
    }));

    auto responseData = new proto::ext::extension::ResponseData;

    responseData->set_allocated_ack(new proto::ext::common::AckResponse);

    // render queued
    return responseData;
  }

  void handleUI(const proto::ext::ui::Request &req) {
    using Request = proto::ext::ui::Request;

    switch (req.payload_case()) {
    case Request::kRender:
      handleRender(req.render());
      break;
    default:
      break;
    }
  }

  void handleStorage(const proto::ext::storage::Request &req) {
    using Request = proto::ext::clipboard::Request;

    switch (req.payload_case()) {
    case Request::kCopy:
      return;
    default:
      break;
    }
  }

  void handleApp(const proto::ext::application::Request &req) {
    using Request = proto::ext::application::Request;
  }

  void handleClipboard(const proto::ext::clipboard::Request &req) {}

  void handleRequest(ExtensionRequest &request) {
    using Request = proto::ext::extension::RequestData;

    if (request.sessionId() != m_sessionId) return;

    auto &data = request.requestData();

    qDebug() << "got request";

    switch (data.payload_case()) {
    case Request::kUi:
      return handleUI(data.ui());
    case Request::kStorage:
      return handleStorage(data.storage());
    case Request::kApp:
      return handleApp(data.app());
    case Request::kClipboard:
      return handleClipboard(data.clipboard());
    default:
      break;
    }
  }

  /*
  void handleRequest(const QString &sessionId, const QString &requestId, const QString &action,
                     const QJsonObject &payload) {
    qDebug() << "request" << action;
    auto manager = ServiceRegistry::instance()->extensionManager();

    if (sessionId != m_sessionId) return;

    auto res = m_actionDispatcher.dispatch(action, payload);

    // manager->respond(requestId, res.value_or(QJsonObject{}));
  }
  */

  void handleCrash(const proto::ext::extension::CrashEventData &crash) {
    qCritical() << "Got crash" << crash.text();
    auto &nav = context()->navigation;

    nav->popToRoot();
    nav->pushView(new ExtensionErrorView(QString::fromStdString(crash.text())));
    nav->setNavigationTitle(QString("%1 - Crash handler").arg(m_command->name()));
    nav->setNavigationIcon(m_command->iconUrl());
  }

  void handleGenericEvent(const proto::ext::extension::GenericEventData &event) {}

  void handleEvent(const ExtensionEvent &event) {
    using Event = proto::ext::extension::Event;

    switch (event.data()->payload_case()) {
    case Event::kCrash:
      return handleCrash(event.data()->crash());
    case Event::kGeneric:
      return handleGenericEvent(event.data()->generic());
    default:
      break;
    }
  }

  void handleError(const QJsonObject &payload) {
    auto message = payload.value("message").toString();

    // push error view
  }

  void handleViewPoped() {
    if (m_viewStack.size() > 1) { notify("pop-view", {}); }

    m_viewStack.pop_back();
  }

public:
  void load(const LaunchProps &props) override {
    auto rootItemManager = ServiceRegistry::instance()->rootItemManager();
    auto preferenceValues =
        rootItemManager->getPreferenceValues(QString("extension.%1").arg(m_command->uniqueId()));
    auto manager = ServiceRegistry::instance()->extensionManager();

    if (m_command->mode() == CommandModeView) {
      // We push the first view immediately, waiting for the initial render to come
      // in and "hydrate" it.
      pushView();
      connect(context()->navigation.get(), &NavigationController::viewPoped, this,
              &ExtensionCommandRuntime::handleViewPoped);
    }

    auto load = new proto::ext::manager::ManagerLoadCommand;
    auto payload = new proto::ext::manager::RequestData;

    load->set_entrypoint(m_command->manifest().entrypoint);
    load->set_env(proto::ext::manager::CommandEnv::Development);
    load->set_mode(proto::ext::manager::CommandMode::View);
    load->set_extension_path("");
    payload->set_allocated_load(load);

    auto loadRequest = manager->requestManager(payload);

    connect(loadRequest, &ManagerRequest::finished, this,
            [this, loadRequest](const proto::ext::manager::ResponseData &data) {
              m_sessionId = QString::fromStdString(data.load().session_id());
              m_controller->setSessionId(m_sessionId);
              loadRequest->deleteLater();
              m_timer.time("Extension loaded");
              m_timer.start();
            });
  }

  void unload() override {
    auto manager = context()->services->extensionManager();

    manager->unloadCommand(m_sessionId);
  }

  ExtensionCommandRuntime(const std::shared_ptr<ExtensionCommand> &command)
      : CommandContext(command), m_command(command) {
    auto manager = ServiceRegistry::instance()->extensionManager();
    m_controller = new ExtensionCommandController(manager);

    connect(manager, &ExtensionManager::extensionRequest, this, &ExtensionCommandRuntime::handleRequest);
    connect(manager, &ExtensionManager::extensionEvent, this, &ExtensionCommandRuntime::handleEvent);
    connect(&m_modelWatcher, &QFutureWatcher<RenderModel>::finished, this,
            &ExtensionCommandRuntime::modelCreated);
  }
};
