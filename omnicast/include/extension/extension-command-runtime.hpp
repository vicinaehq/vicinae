#pragma once
#include "command.hpp"
#include "common.hpp"
#include "extension-error-view.hpp"
#include "extension/extension-command.hpp"
#include "extension/extension-navigation-controller.hpp"
#include "extension/manager/extension-manager.hpp"
#include "extension/requests/app-request-router.hpp"
#include "extension/requests/clipboard-request-router.hpp"
#include "extension/requests/storage-request-router.hpp"
#include "extension/requests/ui-request-router.hpp"
#include "navigation-controller.hpp"
#include "proto/common.pb.h"
#include "proto/extension.pb.h"
#include "proto/manager.pb.h"
#include "service-registry.hpp"
#include "timer.hpp"
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

class ExtensionCommandRuntime : public CommandContext {
  std::shared_ptr<ExtensionCommand> m_command;
  Timer m_timer;

  std::unique_ptr<StorageRequestRouter> m_storageRouter;
  std::unique_ptr<ExtensionNavigationController> m_navigation;
  std::unique_ptr<UIRequestRouter> m_uiRouter;
  std::unique_ptr<AppRequestRouter> m_appRouter;
  std::unique_ptr<ClipboardRequestRouter> m_clipboardRouter;

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

  proto::ext::extension::Response *makeErrorResponse(const QString &errorText) {
    auto res = new proto::ext::extension::Response;
    auto err = new proto::ext::common::ErrorResponse;

    err->set_error_text(errorText.toStdString());
    res->set_allocated_error(err);

    return res;
  }

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
      return m_appRouter->route(data.app());
    case Request::kClipboard:
      return m_clipboardRouter->route(data.clipboard());
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

  void initializeRouters() {
    m_navigation = std::make_unique<ExtensionNavigationController>(m_command, context()->navigation.get(),
                                                                   context()->services->extensionManager());
    m_uiRouter = std::make_unique<UIRequestRouter>(m_navigation.get());
    m_storageRouter =
        std::make_unique<StorageRequestRouter>(context()->services->localStorage(), m_command->extensionId());
    m_appRouter = std::make_unique<AppRequestRouter>(*context()->services->appDb());
    m_clipboardRouter = std::make_unique<ClipboardRequestRouter>(*context()->services->clipman());
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
  }
};
