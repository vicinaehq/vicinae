#include "sni-types.hpp"
#include <QDBusMetaType>

// NOLINTBEGIN(bugprone-return-const-ref-from-parameter)
QDBusArgument &operator<<(QDBusArgument &arg, const SniPixmap &pixmap) {
  arg.beginStructure();
  arg << pixmap.width << pixmap.height << pixmap.data;
  arg.endStructure();
  return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, SniPixmap &pixmap) {
  arg.beginStructure();
  arg >> pixmap.width >> pixmap.height >> pixmap.data;
  arg.endStructure();
  return arg;
}

QDBusArgument &operator<<(QDBusArgument &arg, const SniToolTip &tooltip) {
  arg.beginStructure();
  arg << tooltip.iconName << tooltip.pixmaps << tooltip.title << tooltip.description;
  arg.endStructure();
  return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, SniToolTip &tooltip) {
  arg.beginStructure();
  arg >> tooltip.iconName >> tooltip.pixmaps >> tooltip.title >> tooltip.description;
  arg.endStructure();
  return arg;
}

QDBusArgument &operator<<(QDBusArgument &arg, const DBusMenuLayout &layout) {
  arg.beginStructure();
  arg << layout.id << layout.properties;
  arg.beginArray(QMetaType::fromType<QDBusVariant>());
  for (const auto &child : layout.children) {
    arg << QDBusVariant(QVariant::fromValue(child));
  }
  arg.endArray();
  arg.endStructure();
  return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, DBusMenuLayout &layout) {
  QVariantList children;
  arg.beginStructure();
  arg >> layout.id >> layout.properties >> children;
  arg.endStructure();
  for (const auto &child : children) {
    layout.children << qdbus_cast<DBusMenuLayout>(child);
  }
  return arg;
}

QDBusArgument &operator<<(QDBusArgument &arg, const DBusMenuItemProperties &item) {
  arg.beginStructure();
  arg << item.id << item.properties;
  arg.endStructure();
  return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, DBusMenuItemProperties &item) {
  arg.beginStructure();
  arg >> item.id >> item.properties;
  arg.endStructure();
  return arg;
}

QDBusArgument &operator<<(QDBusArgument &arg, const DBusMenuItemKeys &item) {
  arg.beginStructure();
  arg << item.id << item.keys;
  arg.endStructure();
  return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, DBusMenuItemKeys &item) {
  arg.beginStructure();
  arg >> item.id >> item.keys;
  arg.endStructure();
  return arg;
}

QDBusArgument &operator<<(QDBusArgument &arg, const DBusMenuEvent &event) {
  arg.beginStructure();
  arg << event.id << event.eventId << event.data << event.timestamp;
  arg.endStructure();
  return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, DBusMenuEvent &event) {
  arg.beginStructure();
  arg >> event.id >> event.eventId >> event.data >> event.timestamp;
  arg.endStructure();
  return arg;
}
// NOLINTEND(bugprone-return-const-ref-from-parameter)

void registerSniMetaTypes() {
  static const bool registered = [] {
    qDBusRegisterMetaType<SniPixmap>();
    qDBusRegisterMetaType<QList<SniPixmap>>();
    qDBusRegisterMetaType<SniToolTip>();
    qDBusRegisterMetaType<DBusMenuLayout>();
    qDBusRegisterMetaType<DBusMenuItemProperties>();
    qDBusRegisterMetaType<QList<DBusMenuItemProperties>>();
    qDBusRegisterMetaType<DBusMenuItemKeys>();
    qDBusRegisterMetaType<QList<DBusMenuItemKeys>>();
    qDBusRegisterMetaType<DBusMenuEvent>();
    qDBusRegisterMetaType<QList<DBusMenuEvent>>();
    return true;
  }();
  (void)registered;
}
