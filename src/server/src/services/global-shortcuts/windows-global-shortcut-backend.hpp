#pragma once
#include <expected>
#include <mutex>
#include <thread>
#include <vector>
#include "services/global-shortcuts/abstract-global-shortcut-backend.hpp"

class WindowsGlobalShortcutBackend : public AbstractGlobalShortcutBackend {
  Q_OBJECT

public:
  WindowsGlobalShortcutBackend() = default;
  ~WindowsGlobalShortcutBackend() override;

  QString id() const override { return "win32-hotkey"; }

  bool start() override;
  std::expected<void, QString> bindShortcut(const GlobalShortcutRequest &request) override;
  void unbindShortcut(const QString &id) override;
  void unbindAll() override;
  void setCapturing(bool capturing) override;

  bool dispatchKey(unsigned int vk, unsigned int mods, bool down);

  static void suppressNextKeyUp(unsigned int vk);

private:
  struct HookTarget {
    unsigned int vk;
    unsigned int mods;
    int regId;
    QString id;
    bool down;
  };

  bool dispatchModifier(unsigned int mods, bool down);
  void activate(const QString &id);

  std::mutex m_targetsMutex;
  std::vector<HookTarget> m_targets;
  std::thread m_hookThread;
  unsigned long m_hookThreadId = 0;
  int m_nextRegistrationId = 1;
  unsigned int m_chord = 0;
  bool m_capturing = false;
  bool m_started = false;
};
