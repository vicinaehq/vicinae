#include "extension-command-runtime.hpp"
#include "common.hpp"
#include "glaze-qt.hpp"
#include "service-registry.hpp"
#include "services/asset-resolver/asset-resolver.hpp"
#include <QString>
#include <ranges>
#include "services/root-item-manager/root-item-manager.hpp"
#include "extension/manager/extension-manager.hpp"
#include "vicinae.hpp"
#include "generated/manager.hpp"

/*
void ExtensionCommandRuntime::handleCrash(const proto::ext::extension::CrashEventData &crash) {
  qCritical() << "Got crash" << crash.text().c_str();
  auto &nav = context()->navigation;

  nav->popToRoot();
  nav->pushView(new ExtensionErrorViewHost(QString::fromStdString(crash.text())));
  nav->setNavigationTitle(QString("%1 - Crash handler").arg(m_command->name()));
  nav->setNavigationIcon(m_command->iconUrl());
}

void ExtensionCommandRuntime::handleEvent(const ExtensionEvent &event) {
  using Event = proto::ext::extension::Event;

  switch (event.data()->payload_case()) {
  case Event::kCrash: {
    handleCrash(event.data()->crash());
    return;
  }
  case Event::kGeneric: {
    handleGenericEvent(event.data()->generic());
    return;
  }
  default:
    break;
  }
}
*/

void ExtensionCommandRuntime::initialize() {
  auto manager = context()->services->extensionManager();

  RelativeAssetResolver::instance()->addPath(m_command->assetPath());

  m_isDevMode = manager->hasDevelopmentSession(m_command->extensionId());

  QString const storageNamespace = QString("%1:data").arg(m_command->uniqueId().provider.c_str());
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
    // context()->navigation->pushView();
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

  manager->client().manager()->load(opts);
}

void ExtensionCommandRuntime::unload() {
  RelativeAssetResolver::instance()->removePath(m_command->assetPath());
  auto manager = context()->services->extensionManager();
  auto toast = context()->services->toastService();

  manager->client().manager()->unload(m_sessionId.toStdString());

  context()->navigation->setNavigationSuffixIcon(std::nullopt);
  // manager->unloadCommand(m_sessionId);
  // toast->clear();
}

ExtensionCommandRuntime::ExtensionCommandRuntime(const std::shared_ptr<ExtensionCommand> &command)
    : CommandContext(command), m_command(command) {}
