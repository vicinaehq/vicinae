#pragma once
#include "services/clipboard/clipboard-server.hpp"
#include <QtDBus/QtDBus>
#include <QtDBus/qdbusconnection.h>
#include <QtDBus/qdbusinterface.h>
#include <QObject>
#include <QTimer>

class GnomeClipboardServer : public AbstractClipboardServer {
  Q_OBJECT

private:
  QDBusConnection m_bus;
  QDBusInterface *m_interface = nullptr;
  QTimer *m_reconnectTimer = nullptr;
  bool m_isConnected = false;

  // D-Bus interface details
  static constexpr const char *DBUS_SERVICE = "org.gnome.Shell";
  static constexpr const char *DBUS_PATH = "/org/gnome/Shell/Extensions/Clipboard";
  static constexpr const char *DBUS_INTERFACE = "org.gnome.Shell.Extensions.Clipboard";

  // Helper methods
  bool setupDBusConnection();
  void cleanupDBusConnection();
  bool testExtensionAvailability() const;
  void attemptReconnection();

private slots:
  void handleClipboardChanged(const QString &content, qulonglong timestamp, const QString &source,
                              const QString &mimeType, const QString &contentType, const QString &contentHash,
                              qulonglong size, const QString &sourceApp);
  void handleDBusDisconnection();
  void onReconnectTimer();

public:
  GnomeClipboardServer();
  ~GnomeClipboardServer() override;

  // AbstractClipboardServer interface
  bool start() override;
  QString id() const override;
  bool isAlive() const override;
  bool isActivatable() const override;
  int activationPriority() const override;
};
