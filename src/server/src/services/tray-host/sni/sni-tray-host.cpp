#include "sni-tray-host.hpp"
#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QDBusVariant>
#include <QDateTime>
#include <QPromise>
#include <QtEndian>
#include <unistd.h>

namespace {

QImage toImage(const QList<SniPixmap> &pixmaps) {
  const SniPixmap *best = nullptr;
  for (const auto &p : pixmaps) {
    if (p.width <= 0 || p.height <= 0 || p.data.size() < qint64(p.width) * p.height * 4) continue;
    if (!best || p.width > best->width) best = &p;
  }
  if (!best) return {};

  QImage img(best->width, best->height, QImage::Format_ARGB32);
  const auto *src = reinterpret_cast<const quint32 *>(best->data.constData());
  for (int y = 0; y < best->height; ++y) {
    auto *row = reinterpret_cast<quint32 *>(img.scanLine(y));
    for (int x = 0; x < best->width; ++x) {
      row[x] = qFromBigEndian(src[y * best->width + x]);
    }
  }
  return img;
}

TrayItem::Status parseStatus(const QString &s) {
  if (s == "NeedsAttention") return TrayItem::Status::NeedsAttention;
  if (s == "Passive") return TrayItem::Status::Passive;
  return TrayItem::Status::Active;
}

TrayItem::Category parseCategory(const QString &s) {
  if (s == "Communications") return TrayItem::Category::Communications;
  if (s == "SystemServices") return TrayItem::Category::SystemServices;
  if (s == "Hardware") return TrayItem::Category::Hardware;
  return TrayItem::Category::ApplicationStatus;
}

void applyProperties(TrayItem &item, const QVariantMap &props) {
  for (auto it = props.begin(); it != props.end(); ++it) {
    const auto &key = it.key();
    const auto &v = it.value();

    if (key == "Id")
      item.id = v.toString();
    else if (key == "Title")
      item.title = v.toString();
    else if (key == "Status")
      item.status = parseStatus(v.toString());
    else if (key == "Category")
      item.category = parseCategory(v.toString());
    else if (key == "IconName")
      item.iconName = v.toString();
    else if (key == "IconThemePath")
      item.iconThemePath = v.toString();
    else if (key == "IconPixmap")
      item.iconPixmap = toImage(qdbus_cast<QList<SniPixmap>>(v));
    else if (key == "AttentionIconName")
      item.attentionIconName = v.toString();
    else if (key == "AttentionIconPixmap")
      item.attentionIconPixmap = toImage(qdbus_cast<QList<SniPixmap>>(v));
    else if (key == "ItemIsMenu")
      item.itemIsMenu = v.toBool();
    else if (key == "Menu")
      item.menuPath = qdbus_cast<QDBusObjectPath>(v).path();
    else if (key == "ToolTip") {
      const auto tooltip = qdbus_cast<SniToolTip>(v);
      item.tooltipTitle = tooltip.title;
      item.tooltipDescription = tooltip.description;
    }
  }
}

TrayMenuItem toMenuItem(const DBusMenuLayout &layout) {
  TrayMenuItem item;
  const auto &props = layout.properties;

  item.id = layout.id;
  item.label = props.value("label").toString();
  item.enabled = props.value("enabled", true).toBool();
  item.visible = props.value("visible", true).toBool();
  item.separator = props.value("type").toString() == "separator";
  item.submenu = props.value("children-display").toString() == "submenu";
  item.iconName = props.value("icon-name").toString();

  const auto toggle = props.value("toggle-type").toString();
  if (toggle == "checkmark")
    item.toggleType = TrayMenuItem::ToggleType::Checkmark;
  else if (toggle == "radio")
    item.toggleType = TrayMenuItem::ToggleType::Radio;
  if (props.contains("toggle-state")) item.toggleState = props.value("toggle-state").toInt();

  if (auto data = props.value("icon-data").toByteArray(); !data.isEmpty()) {
    item.iconData = QImage::fromData(data, "PNG");
  }

  for (const auto &child : layout.children) {
    item.children.push_back(toMenuItem(child));
  }

  return item;
}

} // namespace

