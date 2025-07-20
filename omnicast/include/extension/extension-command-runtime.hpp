#pragma once
#include "command.hpp"
#include "common.hpp"
#include "extension-error-view.hpp"
#include "extend/model-parser.hpp"
#include "extension/extension-command.hpp"
#include "extension/extension-navigation-controller.hpp"
#include "extension/extension-view-wrapper.hpp"
#include "extension/manager/extension-manager.hpp"
#include "extension/requests/storage-request-router.hpp"
#include "extension/requests/ui-request-router.hpp"
#include "navigation-controller.hpp"
#include "proto/application.pb.h"
#include "proto/common.pb.h"
#include "proto/extension.pb.h"
#include "proto/manager.pb.h"
#include "proto/ui.pb.h"
#include "service-registry.hpp"
#include "timer.hpp"
#include <algorithm>
#include <google/protobuf/struct.pb.h>
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

class ExtensionCommandRuntime : public CommandContext {
  std::shared_ptr<ExtensionCommand> m_command;
  std::vector<ExtensionViewWrapper *> m_viewStack;
  QFutureWatcher<ParsedRenderData> m_modelWatcher;
  Timer m_timer;

  std::unique_ptr<StorageRequestRouter> m_storageRouter;
  std::unique_ptr<ExtensionNavigationController> m_navigation;
  std::unique_ptr<UIRequestRouter> m_uiRouter;

  QString m_sessionId;

  void notify(const QString &handlerId, const QJsonArray &args) {
    auto manager = ServiceRegistry::instance()->extensionManager();

    manager->emitGenericExtensionEvent(m_sessionId, handlerId, args);
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

  proto::ext::ui::Response *handleRender(const proto::ext::ui::RenderRequest &request) {
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

    auto response = new proto::ext::ui::Response;

    response->set_allocated_render(new proto::ext::common::AckResponse);

    // render queued
    return response;
  }

  proto::ext::extension::Response *makeErrorResponse(const QString &errorText) {
    auto res = new proto::ext::extension::Response;
    auto err = new proto::ext::common::ErrorResponse;

    err->set_error_text(errorText.toStdString());
    res->set_allocated_error(err);

    return res;
  }

  void handleApp(const proto::ext::application::Request &req) {
    using Request = proto::ext::application::Request;
  }

  void handleClipboard(const proto::ext::clipboard::Request &req) {}

  proto::ext::extension::Response *dispatchRequest(const ExtensionRequest &request) {
    using Request = proto::ext::extension::RequestData;

    auto &data = request.requestData();

    qDebug() << "got request";

    switch (data.payload_case()) {
    case Request::kUi:
      return m_uiRouter->route(data.ui());
    case Request::kStorage:
      return m_storageRouter->route(data.storage());
    case Request::kApp:
      // return handleApp(data.app());
    case Request::kClipboard:
      // return handleClipboard(data.clipboard());
    default:
      break;
    }

    return makeErrorResponse("Unhandled top level request");
  }

  void handleRequest(ExtensionRequest &request) {
    if (request.sessionId() != m_sessionId) return;

    request.respond(dispatchRequest(request));
  }

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

  void initializeRouters() {
    m_navigation = std::make_unique<ExtensionNavigationController>(m_command, context()->navigation.get(),
                                                                   context()->services->extensionManager());
    m_uiRouter = std::make_unique<UIRequestRouter>(m_navigation.get());
    m_storageRouter =
        std::make_unique<StorageRequestRouter>(context()->services->localStorage(), m_command->extensionId());
  }

public:
  void load(const LaunchProps &props) override {
    initializeRouters();

    auto rootItemManager = ServiceRegistry::instance()->rootItemManager();
    auto preferenceValues =
        rootItemManager->getPreferenceValues(QString("extension.%1").arg(m_command->uniqueId()));
    auto manager = ServiceRegistry::instance()->extensionManager();

    if (m_command->mode() == CommandModeView) {
      // We push the first view immediately, waiting for the initial render to come
      // in and "hydrate" it.
      m_navigation->pushView();
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
              m_navigation->setSessionId(m_sessionId);
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
    connect(manager, &ExtensionManager::extensionRequest, this, &ExtensionCommandRuntime::handleRequest);
    connect(manager, &ExtensionManager::extensionEvent, this, &ExtensionCommandRuntime::handleEvent);
    connect(&m_modelWatcher, &QFutureWatcher<RenderModel>::finished, this,
            &ExtensionCommandRuntime::modelCreated);
  }
};
