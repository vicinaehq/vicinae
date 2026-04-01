#pragma once
#include "extension/extension-navigation-controller.hpp"
#include "generated/tsapi.hpp"
#include "services/root-item-manager/root-item-manager.hpp"

class ExtCommandService : public tsapi::AbstractCommand {
  using Void = tsapi::Result<void>;

public:
  ExtCommandService(tsapi::RpcTransport &transport, ExtensionNavigationController *navigation,
                    RootItemManager *rootManager)
      : AbstractCommand(transport), m_navigation(navigation), m_rootManager(rootManager) {}

  Void::Future updateCommandMetadata(const tsapi::UpdateCommandMetadataPayload &payload) override {
    if (payload.subtitle && !payload.subtitle->empty()) {
      m_navigation->setSubtitleOverride(QString::fromStdString(*payload.subtitle));
    } else {
      m_navigation->setSubtitleOverride(std::nullopt);
    }

    if (m_rootManager) emit m_rootManager->itemsChanged();

    return Void::ok();
  }

private:
  ExtensionNavigationController *m_navigation;
  RootItemManager *m_rootManager;
};
