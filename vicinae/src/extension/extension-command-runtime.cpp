#include "extension-command-runtime.hpp"
#include "common.hpp"
#include "extension/extension-navigation-controller.hpp"
#include "extension-error-view.hpp"
#include "extension/requests/app-request-router.hpp"
#include "extension/requests/clipboard-request-router.hpp"
#include "extension/requests/storage-request-router.hpp"
#include "extension/requests/ui-request-router.hpp"
#include "extension/requests/file-search-request-router.hpp"
#include "extension/requests/wm-router.hpp"
#include "extension/requests/oauth-router.hpp"
#include "extension/requests/command-request-router.hpp"
#include "proto/manager.pb.h"
#include "proto/oauth.pb.h"
#include "common.hpp"
#include "service-registry.hpp"
#include "services/asset-resolver/asset-resolver.hpp"
#include <QString>
#include <exception>
#include <qurlquery.h>
#include "services/root-item-manager/root-item-manager.hpp"
#include "utils/utils.hpp"

proto::ext::extension::Response *ExtensionCommandRuntime::makeErrorResponse(const QString &errorText) {
  auto res = new proto::ext::extension::Response;
  auto err = new proto::ext::common::ErrorResponse;

  err->set_error_text(errorText.toStdString());
  res->set_allocated_error(err);

  return res;
}

PromiseLike<proto::ext::extension::Response *>
ExtensionCommandRuntime::dispatchRequest(ExtensionRequest *request) {
  using Request = proto::ext::extension::RequestData;
  auto &data = request->requestData();

  switch (data.payload_case()) {
  case Request::kUi:
    return m_uiRouter->route(data.ui());
  case Request::kStorage:
    return m_storageRouter->route(data.storage());
  case Request::kApp:
    return m_appRouter->route(data.app());
  case Request::kClipboard:
    return m_clipboardRouter->route(data.clipboard());
  case Request::kFileSearch:
    return m_fileSearchRouter->route(data.file_search());
  case Request::kWm:
    return m_wmRouter->route(data.wm());
  case Request::kCommand:
    return m_commandRouter->route(data.command());
  case Request::kOauth:
    return m_oauthRouter->route(data.oauth());
  default:
    break;
  }

  qDebug() << request->requestData().DebugString();

  return makeErrorResponse("Unhandled top level request");
}

void ExtensionCommandRuntime::handleRequest(ExtensionRequest *req) {
  if (req->sessionId() != m_sessionId) return;

  auto request = std::shared_ptr<ExtensionRequest>(req);

  try {
    auto result = dispatchRequest(request.get());

    if (auto res = std::get_if<proto::ext::extension::Response *>(&result)) {
      if (!*res) {
        request->respondWithError("No handler for this request");
        return;
      }

      request->respond(*res);
      return;
    }

    auto future = std::get<QFuture<proto::ext::extension::Response *>>(result);
    auto watcher = std::shared_ptr<ResponseWatcher>(new ResponseWatcher, QObjectDeleter{});

    watcher->setFuture(future);
    m_pendingFutures.insert({request, watcher});
    connect(watcher.get(), &ResponseWatcher::finished, this, [this, watcher, request]() {
      m_pendingFutures.erase(request);

      if (!watcher->isFinished()) {
        request->respondWithError("Failed to send response");
        return;
      }

      auto res = watcher->result();

      if (!res) {
        request->respondWithError("No handler for this request");
        return;
      }

      request->respond(res);
    });
  } catch (const std::exception &except) { request->respondWithError(except.what()); }
}

void ExtensionCommandRuntime::handleCrash(const proto::ext::extension::CrashEventData &crash) {
  qCritical() << "Got crash" << crash.text().c_str();
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
  m_isDevMode = manager->hasDevelopmentSession(m_command->extensionId());
  m_navigation->setDevMode(m_isDevMode);
  m_uiRouter = std::make_unique<UIRequestRouter>(m_navigation.get(), *context()->services->toastService());
  m_commandRouter =
      std::make_unique<CommandRequestRouter>(m_navigation.get(), context()->services->rootItemManager());
  m_fileSearchRouter = std::make_unique<FileSearchRequestRouter>(*context()->services->fileService());
  m_storageRouter =
      std::make_unique<StorageRequestRouter>(context()->services->localStorage(), m_command->extensionId());
  m_appRouter = std::make_unique<AppRequestRouter>(*context()->services->appDb());
  m_clipboardRouter = std::make_unique<ClipboardRequestRouter>(*context()->services->clipman());
  m_wmRouter = std::make_unique<WindowManagementRouter>(*context()->services->windowManager(),
                                                        *context()->services->appDb());
  m_oauthRouter = std::make_unique<OAuthRouter>(m_command->extensionId(), *context());

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

  auto resolveCommandEnv = [&]() {
    using namespace proto::ext::manager;
    return m_isDevMode ? CommandEnv::Development : CommandEnv::Production;
  };

  if (m_isDevMode) {
    context()->navigation->setNavigationSuffixIcon(ImageURL::builtin("hammer").setFill(SemanticColor::Green));
  }

  auto load = new proto::ext::manager::ManagerLoadCommand;
  auto payload = new proto::ext::manager::RequestData;

  load->set_entrypoint(m_command->manifest().entrypoint);
  load->set_env(resolveCommandEnv());
  load->set_extension_id(m_command->extensionId().toStdString());
  load->set_vicinae_path(Omnicast::dataDir());
  load->set_command_name(m_command->commandId().toStdString());
  load->set_extension_name(m_command->repositoryName().toStdString());
  load->set_owner_or_author_name(m_command->author().toStdString());

  if (m_command->mode() == CommandMode::CommandModeView) {
    load->set_mode(proto::ext::manager::CommandMode::View);
  } else {
    load->set_mode(proto::ext::manager::CommandMode::NoView);
  }

  load->set_is_raycast(m_command->isRaycast());

  auto preferences = load->mutable_preference_values();

  for (const auto &key : preferenceValues.keys()) {
    auto value = preferenceValues.value(key);

    preferences->insert({key.toStdString(), transformJsonValueToProto(value)});
  }

  auto arguments = load->mutable_argument_values();
  for (const auto &[key, value] : props.arguments) {
    arguments->insert({key.toStdString(), transformJsonValueToProto(value)});
  }

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
  auto toast = context()->services->toastService();

  context()->navigation->setNavigationSuffixIcon(std::nullopt);
  manager->unloadCommand(m_sessionId);
  toast->clear();

  for (const auto &[_, watcher] : m_pendingFutures) {
    watcher->cancel();
  }
}

ExtensionCommandRuntime::ExtensionCommandRuntime(const std::shared_ptr<ExtensionCommand> &command)
    : CommandContext(command), m_command(command) {}
