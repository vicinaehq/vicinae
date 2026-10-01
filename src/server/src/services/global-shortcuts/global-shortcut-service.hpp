#pragma once
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <unordered_map>
#include <QObject>
#include <QPointer>
#include "services/global-shortcuts/abstract-global-shortcut-backend.hpp"

namespace config {
class Manager;
}

class AppRuntime;
class GlobalShortcutService;

class GlobalShortcutHandle {
public:
  GlobalShortcutHandle() = default;
  GlobalShortcutHandle(const GlobalShortcutHandle &) = delete;
  GlobalShortcutHandle &operator=(const GlobalShortcutHandle &) = delete;
  GlobalShortcutHandle(GlobalShortcutHandle &&other) noexcept;
  GlobalShortcutHandle &operator=(GlobalShortcutHandle &&other) noexcept;
  ~GlobalShortcutHandle();

  void reset();

private:
  friend class GlobalShortcutService;

  GlobalShortcutHandle(GlobalShortcutService *service, QString id, std::uint64_t serial);

  QPointer<GlobalShortcutService> m_service;
  QString m_id;
  std::uint64_t m_serial = 0;
};

// Registry of desired bindings projected onto the platform backend.
class GlobalShortcutService : public QObject {
  Q_OBJECT

public:
  using Handler = std::function<void()>;

  struct Binding {
    Keyboard::Shortcut trigger;
    QString description;
    Handler onActivated;
    Handler onReleased;
  };

  GlobalShortcutService(config::Manager &config, AppRuntime &appRuntime,
                        std::unique_ptr<AbstractGlobalShortcutBackend> backend);

  AbstractGlobalShortcutBackend *backend() const { return m_backend.get(); }
  bool isSupported() const { return m_backend && m_backend->isSupported(); }

  [[nodiscard]] GlobalShortcutHandle bind(const QString &id, Binding binding);

  // Description of the registered binding already using `shortcut` (excluding `excludeId`), or nullopt.
  // Always nullopt when the backend is unsupported (inert shortcuts can't actually conflict).
  std::optional<QString> findConflict(const Keyboard::Shortcut &shortcut, const QString &excludeId) const;

  std::optional<QString> probeBind(const Keyboard::Shortcut &shortcut);

  // Suspends all global binds while the in-app recorder captures (so it doesn't hijack the keystroke),
  // then replays them on release.
  void setCapturing(bool capturing);

private:
  friend class GlobalShortcutHandle;

  struct Entry {
    Binding binding;
    std::uint64_t serial;
    bool held = false;
  };

  void release(const QString &id, std::uint64_t serial);
  void reconcile();
  void unbindAll();
  void onActivated(const QString &id);
  void onReleased(const QString &id);
  void releaseHeld();
  void updateInhibition();
  bool computeInhibited() const;

  config::Manager &m_config;
  AppRuntime &m_appRuntime;
  std::unique_ptr<AbstractGlobalShortcutBackend> m_backend;
  std::unordered_map<QString, Entry> m_bindings;
  std::unordered_map<QString, Keyboard::Shortcut> m_applied;
  std::uint64_t m_nextSerial = 1;
  bool m_capturing = false;
  bool m_inhibited = false;
};
