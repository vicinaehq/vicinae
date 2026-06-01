#pragma once
#include <unordered_map>
#include "services/global-shortcuts/abstract-global-shortcut-backend.hpp"

class LinuxInputServer;

/**
 * Global shortcuts backed by the privileged input server (evdev monitoring).
 * Owns the trigger: combos are honored exactly and reported back as structured triggers.
 */
class InputServerGlobalShortcutBackend : public AbstractGlobalShortcutBackend {
  Q_OBJECT

public:
  explicit InputServerGlobalShortcutBackend(LinuxInputServer &inputServer);

  QString id() const override { return "input-server"; }
  bool isActivatable() const override;
  int activationPriority() const override { return 100; }

  bool start() override;
  void bindShortcut(const GlobalShortcutRequest &request) override;
  void unbindShortcut(const QString &id) override;
  std::optional<GlobalShortcutInfo> shortcut(const QString &id) const override;

private:
  void onServerReady();

  LinuxInputServer &m_inputServer;
  std::unordered_map<QString, GlobalShortcutInfo> m_shortcuts;
};
