#pragma once
#include "common.hpp"
#include "extension/extension-command.hpp"
#include "generated/tsapi.hpp"
#include "navigation-controller.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "settings-controller/settings-controller.hpp"
#include "root-search/extensions/extension-root-provider.hpp"
#include "command.hpp"
#include <qlogging.h>

namespace {
LaunchProps transformApiLaunchProps(const tsapi::LaunchCommandOptions &options) {
  LaunchProps props;

  if (auto t = options.fallbackText) props.fallbackText = QString::fromStdString(t.value());
  if (auto args = options.arguments; args && args->is_object()) {
    for (const auto &[k, v] : args->get_object()) {
      if (v.is_string()) {
        props.arguments.push_back({QString::fromStdString(k), QString::fromStdString(v.get_string())});
      }
    }
  }

  return props;
}
}; // namespace

class ExtCommandService : public tsapi::AbstractCommand {
  using Void = tsapi::Result<void>;

public:
  ExtCommandService(tsapi::RpcTransport &transport, const std::shared_ptr<ExtensionCommand> &command,
                    RootItemManager *rootManager, SettingsController &settings, NavigationController &nav)
      : AbstractCommand(transport), m_command(command), m_rootManager(rootManager), m_nav(nav),
        m_settings(settings) {}

  tsapi::Result<void>::Future launchCommand(tsapi::LaunchCommandOptions options) override {
    qDebug() << "extension launching another command with name" << options.name;

    for (auto item : m_rootManager->extensions()) {
      auto &repo = item->repository();

      if (options.extensionName == repo->name() &&
          options.ownerOrAuthorName == item->repository()->author()) {
        for (const auto &cmd : repo->commands()) {
          if (cmd->commandId() == options.name) {
            m_nav.activateEntrypoint(cmd->uniqueId(), {.props = transformApiLaunchProps(options)});
          }
        }
      }
    }

    return Void::fail("No such command");
  }

  tsapi::Result<void>::Future openExtensionPreferences() override {
    // for now both behave the same, we may want to scroll to preferences section in the future for this one
    return openCommandPreferences();
  }

  tsapi::Result<void>::Future openCommandPreferences() override {
    m_settings.openExtensionPreferences(m_command->uniqueId());
    return Void::ok();
  }

  Void::Future updateCommandMetadata(tsapi::UpdateCommandMetadataPayload payload) override {
    if (payload.subtitle && !payload.subtitle->empty()) {
      m_command->setSubtitleOverride(QString::fromStdString(*payload.subtitle));
    } else {
      m_command->setSubtitleOverride(std::nullopt);
    }

    if (m_rootManager) emit m_rootManager->itemsChanged();

    return Void::ok();
  }

private:
  std::shared_ptr<ExtensionCommand> m_command;
  RootItemManager *m_rootManager;
  NavigationController &m_nav;
  SettingsController &m_settings;
};
