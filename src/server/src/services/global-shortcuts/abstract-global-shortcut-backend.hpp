#pragma once
#include <optional>
#include <QObject>
#include "keyboard/keyboard.hpp"

struct GlobalShortcutRequest {
  QString id;
  QString description;
  std::optional<Keyboard::Shortcut> preferredTrigger;
};

enum class GlobalShortcutStatus { Bound, Unbound, Failed };

struct GlobalShortcutInfo {
  QString id;
  GlobalShortcutStatus status = GlobalShortcutStatus::Unbound;
  std::optional<Keyboard::Shortcut> trigger;
  QString triggerDisplay;
  QString error;
};

/**
 * Binds global shortcuts and reports their activation, abstracting the platform mechanism.
 *
 * Uniform "request then observe" contract: callers request a binding with a preferred trigger and
 * observe the resolved state through `shortcut()` / `shortcutsChanged()`. Backends that own the
 * trigger (input server, X11, win, mac) always honor the request and report a structured `trigger`.
 * Backends where the system owns the binding (XDG portal) treat it as a hint and may only report a
 * human-readable `triggerDisplay`. Callers never branch on which backend is in use.
 */
class AbstractGlobalShortcutBackend : public QObject {
  Q_OBJECT

signals:
  void shortcutActivated(const QString &id, quint64 timestamp);
  void shortcutsChanged();
  void ready();

public:
  ~AbstractGlobalShortcutBackend() override = default;

  virtual QString id() const = 0;

  /// Whether this backend actually provides global shortcuts. The dummy fallback returns false, so
  /// callers (and the UI) can tell when global shortcuts are unsupported in the current environment.
  virtual bool isSupported() const { return true; }

  /// Whether this backend is suitable in the current environment. Make it as specific as possible.
  virtual bool isActivatable() const = 0;

  /// If several backends are activatable, the highest priority one is selected.
  virtual int activationPriority() const { return 1; }

  /// Begin setup. May complete asynchronously; `ready()` is emitted once usable. Binds issued
  /// before readiness are queued internally and applied once ready.
  virtual bool start() = 0;

  virtual void bindShortcut(const GlobalShortcutRequest &request) = 0;
  virtual void unbindShortcut(const QString &id) = 0;

  /// Current cached state for a shortcut. Never blocks; reads the backend's local view.
  virtual std::optional<GlobalShortcutInfo> shortcut(const QString &id) const = 0;
};
