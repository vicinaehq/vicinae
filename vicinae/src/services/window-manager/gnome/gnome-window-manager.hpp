#pragma once
#include "services/window-manager/abstract-window-manager.hpp"
#include "services/app-service/abstract-app-db.hpp"
#include "gnome-window.hpp"
#include "gnome-listener.hpp"
#include <QDBusInterface>
#include <QJsonObject>
#include <QJsonArray>
#include <memory>

/**
 * Window manager implementation for GNOME Shell.
 * Communicates with the Vicinae GNOME extension via D-Bus to manage windows.
 */
class GnomeWindowManager : public AbstractWindowManager {
private:
  static constexpr const char *DBUS_SERVICE = "org.gnome.Shell";
  static constexpr const char *DBUS_PATH = "/org/gnome/Shell/Extensions/Windows";
  static constexpr const char *DBUS_INTERFACE = "org.gnome.Shell.Extensions.Windows";

  mutable std::unique_ptr<QDBusInterface> m_dbusInterface;
  std::unique_ptr<Gnome::EventListener> m_eventListener;

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

  bool hasWorkspaces() const override { return true; }
  WorkspaceList listWorkspaces() const override;
  std::shared_ptr<AbstractWorkspace> getActiveWorkspace() const override;

  bool isActivatable() const override;
  bool ping() const override;
  void start() override;

  // GNOME-specific capabilities
  bool supportsInputForwarding() const override { return false; } // Not implemented yet
  bool sendShortcutSync(const AbstractWindow &window, const KeyboardShortcut &shortcut) override {
    return false;
  }

  /**
   * Get detailed information for a specific window
   */
  std::shared_ptr<GnomeWindow> getWindowDetails(uint32_t windowId) const;

  /**
   * GNOME-specific window finding with .desktop suffix handling
   */
  WindowList findAppWindowsGnome(const AbstractApplication &app) const;
  WindowList findWindowByClassGnome(const QString &wmClass) const;

private:
  /**
   * Helper function to match wm_class with .desktop suffix variations
   */
  bool matchWmClassWithDesktopSuffix(const QString &windowWmClass, const QString &targetClass) const;
};