SniTrayHost::SniTrayHost() : m_watcher(WATCHER_SERVICE, QDBusConnection::sessionBus()) {
  registerSniMetaTypes();

  connect(&m_watcher, &QDBusServiceWatcher::serviceOwnerChanged, this,
          [this](const QString &, const QString &, const QString &newOwner) {
            if (m_available) watcherVanished();
            if (!newOwner.isEmpty()) watcherAppeared();
          });

  auto *iface = QDBusConnection::sessionBus().interface();
  if (iface && iface->isServiceRegistered(WATCHER_SERVICE)) watcherAppeared();
}

SniTrayHost::~SniTrayHost() {
  for (const auto &[key, item] : m_items) {
    subscribeItem({item.busName, item.path}, false);
  }
  if (!m_hostName.isEmpty()) QDBusConnection::sessionBus().unregisterService(m_hostName);
}

SniTrayHost::ItemRef SniTrayHost::parseItemRef(const QString &ref) {
  const auto slash = ref.indexOf('/');
  if (slash < 0) return {ref, "/StatusNotifierItem"};
  return {ref.left(slash), ref.mid(slash)};
}

void SniTrayHost::watcherAppeared() {
  if (m_available) return;
  m_available = true;

  auto bus = QDBusConnection::sessionBus();

  bus.connect(WATCHER_SERVICE, WATCHER_PATH, WATCHER_IFACE, "StatusNotifierItemRegistered", this,
              SLOT(onItemRegistered(QString)));
  bus.connect(WATCHER_SERVICE, WATCHER_PATH, WATCHER_IFACE, "StatusNotifierItemUnregistered", this,
              SLOT(onItemUnregistered(QString)));

  registerHost();
  loadRegisteredItems();
}

void SniTrayHost::watcherVanished() {
  auto bus = QDBusConnection::sessionBus();
  bus.disconnect(WATCHER_SERVICE, WATCHER_PATH, WATCHER_IFACE, "StatusNotifierItemRegistered", this,
                 SLOT(onItemRegistered(QString)));
  bus.disconnect(WATCHER_SERVICE, WATCHER_PATH, WATCHER_IFACE, "StatusNotifierItemUnregistered", this,
                 SLOT(onItemUnregistered(QString)));

  auto items = std::move(m_items);
  m_items.clear();
  for (const auto &[key, item] : items) {
    subscribeItem({item.busName, item.path}, false);
    if (auto sub = m_menuSubscriptions.find(key); sub != m_menuSubscriptions.end()) {
      subscribeMenu(item.busName, sub->second, false);
    }
  }
  m_menuSubscriptions.clear();
  m_pending.clear();
  m_owners.clear();
  m_available = false;
  emit itemsChanged();
}

void SniTrayHost::registerHost() {
  auto bus = QDBusConnection::sessionBus();

  if (m_hostName.isEmpty()) {
    m_hostName = QString("org.kde.StatusNotifierHost-%1").arg(getpid());
    if (!bus.registerService(m_hostName)) {
      qWarning() << "Failed to register tray host name" << m_hostName;
      m_hostName.clear();
    }
  }

  auto msg = QDBusMessage::createMethodCall(WATCHER_SERVICE, WATCHER_PATH, WATCHER_IFACE,
                                            "RegisterStatusNotifierHost");
  msg << (m_hostName.isEmpty() ? bus.baseService() : m_hostName);
  bus.asyncCall(msg);
}

void SniTrayHost::loadRegisteredItems() {
  auto msg = QDBusMessage::createMethodCall(WATCHER_SERVICE, WATCHER_PATH, PROPERTIES_IFACE, "Get");
  msg << QString(WATCHER_IFACE) << QString("RegisteredStatusNotifierItems");

  auto *watcher = new QDBusPendingCallWatcher(QDBusConnection::sessionBus().asyncCall(msg), this);
  connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *w) {
    w->deleteLater();
    QDBusPendingReply<QVariant> reply = *w;
    if (reply.isError()) {
      qWarning() << "Failed to list tray items:" << reply.error().message();
      return;
    }

    for (const auto &ref : qdbus_cast<QStringList>(reply.value())) {
      addItem(parseItemRef(ref));
    }
  });
}

