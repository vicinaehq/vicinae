#pragma once
#include "extension/extension-command.hpp"
#include "generated/tsapi.hpp"
#include "services/root-item-manager/root-item-manager.hpp"

class ExtCommandService : public tsapi::AbstractCommand {
  using Void = tsapi::Result<void>;

public:
  ExtCommandService(tsapi::RpcTransport &transport, const std::shared_ptr<ExtensionCommand> &command,
                    RootItemManager *rootManager)
      : AbstractCommand(transport), m_command(command), m_rootManager(rootManager) {}

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
};
