#include "shortcut-conflict.hpp"
#include <QCoreApplication>
#include "keyboard/keybind-manager.hpp"
#include "keyboard/keyboard.hpp"
#include "service-registry.hpp"
#include "services/global-shortcuts/global-shortcut-service.hpp"

QString shortcut_conflict::validate(const Keyboard::Shortcut &shortcut, const QString &excludeId) {
  if (!shortcut.hasMods() && !shortcut.isFunctionKey()) {
    return QCoreApplication::translate("shortcut-conflict", "Modifier required");
  }

  if (auto bound = KeybindManager::instance()->findBoundInfo(shortcut, excludeId)) {
    return QCoreApplication::translate("shortcut-conflict", "Already bound to \"%1\"").arg(bound->name);
  }

  if (auto *service = ServiceRegistry::instance()->globalShortcuts()) {
    if (auto name = service->findConflict(shortcut, excludeId)) {
      return QCoreApplication::translate("shortcut-conflict", "Already bound to \"%1\"").arg(*name);
    }
  }

  return {};
}
