#include "services/global-shortcuts/global-shortcut-portal-backend.hpp"
#include "services/global-shortcuts/xkb-keysym.hpp"
#include <QDBusArgument>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDBusReply>
#include <QRandomGenerator>
#include <QStringList>
#include <array>
#include <qlogging.h>
#include <xkbcommon/xkbcommon.h>

namespace {
constexpr auto PORTAL_SERVICE = "org.freedesktop.portal.Desktop";
constexpr auto PORTAL_PATH = "/org/freedesktop/portal/desktop";
constexpr auto SHORTCUTS_IFACE = "org.freedesktop.portal.GlobalShortcuts";
constexpr auto SESSION_IFACE = "org.freedesktop.portal.Session";
constexpr auto REQUEST_IFACE = "org.freedesktop.portal.Request";

// Builds the XDG Shortcuts-spec trigger string (e.g. "LOGO+SHIFT+space") for `preferred_trigger`.
std::optional<QString> xdgTriggerFor(const Keyboard::Shortcut &shortcut) {
  auto keysym = global_shortcuts::xkbKeysymForQtKey(shortcut.key());
  if (!keysym) return std::nullopt;

  std::array<char, 64> name{};
  const int len = xkb_keysym_get_name(*keysym, name.data(), name.size());
  if (len <= 0) return std::nullopt;

  QStringList parts;
  const auto mods = shortcut.mods();
  if (mods & Qt::ControlModifier) parts << QStringLiteral("CTRL");
  if (mods & Qt::AltModifier) parts << QStringLiteral("ALT");
  if (mods & Qt::ShiftModifier) parts << QStringLiteral("SHIFT");
  if (mods & Qt::MetaModifier) parts << QStringLiteral("LOGO");
  parts << QString::fromLatin1(name.data(), len);

  return parts.join(QLatin1Char('+'));
}
} // namespace

// Wire type for the portal `a(sa{sv})` shortcut list.
struct PortalShortcut {
  QString id;
  QVariantMap props;
};
Q_DECLARE_METATYPE(PortalShortcut)
Q_DECLARE_METATYPE(QList<PortalShortcut>)

QDBusArgument &operator<<(QDBusArgument &arg, const PortalShortcut &shortcut) {
  arg.beginStructure();
  arg << shortcut.id << shortcut.props;
  arg.endStructure();
  return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, PortalShortcut &shortcut) {
  arg.beginStructure();
  arg >> shortcut.id >> shortcut.props;
  arg.endStructure();
  return arg;
}

GlobalShortcutPortalBackend::GlobalShortcutPortalBackend() : m_bus(QDBusConnection::sessionBus()) {
  qDBusRegisterMetaType<PortalShortcut>();
  qDBusRegisterMetaType<QList<PortalShortcut>>();

  m_iface = new QDBusInterface(PORTAL_SERVICE, PORTAL_PATH, SHORTCUTS_IFACE, m_bus, this);

  m_applyTimer.setSingleShot(true);
  m_applyTimer.setInterval(0);
  connect(&m_applyTimer, &QTimer::timeout, this, [this] {
    if (!m_sessionReady) return;
    // The portal allows binding a session only once, so re-apply by recreating it.
    closeSession();
    createSession();
  });

  m_bus.connect(PORTAL_SERVICE, PORTAL_PATH, SHORTCUTS_IFACE, "Activated", this,
                SLOT(onActivated(QDBusObjectPath, QString, qulonglong, QVariantMap)));
}

bool GlobalShortcutPortalBackend::isActivatable() const {
  if (!m_bus.isConnected() || !m_iface->isValid()) return false;
  const QVariant version = m_iface->property("version");
  return version.isValid() && version.toUInt() >= 1;
}

bool GlobalShortcutPortalBackend::start() {
  if (m_started) return true;
  m_started = true;
  createSession();
  return true;
}

QString GlobalShortcutPortalBackend::newToken(const QString &prefix) const {
  return QStringLiteral("vicinae_%1_%2").arg(prefix).arg(QRandomGenerator::global()->generate());
}

