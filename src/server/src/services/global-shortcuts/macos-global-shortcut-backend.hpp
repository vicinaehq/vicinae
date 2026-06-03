#pragma once
#include <cstdint>
#include <optional>
#include <unordered_map>
#include "services/global-shortcuts/abstract-global-shortcut-backend.hpp"

// Global shortcuts on macOS via Carbon RegisterEventHotKey. Carbon types stay out of this header
// (opaque void *) so plain C++ translation units can include it.
class MacOSGlobalShortcutBackend : public AbstractGlobalShortcutBackend {
  Q_OBJECT

public:
  MacOSGlobalShortcutBackend();
  ~MacOSGlobalShortcutBackend() override;

  QString id() const override { return "carbon"; }
  bool isActivatable() const override { return true; }
  int activationPriority() const override { return 100; }

  bool start() override;
  void bindShortcut(const GlobalShortcutRequest &request) override;
  void unbindShortcut(const QString &id) override;
  void unbindAll() override;
  std::optional<GlobalShortcutInfo> shortcut(const QString &id) const override;

  void handleHotKey(uint32_t carbonId, quint64 timestamp);

private:
  struct Binding {
    GlobalShortcutInfo info;
    void *ref = nullptr; // EventHotKeyRef
    uint32_t carbonId = 0;
  };

  std::unordered_map<QString, Binding> m_bindings;
  std::unordered_map<uint32_t, QString> m_idByCarbonId;
  void *m_handler = nullptr; // EventHandlerRef
  uint32_t m_nextCarbonId = 1;
  bool m_started = false;
};
