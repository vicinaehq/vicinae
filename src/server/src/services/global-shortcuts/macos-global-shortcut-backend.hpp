#pragma once
#include <cstdint>
#include <expected>
#include <optional>
#include <vector>
#include "services/global-shortcuts/abstract-global-shortcut-backend.hpp"

// Global shortcuts on macOS via Carbon RegisterEventHotKey. Hot keys are matched and delivered by
// the system, so they need no permission, keep firing while Secure Input is active, and never
// depend on this process being scheduled (an event tap in a throttled process delays every
// keystroke system-wide). Character keys are resolved to a keycode against the active layout with
// a QWERTY fallback, and re-registered whenever the layout changes, so shortcuts follow the
// character across layouts. Everything runs on the main thread.
class MacOSGlobalShortcutBackend : public AbstractGlobalShortcutBackend {
  Q_OBJECT

public:
  MacOSGlobalShortcutBackend();
  ~MacOSGlobalShortcutBackend() override;

  QString id() const override { return "carbon"; }

  bool start() override;
  std::expected<void, QString> bindShortcut(const GlobalShortcutRequest &request) override;
  void unbindShortcut(const QString &id) override;
  void unbindAll() override;

  // called on the main thread by the Carbon hot key handler
  void handleHotKey(uint32_t carbonId, quint64 timestamp);

  // called on the main thread when the keyboard layout changes
  void refreshLayout();

private:
  struct Binding {
    QString id;
    std::optional<uint32_t> keycode;
    QString character;
    uint64_t flags = 0;
    void *hotKeyRef = nullptr; // EventHotKeyRef
    uint32_t carbonId = 0;
  };

  bool startCarbonHandler();
  void teardownCarbon();
  std::expected<void, QString> registerCarbonHotKey(Binding &binding);
  void unregisterCarbonHotKey(Binding &binding);
  std::optional<uint32_t> resolveCarbonKeycode(const Binding &binding) const;

  std::vector<Binding> m_bindings;
  const void *m_layoutData = nullptr;       // CFDataRef
  const void *m_qwertyLayoutData = nullptr; // CFDataRef
  uint8_t m_kbdType = 0;

  void *m_hotKeyHandler = nullptr; // EventHandlerRef
  uint32_t m_nextCarbonId = 1;
  bool m_started = false;
};
