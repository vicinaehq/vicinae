#pragma once
#include <expected>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QString>
#include <QVariantList>

namespace dbus {

inline bool isServiceRegistered(const QString &service) {
  auto *iface = QDBusConnection::sessionBus().interface();
  return iface && iface->isServiceRegistered(service).value();
}

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
