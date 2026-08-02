#pragma once
#include <expected>
#include <functional>
#include <optional>
#include <utility>
#include <QObject>
#include "keyboard/keyboard.hpp"

struct GlobalShortcutRequest {
  QString id;
  Keyboard::Shortcut trigger;
  QString description;
};

/**
 * Binds global shortcuts and reports their activation, abstracting the platform mechanism.
 */
class AbstractGlobalShortcutBackend : public QObject {
  Q_OBJECT

signals:
  void shortcutActivated(const QString &id, quint64 timestamp);
  void ready();

public:
  ~AbstractGlobalShortcutBackend() override = default;

  virtual QString id() const = 0;

  /// Whether this backend actually provides global shortcuts. The dummy fallback returns false, so
  /// callers (and the UI) can tell when global shortcuts are unsupported in the current environment.
  virtual bool isSupported() const { return true; }

  /// Begin setup, `ready` should be emitted once done.
  virtual bool start() = 0;

  /// Synchronously bind the shortcut, or return a human-readable reason if the platform rejected it.
  virtual std::expected<void, QString> bindShortcut(const GlobalShortcutRequest &request) = 0;
  virtual void unbindShortcut(const QString &id) = 0;
  virtual void unbindAll() = 0;

  /**
   * Install a predicate consulted before acting on a shortcut activation. Must be safe to call from
   * any thread (implementations may be invoked from a platform event-tap thread). Returning true
   * means the activation should be suppressed: backends that can avoid consuming the physical
   * keystroke (e.g. a non-exclusive macOS CGEventTap) should let it propagate to the focused app
   * instead of acting on it.
   */
  void setActivationGate(std::function<bool(const QString &id)> gate) { m_gate = std::move(gate); }

protected:
  std::function<bool(const QString &id)> m_gate;
};
