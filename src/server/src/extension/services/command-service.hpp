#pragma once
#include "extension/extension-command.hpp"
#include "generated/tsapi.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "settings-controller/settings-controller.hpp"

class ExtCommandService : public tsapi::AbstractCommand {
  using Void = tsapi::Result<void>;

public:
  ExtCommandService(tsapi::RpcTransport &transport, const std::shared_ptr<ExtensionCommand> &command,
                    RootItemManager *rootManager, SettingsController &settings)
      : AbstractCommand(transport), m_command(command), m_rootManager(rootManager), m_settings(settings) {}

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
  SettingsController &m_settings;
};
