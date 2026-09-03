#include "services/tray/linux/tray-service-linux.hpp"
#include <algorithm>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QIcon>
#include <QImage>
#include <QPainter>
#include <QSvgRenderer>
#include <QtEndian>
#include <unistd.h>

namespace {
constexpr const char *WATCHER_SERVICE = "org.kde.StatusNotifierWatcher";
constexpr const char *WATCHER_PATH = "/StatusNotifierWatcher";
constexpr const char *WATCHER_IFACE = "org.kde.StatusNotifierWatcher";
constexpr const char *THEME_ICON_NAME = "vicinae";
constexpr int PIXMAP_SIZES[] = {16, 22, 24, 32, 48, 64};
constexpr qreal DISC_RADIUS_RATIO = 80.0 / 200.0;

SniPixmap renderPixmap(QSvgRenderer &renderer, int size) {
  QImage image(size, size, QImage::Format_ARGB32);
  image.fill(Qt::transparent);
  {
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    const qreal radius = size * DISC_RADIUS_RATIO;
    painter.drawEllipse(QPointF(size / 2.0, size / 2.0), radius, radius);
    renderer.render(&painter);
  }

  SniPixmap pixmap{.width = size, .height = size};
  pixmap.data.resize(static_cast<qsizetype>(size) * size * 4);
  auto *dst = reinterpret_cast<quint32 *>(pixmap.data.data());
  for (int y = 0; y < size; ++y) {
    const auto *row = reinterpret_cast<const quint32 *>(image.constScanLine(y));
    for (int x = 0; x < size; ++x) {
      dst[y * size + x] = qToBigEndian(row[x]);
    }
  }
  return pixmap;
}

QList<SniPixmap> renderPixmaps() {
  QSvgRenderer renderer(QStringLiteral(":icons/vicinae.svg"));
  if (!renderer.isValid()) return {};

  QList<SniPixmap> pixmaps;
  for (int size : PIXMAP_SIZES) {
    pixmaps << renderPixmap(renderer, size);
  }
  return pixmaps;
}
} // namespace

SniItemObject::SniItemObject(TrayServiceLinux &owner) : m_owner(owner) {
  if (QIcon::hasThemeIcon(THEME_ICON_NAME)) m_iconName = THEME_ICON_NAME;
  m_pixmaps = renderPixmaps();
}

QString SniItemObject::status() const { return m_owner.visible() ? "Active" : "Passive"; }

SniToolTip SniItemObject::toolTip() const { return {.iconName = m_iconName, .title = "Vicinae"}; }

QDBusObjectPath SniItemObject::menu() const { return QDBusObjectPath(TrayServiceLinux::MENU_PATH); }

void SniItemObject::Activate(int, int) { emit m_owner.toggleRequested(); }

void SniItemObject::SecondaryActivate(int, int) { emit m_owner.toggleRequested(); }

void SniItemObject::ContextMenu(int, int) {}

void SniItemObject::Scroll(int, const QString &) {}

void SniItemObject::ProvideXdgActivationToken(const QString &) {}

DBusMenuObject::DBusMenuObject(TrayServiceLinux &owner) : m_owner(owner) {}

void DBusMenuObject::layoutChanged() { emit LayoutUpdated(++m_revision, 0); }

QVariantMap DBusMenuObject::itemProperties(int id, const QStringList &names) const {
  QVariantMap all;

  if (id == 0) {
    all["children-display"] = "submenu";
  } else {
    const auto &entries = m_owner.entries();
    auto it = std::find_if(entries.begin(), entries.end(), [id](const auto &e) { return e.id == id; });
    if (it == entries.end()) return {};

    if (it->kind == TrayServiceLinux::MenuEntry::Kind::Separator) {
      all["type"] = "separator";
    } else {
      all["type"] = "standard";
      all["label"] = m_owner.entryLabel(*it);
      all["enabled"] = m_owner.entryEnabled(*it);
    }
    all["visible"] = true;
  }

  if (names.isEmpty()) return all;

  QVariantMap filtered;
  for (const auto &name : names) {
    if (auto it = all.find(name); it != all.end()) filtered.insert(name, *it);
  }
  return filtered;
}

uint DBusMenuObject::GetLayout(int parentId, int recursionDepth, const QStringList &propertyNames,
                               DBusMenuLayout &layout) {
  layout.id = parentId;
  layout.properties = itemProperties(parentId, propertyNames);

  if (parentId == 0 && recursionDepth != 0) {
    for (const auto &entry : m_owner.entries()) {
      layout.children << DBusMenuLayout{.id = entry.id,
                                        .properties = itemProperties(entry.id, propertyNames)};
    }
  }

  return m_revision;
}

QList<DBusMenuItemProperties> DBusMenuObject::GetGroupProperties(const QList<int> &ids,
                                                                 const QStringList &propertyNames) {
  QList<DBusMenuItemProperties> out;
  auto append = [&](int id) {
    out << DBusMenuItemProperties{.id = id, .properties = itemProperties(id, propertyNames)};
  };

  if (ids.isEmpty()) {
    append(0);
    for (const auto &entry : m_owner.entries()) {
      append(entry.id);
    }
    return out;
  }

  for (int id : ids) {
    append(id);
  }
  return out;
}

QDBusVariant DBusMenuObject::GetProperty(int id, const QString &name) {
  return QDBusVariant(itemProperties(id, {name}).value(name));
}

void DBusMenuObject::Event(int id, const QString &eventId, const QDBusVariant &, uint) {
  if (eventId == "clicked") m_owner.activateEntry(id);
}