void GlobalShortcutPortalBackend::createSession() {
  QVariantMap options;
  options["handle_token"] = newToken("create");
  options["session_handle_token"] = newToken("session");

  const QDBusReply<QDBusObjectPath> reply = m_iface->call("CreateSession", options);
  if (!reply.isValid()) {
    qWarning() << "GlobalShortcuts CreateSession failed:" << reply.error();
    return;
  }

  m_bus.connect("", reply.value().path(), REQUEST_IFACE, "Response", this,
                SLOT(onCreateSessionResponse(uint, QVariantMap)));
}

void GlobalShortcutPortalBackend::onCreateSessionResponse(uint response, const QVariantMap &results) {
  if (response != 0) {
    qWarning() << "GlobalShortcuts session creation failed, response:" << response;
    return;
  }

  const QVariant handle = results.value("session_handle");
  m_sessionHandle =
      handle.canConvert<QDBusObjectPath>() ? handle.value<QDBusObjectPath>().path() : handle.toString();
  m_sessionReady = true;
  emit ready();

  if (!m_desired.empty()) bindShortcuts();
}

void GlobalShortcutPortalBackend::bindShortcut(const GlobalShortcutRequest &request) {
  m_desired[request.id] = request;

  GlobalShortcutInfo info;
  info.id = request.id;
  info.status = GlobalShortcutStatus::Unbound;
  if (request.preferredTrigger) info.triggerDisplay = request.preferredTrigger->toDisplayString();
  m_info[request.id] = std::move(info);

  if (m_sessionReady) m_applyTimer.start();
}

void GlobalShortcutPortalBackend::unbindShortcut(const QString &id) {
  if (m_desired.erase(id) == 0) return;
  m_info.erase(id);
  if (m_sessionReady) m_applyTimer.start();
}

void GlobalShortcutPortalBackend::bindShortcuts() {
  if (m_sessionHandle.isEmpty()) return;

  QList<PortalShortcut> shortcuts;
  shortcuts.reserve(static_cast<int>(m_desired.size()));
  for (const auto &[id, request] : m_desired) {
    QVariantMap props;
    props["description"] = request.description;
    if (request.preferredTrigger) {
      if (auto trigger = xdgTriggerFor(*request.preferredTrigger)) { props["preferred_trigger"] = *trigger; }
    }
    shortcuts.append({id, props});
  }

  QVariantMap options;
  options["handle_token"] = newToken("bind");

  const QDBusReply<QDBusObjectPath> reply =
      m_iface->call("BindShortcuts", QVariant::fromValue(QDBusObjectPath(m_sessionHandle)),
                    QVariant::fromValue(shortcuts), QString(), options);
  if (!reply.isValid()) {
    qWarning() << "GlobalShortcuts BindShortcuts failed:" << reply.error();
    return;
  }

  m_bus.connect("", reply.value().path(), REQUEST_IFACE, "Response", this,
                SLOT(onBindResponse(uint, QVariantMap)));
}

void GlobalShortcutPortalBackend::onBindResponse(uint response, const QVariantMap &results) {
  if (response != 0) {
    qWarning() << "GlobalShortcuts BindShortcuts rejected, response:" << response;
    return;
  }

  const QDBusArgument arg = results.value("shortcuts").value<QDBusArgument>();
  QList<PortalShortcut> bound;
  arg >> bound;

  for (const auto &shortcut : bound) {
    auto &info = m_info[shortcut.id];
    info.id = shortcut.id;
    info.status = GlobalShortcutStatus::Bound;
    info.triggerDisplay = shortcut.props.value("trigger_description").toString();
  }

  emit shortcutsChanged();
}

void GlobalShortcutPortalBackend::onActivated(const QDBusObjectPath &, const QString &shortcutId,
                                              qulonglong timestamp, const QVariantMap &) {
  emit shortcutActivated(shortcutId, timestamp);
}

void GlobalShortcutPortalBackend::closeSession() {
  if (m_sessionHandle.isEmpty()) return;
  QDBusInterface session(PORTAL_SERVICE, m_sessionHandle, SESSION_IFACE, m_bus);
  session.call("Close");
  m_sessionHandle.clear();
  m_sessionReady = false;
}

std::optional<GlobalShortcutInfo> GlobalShortcutPortalBackend::shortcut(const QString &id) const {
  if (auto it = m_info.find(id); it != m_info.end()) return it->second;
  return std::nullopt;
}
