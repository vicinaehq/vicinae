#include "freedesktop-notification-client.hpp"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <qcontainerfwd.h>
#include <qlogging.h>

// https://specifications.freedesktop.org/notification/latest/protocol.html

FreedesktopNotificationClient::FreedesktopNotificationClient() {
  m_iface = std::make_unique<QDBusInterface>(DBUS_SERVICE, DBUS_PATH, DBUS_INTERFACE,
                                             QDBusConnection::sessionBus());
}

bool FreedesktopNotificationClient::send(const Notification &n) {
  if (!m_iface->isValid()) return false;

  QVariantMap hints;
  hints[QStringLiteral("urgency")] = QVariant::fromValue(mapUrgency(n.urgency));

  QDBusMessage reply =
      m_iface->call(QStringLiteral("Notify"), QStringLiteral("Vicinae"), uint(0),
                    n.iconPath.value_or(QString()), n.title, n.body, QStringList(), hints, -1);

  return reply.type() != QDBusMessage::ErrorMessage;
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
