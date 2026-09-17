#pragma once
#include <QDBusArgument>
#include <QDBusVariant>
#include <QList>
#include <QStringList>
#include <QVariantMap>

struct SniPixmap {
  int width = 0;
  int height = 0;
  QByteArray data;
};

struct SniToolTip {
  QString iconName;
  QList<SniPixmap> pixmaps;
  QString title;
  QString description;
};

struct DBusMenuLayout {
  int id = 0;
  QVariantMap properties;
  QList<DBusMenuLayout> children;
};

struct DBusMenuItemProperties {
  int id = 0;
  QVariantMap properties;
};

struct DBusMenuItemKeys {
  int id = 0;
  QStringList keys;
};

struct DBusMenuEvent {
  int id = 0;
  QString eventId;
  QDBusVariant data;
  uint timestamp = 0;
};

Q_DECLARE_METATYPE(SniPixmap)
Q_DECLARE_METATYPE(SniToolTip)
Q_DECLARE_METATYPE(DBusMenuLayout)
Q_DECLARE_METATYPE(DBusMenuItemProperties)
Q_DECLARE_METATYPE(DBusMenuItemKeys)
Q_DECLARE_METATYPE(DBusMenuEvent)

QDBusArgument &operator<<(QDBusArgument &arg, const SniPixmap &pixmap);
const QDBusArgument &operator>>(const QDBusArgument &arg, SniPixmap &pixmap);
QDBusArgument &operator<<(QDBusArgument &arg, const SniToolTip &tooltip);
const QDBusArgument &operator>>(const QDBusArgument &arg, SniToolTip &tooltip);
QDBusArgument &operator<<(QDBusArgument &arg, const DBusMenuLayout &layout);
const QDBusArgument &operator>>(const QDBusArgument &arg, DBusMenuLayout &layout);
QDBusArgument &operator<<(QDBusArgument &arg, const DBusMenuItemProperties &item);
const QDBusArgument &operator>>(const QDBusArgument &arg, DBusMenuItemProperties &item);
QDBusArgument &operator<<(QDBusArgument &arg, const DBusMenuItemKeys &item);
const QDBusArgument &operator>>(const QDBusArgument &arg, DBusMenuItemKeys &item);
QDBusArgument &operator<<(QDBusArgument &arg, const DBusMenuEvent &event);
const QDBusArgument &operator>>(const QDBusArgument &arg, DBusMenuEvent &event);

void registerSniMetaTypes();