void SniTrayHost::addItem(const ItemRef &ref) {
  const auto key = ref.busName + ref.path;
  if (m_items.contains(key) || !m_pending.insert(key).second) return;
  resolveOwner(ref.busName);
  subscribeItem(ref, true);
  fetchItem(ref);
}

void SniTrayHost::resolveOwner(const QString &busName) {
  if (busName.startsWith(':') || m_owners.contains(busName)) return;

  auto msg = QDBusMessage::createMethodCall("org.freedesktop.DBus", "/org/freedesktop/DBus",
                                            "org.freedesktop.DBus", "GetNameOwner");
  msg << busName;

  auto *watcher = new QDBusPendingCallWatcher(QDBusConnection::sessionBus().asyncCall(msg), this);
  connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, busName](QDBusPendingCallWatcher *w) {
    w->deleteLater();
    QDBusPendingReply<QString> reply = *w;
    if (!reply.isError()) m_owners[busName] = reply.value();
  });
}

void SniTrayHost::removeItem(const QString &refStr) {
  const auto ref = parseItemRef(refStr);
  const bool hasPath = refStr.contains('/');

  for (auto it = m_items.begin(); it != m_items.end();) {
    const bool match = hasPath ? it->first == ref.busName + ref.path : it->second.busName == ref.busName;
    if (!match) {
      ++it;
      continue;
    }
    subscribeItem({it->second.busName, it->second.path}, false);
    const auto key = it->first;
    if (auto sub = m_menuSubscriptions.find(key); sub != m_menuSubscriptions.end()) {
      subscribeMenu(it->second.busName, sub->second, false);
      m_menuSubscriptions.erase(sub);
    }
    m_owners.erase(it->second.busName);
    it = m_items.erase(it);
    emit itemsChanged();
  }
  m_pending.erase(ref.busName + ref.path);
}

void SniTrayHost::fetchItem(const ItemRef &ref) {
  auto msg = QDBusMessage::createMethodCall(ref.busName, ref.path, PROPERTIES_IFACE, "GetAll");
  msg << QString(ITEM_IFACE);

  auto *watcher = new QDBusPendingCallWatcher(QDBusConnection::sessionBus().asyncCall(msg), this);
  connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, ref](QDBusPendingCallWatcher *w) {
    w->deleteLater();
    const auto key = ref.busName + ref.path;
    m_pending.erase(key);

    QDBusPendingReply<QVariantMap> reply = *w;
    if (reply.isError()) {
      qWarning() << "Failed to fetch tray item" << ref.busName << ref.path << reply.error().message();
      return;
    }

    auto &item = m_items[key];
    item.busName = ref.busName;
    item.path = ref.path;
    applyProperties(item, reply.value());
    item.resolveThemeIcons();

    if (auto sub = m_menuSubscriptions.find(key);
        sub == m_menuSubscriptions.end() || sub->second != item.menuPath) {
      if (sub != m_menuSubscriptions.end()) subscribeMenu(item.busName, sub->second, false);
      if (item.hasMenu()) {
        subscribeMenu(item.busName, item.menuPath, true);
        m_menuSubscriptions[key] = item.menuPath;
      } else {
        m_menuSubscriptions.erase(key);
      }
    }

    emit itemsChanged();
  });
}

void SniTrayHost::subscribeMenu(const QString &busName, const QString &menuPath, bool subscribe) {
  auto bus = QDBusConnection::sessionBus();
  for (const auto *signal : {"LayoutUpdated", "ItemsPropertiesUpdated"}) {
    if (subscribe) {
      bus.connect(busName, menuPath, MENU_IFACE, signal, this, SLOT(onMenuSignal(QDBusMessage)));
    } else {
      bus.disconnect(busName, menuPath, MENU_IFACE, signal, this, SLOT(onMenuSignal(QDBusMessage)));
    }
  }
}

bool SniTrayHost::isSender(const QString &busName, const QString &sender) const {
  if (busName == sender) return true;
  auto owner = m_owners.find(busName);
  return owner != m_owners.end() && owner->second == sender;
}

void SniTrayHost::onMenuSignal(const QDBusMessage &msg) {
  for (const auto &[key, item] : m_items) {
    if (item.menuPath == msg.path() && isSender(item.busName, msg.service())) emit menuChanged(key);
  }
}

