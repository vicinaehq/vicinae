#pragma once
#include <expected>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QString>
#include <QVariantList>

/**
 * Thin helpers around session-bus DBus calls, shared by the services that need
 * to talk to desktop-environment daemons (KWin, plasmashell...).
 */
namespace dbus {

inline bool isServiceRegistered(const QString &service) {
  auto *iface = QDBusConnection::sessionBus().interface();
  return iface && iface->isServiceRegistered(service).value();
}

/**
 * Synchronously call a method on the session bus, returning the reply arguments
 * on success or the DBus error message otherwise.
 */
inline std::expected<QVariantList, QString> call(const QString &service, const QString &path,
                                                 const QString &interface, const QString &method,
                                                 const QVariantList &args = {}) {
  auto msg = QDBusMessage::createMethodCall(service, path, interface, method);
  msg.setArguments(args);

  const auto reply = QDBusConnection::sessionBus().call(msg);

  if (reply.type() == QDBusMessage::ErrorMessage) { return std::unexpected(reply.errorMessage()); }

  return reply.arguments();
}

} // namespace dbus
