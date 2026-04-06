#include "extension-command-runtime.hpp"
#include "common.hpp"
#include "common/context.hpp"
#include "extension-error-view-host.hpp"
#include "extension/services/application-service.hpp"
#include "extension/services/clipboard-service.hpp"
#include "extension/services/command-service.hpp"
#include "extension/services/event-core-service.hpp"
#include "extension/services/file-search-service.hpp"
#include "extension/services/oauth-service.hpp"
#include "extension/services/storage-service.hpp"
#include "extension/services/ui-service.hpp"
#include "extension/services/wm-service.hpp"
#include "extension/services/ai-service.hpp"
#include "generated/tsapi.hpp"
#include "glaze-qt.hpp"
#include "service-registry.hpp"
#include "services/asset-resolver/asset-resolver.hpp"
#include <QString>
#include <glaze/json/generic.hpp>
#include <qfuturewatcher.h>
#include <qlogging.h>
#include <ranges>
#include "services/root-item-manager/root-item-manager.hpp"
#include "extension/manager/extension-manager.hpp"
#include "vicinae.hpp"
#include "generated/manager.hpp"

void ExtensionCommandRuntime::initialize() {
  auto manager = context()->services->extensionManager();

  m_bus = std::make_unique<ExtensionManagerBus>(*manager);
  m_logger = std::make_unique<ExtensionLogger>();
  m_transport = std::make_unique<tsapi::RpcTransport>(*m_bus);

  RelativeAssetResolver::instance()->addPath(m_command->assetPath());

  m_isDevMode = manager->hasDevelopmentSession(m_command->extensionId());

  QString const storageNamespace = QString("%1:data").arg(m_command->uniqueId().provider.c_str());

  auto services = context()->services;
  auto &ctx = *context();

  auto *eventCore = new ExtEventCoreService(*m_transport);
  auto *app = new ExtApplicationService(*m_transport, *services->appDb());
  auto *ui = new ExtUIService(*m_transport, context()->navigation.get(), m_command, eventCore,
                              *services->toastService());
  auto *wm = new ExtWindowManagementService(*m_transport, *services->windowManager(), *services->appDb());
  auto *clipboard = new ExtClipboardService(*m_transport, *services->clipman(), *services->pasteService());
  auto *storage = new ExtStorageService(*m_transport, *services->localStorage(), storageNamespace);
  auto *fileSearch = new ExtFileSearchService(*m_transport, *services->fileService());
  auto *command = new ExtCommandService(*m_transport, m_command, services->rootItemManager(), *ctx.settings);
  auto *oauth = new ExtOAuthService(*m_transport, m_command->extensionId(), ctx);
  auto *ai = new ExtensionAIRouter(*m_transport, *services->ai());

  m_server = new tsapi::Server(*m_transport, app, ui, wm, clipboard, storage, fileSearch, command, oauth,
                               eventCore, ai);
  m_server->setLogger(m_logger.get());
  m_server->setParent(this);
}

void ExtensionCommandRuntime::load(const LaunchProps &props) {
  initialize();

  auto rootItemManager = context()->services->rootItemManager();
  auto preferenceValues = rootItemManager->getPreferenceValues(m_command->uniqueId());
  auto manager = context()->services->extensionManager();
  manager::LoadOptions opts;

  if (m_command->mode() == CommandModeView) {
    // We push the first view immediately, waiting for the initial render to come
    // in and "hydrate" it.
    m_server->UI()->pushView();
  }

  if (m_isDevMode) {
    context()->navigation->setNavigationSuffixIcon(ImageURL::builtin("hammer").setFill(SemanticColor::Green));
    opts.env = manager::CommandEnv::Development;
  } else {
    opts.env = manager::CommandEnv::Production;
  }

  opts.entrypoint = m_command->manifest().entrypoint;
  opts.mode = m_command->mode() == CommandMode::CommandModeView ? manager::CommandMode::View
                                                                : manager::CommandMode::NoView;
  opts.extension_id = m_command->extensionId().toStdString();
  opts.vicinae_path = Omnicast::dataDir();
  opts.command_name = m_command->commandId().toStdString();
  opts.extension_name = m_command->repositoryName().toStdString();
  opts.owner_or_author_name = m_command->author().toStdString();
  opts.is_raycast = m_command->isRaycast();
  opts.preferences = qJsonObjectToGlazeGeneric(preferenceValues);
  opts.arguments = props.arguments |
                   std::views::transform([](auto &&pair) -> std::pair<std::string, std::string> {
                     return {pair.first.toStdString(), pair.second.toStdString()};
                   }) |
                   std::ranges::to<std::unordered_map<std::string, std::string>>();

  auto watcher = new QFutureWatcher<std::expected<manager::LoadResponse, std::string>>(this);

  connect(manager, &ExtensionManager::extensionMessageReceived, this,
          [this](const std::string &sessionId, std::string_view data) {
            if (sessionId != m_sessionId) return; // not for us
            m_server->route(data);
          });

  connect(manager, &ExtensionManager::extensionCrashed, this,
          [this](const std::string &sessionId, const std::string &reason) {
            if (sessionId != m_sessionId) return;

            qCritical() << "Got crash" << reason;
            auto &nav = context()->navigation;

            nav->popToRoot();
            nav->pushView(new ExtensionErrorViewHost(QString::fromStdString(reason)));
            nav->setNavigationTitle(QString("%1 - Crash handler").arg(m_command->name()));
            nav->setNavigationIcon(m_command->iconUrl());
          });

  connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher]() {
    if (!watcher->isCanceled()) {
      auto res = watcher->result();

      if (!res) {
        qWarning() << "Failed to load extension" << res.error();
      } else {
        m_sessionId = res->session_id;
        m_bus->setSessionId(m_sessionId);
      }
    }

    watcher->deleteLater();
  });

  watcher->setFuture(manager->client().manager()->load(opts));
}

void ExtensionCommandRuntime::unload() {
  RelativeAssetResolver::instance()->removePath(m_command->assetPath());
  auto manager = context()->services->extensionManager();

  manager->client().manager()->unload(m_sessionId);

  context()->navigation->setNavigationSuffixIcon(std::nullopt);
}

ExtensionCommandRuntime::ExtensionCommandRuntime(const std::shared_ptr<ExtensionCommand> &command)
    : CommandContext(command), m_command(command) {}