void SniTrayHost::subscribeItem(const ItemRef &ref, bool subscribe) {
  auto bus = QDBusConnection::sessionBus();
  static const char *signalNames[] = {"NewIcon",    "NewAttentionIcon", "NewTitle", "NewStatus",
                                      "NewToolTip", "NewIconThemePath", "NewMenu"};

  for (const auto *signal : signalNames) {
    if (subscribe) {
      bus.connect(ref.busName, ref.path, ITEM_IFACE, signal, this, SLOT(onItemSignal(QDBusMessage)));
    } else {
      bus.disconnect(ref.busName, ref.path, ITEM_IFACE, signal, this, SLOT(onItemSignal(QDBusMessage)));
    }
  }
}

void SniTrayHost::onItemRegistered(const QString &ref) { addItem(parseItemRef(ref)); }

void SniTrayHost::onItemUnregistered(const QString &ref) { removeItem(ref); }

void SniTrayHost::onItemSignal(const QDBusMessage &msg) {
  const auto sender = msg.service();
  const auto path = msg.path();

  for (const auto &[key, item] : m_items) {
    if (item.path == path && isSender(item.busName, sender)) fetchItem({item.busName, item.path});
  }
}

std::vector<TrayItem> SniTrayHost::items() const {
  std::vector<TrayItem> out;
  out.reserve(m_items.size());
  for (const auto &[key, item] : m_items) {
    out.push_back(item);
  }
  return out;
}

void SniTrayHost::callItem(const TrayItem &item, const QString &method, const QVariantList &args) {
  auto msg = QDBusMessage::createMethodCall(item.busName, item.path, ITEM_IFACE, method);
  msg.setArguments(args);
  QDBusConnection::sessionBus().asyncCall(msg);
}

void SniTrayHost::activate(const TrayItem &item) { callItem(item, "Activate", {0, 0}); }

void SniTrayHost::secondaryActivate(const TrayItem &item) { callItem(item, "SecondaryActivate", {0, 0}); }

QFuture<std::vector<TrayMenuItem>> SniTrayHost::menu(const TrayItem &item) {
  auto promise = std::make_shared<QPromise<std::vector<TrayMenuItem>>>();
  auto future = promise->future();
  promise->start();

  if (!item.hasMenu()) {
    promise->addResult({});
    promise->finish();
    return future;
  }

  auto bus = QDBusConnection::sessionBus();
  auto aboutToShow = QDBusMessage::createMethodCall(item.busName, item.menuPath, MENU_IFACE, "AboutToShow");
  aboutToShow << 0;

  auto *w1 = new QDBusPendingCallWatcher(bus.asyncCall(aboutToShow), this);
  connect(w1, &QDBusPendingCallWatcher::finished, this, [this, item, promise](QDBusPendingCallWatcher *w) {
    w->deleteLater();

    auto layout = QDBusMessage::createMethodCall(item.busName, item.menuPath, MENU_IFACE, "GetLayout");
    layout << 0 << -1
           << QStringList{"label",     "enabled",   "visible",         "type", "toggle-type", "toggle-state",
                          "icon-name", "icon-data", "children-display"};

    auto *w2 = new QDBusPendingCallWatcher(QDBusConnection::sessionBus().asyncCall(layout), this);
    connect(w2, &QDBusPendingCallWatcher::finished, this, [item, promise](QDBusPendingCallWatcher *w) {
      w->deleteLater();
      QDBusPendingReply<uint, DBusMenuLayout> reply = *w;

      if (reply.isError()) {
        qWarning() << "Failed to fetch tray menu for" << item.id << reply.error().message();
        promise->addResult({});
      } else {
        promise->addResult(toMenuItem(reply.argumentAt<1>()).children);
      }
      promise->finish();
    });
  });

  return future;
}

void SniTrayHost::triggerMenuItem(const TrayItem &item, int menuItemId) {
  if (!item.hasMenu()) return;
  auto msg = QDBusMessage::createMethodCall(item.busName, item.menuPath, MENU_IFACE, "Event");
  msg << menuItemId << QString("clicked") << QVariant::fromValue(QDBusVariant(0))
      << static_cast<uint>(QDateTime::currentSecsSinceEpoch());
  QDBusConnection::sessionBus().asyncCall(msg);
}
