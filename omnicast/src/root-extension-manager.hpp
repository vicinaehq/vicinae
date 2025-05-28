#pragma once

// Messy class whichs role is to keep the command db in sync with the root item manager
// Eventually we will refactor most of this to have less moving parts.

#include "root-search/extensions/extension-root-provider.hpp"
#include "omni-command-db.hpp"
#include "root-item-manager.hpp"
#include <qobject.h>

class RootExtensionManager : public QObject {
  RootItemManager &m_manager;
  OmniCommandDatabase &m_commandDb;

public:
  RootExtensionManager(RootItemManager &manager, OmniCommandDatabase &commandDb)
      : m_manager(manager), m_commandDb(commandDb) {
    connect(&commandDb, &OmniCommandDatabase::registryAdded, this, [this](const auto &registry) {
      m_manager.addProvider(std::make_unique<ExtensionRootProvider>(registry));
    });
  }
};
