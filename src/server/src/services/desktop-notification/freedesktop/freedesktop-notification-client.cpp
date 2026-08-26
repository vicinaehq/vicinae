#include "freedesktop-notification-client.hpp"
#include <QDBusConnection>
#include <QDBusMessage>

// https://specifications.freedesktop.org/notification/latest/protocol.html

bool FreedesktopNotificationClient::send(const Notification &n) {
  QVariantMap hints;
  hints[QStringLiteral("urgency")] = QVariant::fromValue(mapUrgency(n.urgency));

  QDBusMessage msg =
      QDBusMessage::createMethodCall(DBUS_SERVICE, DBUS_PATH, DBUS_INTERFACE, QStringLiteral("Notify"));
  msg << QStringLiteral("Vicinae") << uint(0) << n.iconPath.value_or(QString()) << n.title << n.body
      << QStringList() << hints << -1;

  return QDBusConnection::sessionBus().send(msg);
}

uchar FreedesktopNotificationClient::mapUrgency(Urgency urgency) {
  switch (urgency) {
  case Urgency::Low:
    return 0;
  case Urgency::Normal:
    return 1;
  case Urgency::High:
    return 2;
  }
  return 1;
}
