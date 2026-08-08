#pragma once
#include <atomic>
#include <cstdint>
#include <expected>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>
#include <QTimer>
#include "services/global-shortcuts/abstract-global-shortcut-backend.hpp"

// Global shortcuts on macOS via a CGEventTap. Character keys are matched by translating the incoming
// keycode through the active layout, so shortcuts follow the character across layouts. Without the
// accessibility permission, a Carbon RegisterEventHotKey fallback is used instead. CF/CG types stay
// out of this header (opaque void *) so plain C++ translation units can include it.
class MacOSGlobalShortcutBackend : public AbstractGlobalShortcutBackend {
  Q_OBJECT

public:
  MacOSGlobalShortcutBackend();
  ~MacOSGlobalShortcutBackend() override;

  QString id() const override { return "event-tap"; }

  bool start() override;
  std::expected<void, QString> bindShortcut(const GlobalShortcutRequest &request) override;
  void unbindShortcut(const QString &id) override;
  void unbindAll() override;

  // called from the tap thread
  bool handleKeyDown(uint32_t keycode, uint64_t flags, bool autorepeat, quint64 timestamp);
  void reenableTap();

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

  void startTapThread();
  void stopTapThread();
  void ensureTapRunning();
  void runTap();

  void syncPermissionState();
  bool startCarbonHandler();
  void teardownCarbon();
  std::expected<void, QString> registerCarbonHotKey(Binding &binding);
  void unregisterCarbonHotKey(Binding &binding);
  std::optional<uint32_t> resolveCarbonKeycode(const Binding &binding) const;

  std::vector<Binding> m_bindings;          // guarded by m_mutex
  const void *m_layoutData = nullptr;       // CFDataRef, guarded by m_mutex
  const void *m_qwertyLayoutData = nullptr; // CFDataRef, guarded by m_mutex
  uint8_t m_kbdType = 0;                    // guarded by m_mutex
  mutable std::mutex m_mutex;

  std::thread m_thread;
  std::atomic<void *> m_runLoop = nullptr;
  std::atomic<bool> m_tapThreadDone = false;
  void *m_tap = nullptr; // CFMachPortRef, tap thread only

  void *m_hotKeyHandler = nullptr; // EventHandlerRef, main thread only
  uint32_t m_nextCarbonId = 1;

  QTimer m_permissionTimer;
  bool m_trusted = false;
  bool m_started = false;
};
