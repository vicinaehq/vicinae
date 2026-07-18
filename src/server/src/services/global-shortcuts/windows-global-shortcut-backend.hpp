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

  std::mutex m_targetsMutex;
  std::vector<HookTarget> m_targets;
  std::thread m_hookThread;
  unsigned long m_hookThreadId = 0;
  int m_nextRegistrationId = 1;
  bool m_started = false;
};