QList<int> DBusMenuObject::EventGroup(const QList<DBusMenuEvent> &events) {
  for (const auto &event : events) {
    Event(event.id, event.eventId, event.data, event.timestamp);
  }
  return {};
}

bool DBusMenuObject::AboutToShow(int) { return false; }

QList<int> DBusMenuObject::AboutToShowGroup(const QList<int> &, QList<int> &idErrors) {
  idErrors = {};
  return {};
}

TrayServiceLinux::TrayServiceLinux(QObject *parent)
    : TrayService(parent),
      m_watcher(WATCHER_SERVICE, QDBusConnection::sessionBus(), QDBusServiceWatcher::WatchForOwnerChange),
      m_item(*this), m_menu(*this) {
  using Kind = MenuEntry::Kind;
  std::vector<Kind> kinds = {Kind::Toggle,    Kind::Version, Kind::Separator, Kind::About, Kind::Settings,
                             Kind::Separator, Kind::Sponsor, Kind::Discord,   Kind::Follow};
  if (!qEnvironmentVariableIsSet("INVOCATION_ID")) kinds.insert(kinds.end(), {Kind::Separator, Kind::Quit});

  int nextId = 1;
  for (auto kind : kinds) {
    m_entries.push_back({.id = nextId++, .kind = kind});
  }

  registerSniMetaTypes();

  auto bus = QDBusConnection::sessionBus();
  if (!bus.isConnected()) return;

  const auto flags = QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllProperties |
                     QDBusConnection::ExportAllSignals;
  if (!bus.registerObject(ITEM_PATH, &m_item, flags) || !bus.registerObject(MENU_PATH, &m_menu, flags)) {
    qWarning() << "Failed to export tray item objects on the session bus";
    return;
  }

  m_serviceName = QString("org.kde.StatusNotifierItem-%1-1").arg(getpid());
  if (!bus.registerService(m_serviceName)) {
    qWarning() << "Failed to register tray item service name" << m_serviceName;
    m_serviceName = bus.baseService();
  }

  connect(&m_watcher, &QDBusServiceWatcher::serviceOwnerChanged, this,
          [this](const QString &, const QString &, const QString &newOwner) {
            m_registered = false;
            if (!newOwner.isEmpty() && m_visible) registerWithWatcher();
          });
}

TrayServiceLinux::~TrayServiceLinux() {
  auto bus = QDBusConnection::sessionBus();
  bus.unregisterObject(ITEM_PATH);
  bus.unregisterObject(MENU_PATH);
  if (!m_serviceName.isEmpty() && m_serviceName != bus.baseService()) bus.unregisterService(m_serviceName);
}

void TrayServiceLinux::registerWithWatcher() {
  if (m_registered || m_serviceName.isEmpty()) return;

  auto bus = QDBusConnection::sessionBus();
  auto *iface = bus.interface();
  if (!iface || !iface->isServiceRegistered(WATCHER_SERVICE)) return;

  auto msg = QDBusMessage::createMethodCall(WATCHER_SERVICE, WATCHER_PATH, WATCHER_IFACE,
                                            "RegisterStatusNotifierItem");
  msg << m_serviceName;
  bus.asyncCall(msg);
  m_registered = true;
}

QString TrayServiceLinux::entryLabel(const MenuEntry &entry) const {
  using Kind = MenuEntry::Kind;
  switch (entry.kind) {
  case Kind::Toggle:
    return tr("Toggle Vicinae");
  case Kind::Version:
    return m_version.isEmpty() ? QStringLiteral("Vicinae") : QStringLiteral("Vicinae %1").arg(m_version);
  case Kind::About:
    return tr("About Vicinae");
  case Kind::Settings:
    return tr("Settings…");
  case Kind::Sponsor:
    return tr("Sponsor Vicinae");
  case Kind::Discord:
    return tr("Join the Discord");
  case Kind::Follow:
    return tr("Follow on X");
  case Kind::Quit:
    return tr("Quit Vicinae");
  case Kind::Separator:
    return {};
  }
  return {};
}

bool TrayServiceLinux::entryEnabled(const MenuEntry &entry) const {
  return entry.kind != MenuEntry::Kind::Version;
}

void TrayServiceLinux::activateEntry(int id) {
  using Kind = MenuEntry::Kind;
  auto it = std::find_if(m_entries.begin(), m_entries.end(), [id](const auto &e) { return e.id == id; });
  if (it == m_entries.end()) return;

  switch (it->kind) {
  case Kind::Toggle:
    emit toggleRequested();
    break;
  case Kind::About:
    emit openSettingsRequested(QStringLiteral("about"));
    break;
  case Kind::Settings:
    emit openSettingsRequested(QString());
    break;
  case Kind::Sponsor:
    emit openLinkRequested(Link::Sponsor);
    break;
  case Kind::Discord:
    emit openLinkRequested(Link::Discord);
    break;
  case Kind::Follow:
    emit openLinkRequested(Link::Follow);
    break;
  case Kind::Quit:
    emit quitRequested();
    break;
  case Kind::Version:
  case Kind::Separator:
    break;
  }
}

void TrayServiceLinux::setVersion(const QString &version) {
  m_version = version;
  m_menu.layoutChanged();
}

void TrayServiceLinux::setCheckForUpdatesVisible(bool) {}

void TrayServiceLinux::setAvailableUpdate(const QString &) {}

void TrayServiceLinux::show() {
  if (m_visible) return;
  m_visible = true;
  registerWithWatcher();
  emit m_item.NewStatus(m_item.status());
}

void TrayServiceLinux::hide() {
  if (!m_visible) return;
  m_visible = false;
  emit m_item.NewStatus(m_item.status());
}
