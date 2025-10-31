#pragma once
#include "proto/command.pb.h"
#include "proto/extension.pb.h"
#include "extension/extension-navigation-controller.hpp"
#include "services/root-item-manager/root-item-manager.hpp"

class CommandRequestRouter {

  ExtensionNavigationController *m_navigation = nullptr;
  RootItemManager *m_rootManager = nullptr;

  proto::ext::extension::Response *wrap(proto::ext::command::Response *cmdRes);

public:
  CommandRequestRouter(ExtensionNavigationController *navigation, RootItemManager *rootManager)
      : m_navigation(navigation), m_rootManager(rootManager) {}

  proto::ext::extension::Response *route(const proto::ext::command::Request &req);
};
