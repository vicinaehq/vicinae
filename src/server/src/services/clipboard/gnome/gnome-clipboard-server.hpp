#pragma once
#include "services/clipboard/clipboard-server.hpp"
#include <QtDBus/QtDBus>
#include <QtDBus/qdbusconnection.h>
#include <QtDBus/qdbusinterface.h>
#include <QObject>
#include <QTimer>
#include <QMimeData>

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

  // Clipboard content helpers
  QString selectPreferredFormat(const QStringList &formats);
  bool setTextContent(const QString &text);
  bool setBinaryContent(const QByteArray &data, const QString &mimeType);

private slots:
  void handleClipboardChanged(const QByteArray &content, const QString &mimeType, const QString &sourceApp);
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
  bool setClipboardContent(QMimeData *data) override;
};
