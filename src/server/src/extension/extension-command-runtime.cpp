#include "extension-command-runtime.hpp"
#include "common.hpp"
#include "extension/requests/file-search-request-router.hpp"
#include "generated/manager.hpp"
#include "proto/manager.pb.h"
#include "service-registry.hpp"
#include "services/asset-resolver/asset-resolver.hpp"
#include <QString>
#include <qurlquery.h>
#include "services/root-item-manager/root-item-manager.hpp"
#include "utils/utils.hpp"
#include "extension/manager/extension-manager.hpp"
#include "vicinae.hpp"

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

  if (m_command->mode() == CommandModeView) {
    // We push the first view immediately, waiting for the initial render to come
    // in and "hydrate" it.
    // context()->navigation->pushView();
  }

  auto resolveCommandEnv = [&]() {
    using namespace proto::ext::manager;
    return m_isDevMode ? CommandEnv::Development : CommandEnv::Production;
  };

  if (m_isDevMode) {
    context()->navigation->setNavigationSuffixIcon(ImageURL::builtin("hammer").setFill(SemanticColor::Green));
  }

  auto load = new proto::ext::manager::ManagerLoadCommand;

  manager::LoadOptions opts;

  opts.entrypoint = m_command->manifest().entrypoint;
  opts.env = ""; // to fill
  opts.extension_id = m_command->extensionId().toStdString();
  opts.vicinae_path = Omnicast::dataDir();
  opts.command_name = m_command->commandId().toStdString();
  opts.extension_name = m_command->repositoryName().toStdString();
  opts.owner_or_author_name = m_command->author().toStdString();
  opts.is_raycast = m_command->isRaycast();

  if (m_command->mode() == CommandMode::CommandModeView) {
    load->set_mode(proto::ext::manager::CommandMode::View);
  } else {
    load->set_mode(proto::ext::manager::CommandMode::NoView);
  }

  auto preferences = load->mutable_preference_values();

  for (const auto &key : preferenceValues.keys()) {
    auto value = preferenceValues.value(key);

    preferences->insert({key.toStdString(), transformJsonValueToProto(value)});
  }

  auto arguments = load->mutable_argument_values();
  for (const auto &[key, value] : props.arguments) {
    arguments->insert({key.toStdString(), transformJsonValueToProto(value)});
  }

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
