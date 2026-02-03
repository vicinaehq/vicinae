#include "actions/fallback/fallback-actions.hpp"
#include "manage-fallback-commands.hpp"
#include "service-registry.hpp"

void ManageFallbackAction::execute() {
  auto ui = ServiceRegistry::instance()->UI();
  auto view = new ManageFallbackCommands();

  ui->pushView(view);
}

ManageFallbackAction::ManageFallbackAction()
    : AbstractAction("Manage Fallback Actions", ImageURL::builtin("arrow-counter-clockwise")) {}

// DisableFallbackAction

void DisableFallbackAction::execute() {
  auto manager = ServiceRegistry::instance()->rootItemManager();

  // manager->disableFallback(m_id);
}

DisableFallbackAction::DisableFallbackAction(const QString &id)
    : AbstractAction("Disable fallback", ImageURL::builtin("trash")), m_id(id) {}

// MoveFallbackUpAction

void MoveFallbackUpAction::execute() {
  auto manager = ServiceRegistry::instance()->rootItemManager();
  // int pos = manager->itemMetadata(m_id).fallbackPosition;

  // manager->setFallback(m_id, std::max(0, pos - 1));
}

MoveFallbackUpAction::MoveFallbackUpAction(const QString &id)
    : AbstractAction("Move fallback up", ImageURL::builtin("arrow-up")), m_id(id) {}

// MoveFallbackDownAction

void MoveFallbackDownAction::execute() {
  auto manager = ServiceRegistry::instance()->rootItemManager();
  // int pos = manager->itemMetadata(m_id).fallbackPosition;

  // manager->setFallback(m_id, pos + 1);
}

MoveFallbackDownAction::MoveFallbackDownAction(const QString &id)
    : AbstractAction("Move fallback down", ImageURL::builtin("arrow-down")), m_id(id) {}

// EnableFallbackAction

void EnableFallbackAction::execute() {
  auto manager = ServiceRegistry::instance()->rootItemManager();

  // manager->setFallback(m_id);
}

EnableFallbackAction::EnableFallbackAction(const QString &id)
    : AbstractAction("Enable fallback", ImageURL::builtin("checkmark")), m_id(id) {}
