#pragma once

// Messy class whichs role is to keep the command db in sync with the root item manager
// Eventually we will refactor most of this to have less moving parts.

#include "root-search/extensions/extension-root-provider.hpp"
#include "omni-command-db.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include <qobject.h>

class RootExtensionManager : public QObject {
  RootItemManager &m_manager;
  OmniCommandDatabase &m_commandDb;

public:
  void start() {
    connect(&m_commandDb, &OmniCommandDatabase::registryAdded, this, [this](const auto &registry) {
      m_manager.addProvider(std::make_unique<ExtensionRootProvider>(registry));
    });
    connect(&m_commandDb, &OmniCommandDatabase::repositoryRemoved, this,
            [this](const QString &id) { m_manager.removeProvider(QString("extension.%1").arg(id)); });
  }

  RootExtensionManager(RootItemManager &manager, OmniCommandDatabase &commandDb)
      : m_manager(manager), m_commandDb(commandDb) {}
};
