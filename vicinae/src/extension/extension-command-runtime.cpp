#include "extension-command-runtime.hpp"
#include "common.hpp"
#include "extension/extension-navigation-controller.hpp"
#include "extension-error-view.hpp"
#include "extension/requests/app-request-router.hpp"
#include "extension/requests/clipboard-request-router.hpp"
#include "extension/requests/storage-request-router.hpp"
#include "extension/requests/ui-request-router.hpp"
#include "proto/oauth.pb.h"
#include "common.hpp"
#include "service-registry.hpp"
#include "services/asset-resolver/asset-resolver.hpp"
#include "ui/oauth-view.hpp"
#include <QString>
#include "overlay-controller/overlay-controller.hpp"
#include "utils/utils.hpp"

proto::ext::extension::Response *ExtensionCommandRuntime::makeErrorResponse(const QString &errorText) {
  auto res = new proto::ext::extension::Response;
  auto err = new proto::ext::common::ErrorResponse;

  err->set_error_text(errorText.toStdString());
  res->set_allocated_error(err);

  return res;
}

void ExtensionCommandRuntime::handleOAuth(ExtensionRequest *request, const proto::ext::oauth::Request &req) {
  auto oauth = context()->services->oauthService();

  auto e = [request](const QString &code) {
    auto res = new proto::ext::extension::Response;
    auto resData = new proto::ext::extension::ResponseData;
    auto oauthRes = new proto::ext::oauth::Response;
    auto authorizeRes = new proto::ext::oauth::AuthorizeResponse;

    res->set_allocated_data(resData);
    resData->set_allocated_oauth(oauthRes);
    oauthRes->set_allocated_authorize(authorizeRes);
    authorizeRes->set_code(code.toStdString());

    request->respond(res);
    delete request;
  };

  switch (req.payload_case()) {
  case proto::ext::oauth::Request::kAuthorize: {
    context()->overlay->setCurrent(new OAuthView(context(), request, req.authorize()));
  }
  default:
    break;
  }
}

proto::ext::extension::Response *ExtensionCommandRuntime::dispatchRequest(ExtensionRequest *request) {
  using Request = proto::ext::extension::RequestData;
  auto &data = request->requestData();

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
  case Request::kOauth:
    handleOAuth(request, data.oauth());
    return nullptr;
  default:
    break;
  }

  return makeErrorResponse("Unhandled top level request");
}

void ExtensionCommandRuntime::handleRequest(ExtensionRequest *request) {
  if (request->sessionId() != m_sessionId) return;

  if (auto res = dispatchRequest(request)) {
    request->respond(res);
    delete request;
  }
}

void ExtensionCommandRuntime::handleCrash(const proto::ext::extension::CrashEventData &crash) {
  qCritical() << "Got crash" << crash.text();
  auto &nav = context()->navigation;

  nav->popToRoot();
  nav->pushView(new ExtensionErrorView(QString::fromStdString(crash.text())));
  nav->setNavigationTitle(QString("%1 - Crash handler").arg(m_command->name()));
  nav->setNavigationIcon(m_command->iconUrl());
}

void ExtensionCommandRuntime::handleEvent(const ExtensionEvent &event) {
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

void ExtensionCommandRuntime::initialize() {
  auto manager = context()->services->extensionManager();

  RelativeAssetResolver::instance()->addPath(m_command->assetPath());

  m_navigation = std::make_unique<ExtensionNavigationController>(m_command, context()->navigation.get(),
                                                                 context()->services->extensionManager());
  m_uiRouter = std::make_unique<UIRequestRouter>(m_navigation.get(), *context()->services->toastService());
  m_storageRouter =
      std::make_unique<StorageRequestRouter>(context()->services->localStorage(), m_command->extensionId());
  m_appRouter = std::make_unique<AppRequestRouter>(*context()->services->appDb());
  m_clipboardRouter = std::make_unique<ClipboardRequestRouter>(*context()->services->clipman());

  connect(manager, &ExtensionManager::extensionRequest, this, &ExtensionCommandRuntime::handleRequest);
  connect(manager, &ExtensionManager::extensionEvent, this, &ExtensionCommandRuntime::handleEvent);
}

void ExtensionCommandRuntime::load(const LaunchProps &props) {
  initialize();

  auto rootItemManager = context()->services->rootItemManager();
  auto preferenceValues =
      rootItemManager->getPreferenceValues(QString("extension.%1").arg(m_command->uniqueId()));
  auto manager = context()->services->extensionManager();

  if (m_command->mode() == CommandModeView) {
    // We push the first view immediately, waiting for the initial render to come
    // in and "hydrate" it.
    m_navigation->pushView();
  }

  auto load = new proto::ext::manager::ManagerLoadCommand;
  auto payload = new proto::ext::manager::RequestData;

  load->set_entrypoint(m_command->manifest().entrypoint);
  load->set_env(proto::ext::manager::CommandEnv::Development);
  if (m_command->mode() == CommandMode::CommandModeView) {
    load->set_mode(proto::ext::manager::CommandMode::View);
  } else {
    load->set_mode(proto::ext::manager::CommandMode::NoView);
  }

  for (const auto &key : preferenceValues.keys()) {
    auto value = preferenceValues.value(key);

    qDebug() << "preference" << key << value;

    load->mutable_preference_values()->insert({key.toStdString(), transformJsonValueToProto(value)});
  }

  load->set_extension_path("");
  payload->set_allocated_load(load);

  auto loadRequest = manager->requestManager(payload);

  connect(loadRequest, &ManagerRequest::finished, this,
          [this, loadRequest](const proto::ext::manager::ResponseData &data) {
            m_sessionId = QString::fromStdString(data.load().session_id());
            m_navigation->setSessionId(m_sessionId);
            loadRequest->deleteLater();
          });
}

void ExtensionCommandRuntime::unload() {
  RelativeAssetResolver::instance()->removePath(m_command->assetPath());

  auto manager = context()->services->extensionManager();

  manager->unloadCommand(m_sessionId);
}

ExtensionCommandRuntime::ExtensionCommandRuntime(const std::shared_ptr<ExtensionCommand> &command)
    : CommandContext(command), m_command(command) {}
