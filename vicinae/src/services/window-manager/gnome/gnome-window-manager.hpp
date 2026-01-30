#pragma once
#include "gnome-listener.hpp"
#include "gnome-window.hpp"
#include "lib/keyboard/keyboard.hpp"
#include "services/app-service/abstract-app-db.hpp"
#include "services/window-manager/abstract-window-manager.hpp"
#include <QDBusInterface>
#include <QJsonArray>
#include <QJsonObject>
#include <QTimer>
#include <memory>

/**
 * Window manager implementation for GNOME Shell.
 * Communicates with the Vicinae GNOME extension via D-Bus to manage windows.
 */
class GnomeWindowManager : public AbstractWindowManager {
  Q_OBJECT

signals:
  void windowFocused(const QString &windowId);

private:
  static constexpr const char *DBUS_SERVICE = "org.gnome.Shell";
  static constexpr const char *DBUS_PATH = "/org/gnome/Shell/Extensions/Windows";
  static constexpr const char *DBUS_INTERFACE = "org.gnome.Shell.Extensions.Windows";

  mutable std::unique_ptr<QDBusInterface> m_dbusInterface;
  std::unique_ptr<Gnome::EventListener> m_eventListener;
  QTimer m_debounceTimer;
  bool m_pendingWindowsChanged = false;

  /**
   * Get or create the D-Bus interface
   */
  QDBusInterface *getDBusInterface() const;

  /**
   * Execute a D-Bus method call and return the response
   */
  QString callDBusMethod(const QString &method, const QVariantList &args = {}) const;

  /**
   * Execute a D-Bus method call that returns void
   */
  bool callDBusMethodVoid(const QString &method, const QVariantList &args = {}) const;

  /**
   * Parse JSON string response from D-Bus
   */
  QJsonObject parseJsonResponse(const QString &response) const;
  QJsonArray parseJsonArrayResponse(const QString &response) const;

  /**
   * Map simple Flatpak IDs to complex path-based IDs for app database matching
   */
  QString mapToComplexId(const QString &simpleId) const;

public:
  GnomeWindowManager();
  ~GnomeWindowManager() override;

  // AbstractWindowManager interface implementation
  QString id() const override { return "gnome"; }
  QString displayName() const override { return "GNOME Shell"; }

  WindowList listWindowsSync() const override;
  std::shared_ptr<AbstractWindow> getFocusedWindowSync() const override;
  void focusWindowSync(const AbstractWindow &window) const override;
  bool closeWindow(const AbstractWindow &window) const override;

  bool pasteToWindow(const AbstractWindow *window, const AbstractApplication *app) override;

  bool hasWorkspaces() const override { return true; }
  WorkspaceList listWorkspaces() const override;
  std::shared_ptr<AbstractWorkspace> getActiveWorkspace() const override;

  bool isActivatable() const override;
  bool ping() const override;
  void start() override;

  // GNOME-specific capabilities
  bool supportsPaste() const override { return true; } // Now implemented

  /**
   * Send a keyboard shortcut to a specific window
   */
  bool sendShortcutSync(const AbstractWindow &window, const Keyboard::Shortcut &shortcut);

  /**
   * Get detailed information for a specific window
   */
  std::shared_ptr<GnomeWindow> getWindowDetails(uint32_t windowId) const;

  /**
   * GNOME-specific window finding with .desktop suffix handling
   */
  WindowList findAppWindowsGnome(const AbstractApplication &app) const;
  WindowList findWindowByClassGnome(const QString &wmClass) const;
};
