#include "extension-command-runtime.hpp"
#include "services/asset-resolver/asset-resolver.hpp"
#include <QString>

proto::ext::extension::Response *ExtensionCommandRuntime::makeErrorResponse(const QString &errorText) {
  auto res = new proto::ext::extension::Response;
  auto err = new proto::ext::common::ErrorResponse;

  err->set_error_text(errorText.toStdString());
  res->set_allocated_error(err);

  return res;
}

proto::ext::extension::Response *ExtensionCommandRuntime::dispatchRequest(const ExtensionRequest &request) {
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

void ExtensionCommandRuntime::handleRequest(ExtensionRequest &request) {
  if (request.sessionId() != m_sessionId) return;

  request.respond(dispatchRequest(request));
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

void ExtensionCommandRuntime::unload() {
  RelativeAssetResolver::instance()->removePath(m_command->assetPath());

  auto manager = context()->services->extensionManager();

  manager->unloadCommand(m_sessionId);
}

ExtensionCommandRuntime::ExtensionCommandRuntime(const std::shared_ptr<ExtensionCommand> &command)
    : CommandContext(command), m_command(command) {}
