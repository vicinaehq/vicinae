#include <algorithm>
#include <ranges>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDebug>
#include "services/media-control/mpris/mpris-media-control.hpp"
#include "services/media-control/mpris/mpris-metadata.hpp"

namespace {

constexpr auto BUS_SERVICE = QLatin1StringView("org.freedesktop.DBus");
constexpr auto BUS_PATH = QLatin1StringView("/org/freedesktop/DBus");
constexpr auto BUS_INTERFACE = QLatin1StringView("org.freedesktop.DBus");
constexpr auto PROPERTIES_INTERFACE = QLatin1StringView("org.freedesktop.DBus.Properties");

constexpr auto MPRIS_PREFIX = QLatin1StringView("org.mpris.MediaPlayer2.");
constexpr auto MPRIS_PATH = QLatin1StringView("/org/mpris/MediaPlayer2");
constexpr auto MPRIS_INTERFACE = QLatin1StringView("org.mpris.MediaPlayer2");
constexpr auto MPRIS_PLAYER_INTERFACE = QLatin1StringView("org.mpris.MediaPlayer2.Player");

// players are third party programs: an unresponsive one must not hang the launcher
constexpr int CALL_TIMEOUT_MS = 1000;

bool isPlayerName(QStringView service) { return service.startsWith(MPRIS_PREFIX); }

} // namespace

MprisMediaControl::MprisMediaControl() : m_bus(QDBusConnection::sessionBus()) {
  if (!m_bus.isConnected()) {
    qWarning() << "session bus is not available, media control will not work";
    return;
  }

  m_bus.connect(BUS_SERVICE, BUS_PATH, BUS_INTERFACE, "NameOwnerChanged", this,
                SLOT(onNameOwnerChanged(QString, QString, QString)));
  // no service filter: we want the signal from every player sitting on the MPRIS object path
  m_bus.connect({}, MPRIS_PATH, PROPERTIES_INTERFACE, "PropertiesChanged", this,
                SLOT(onPropertiesChanged(QString, QVariantMap, QStringList)));

  if (auto *iface = m_bus.interface()) {
    m_services = iface->registeredServiceNames().value() | std::views::filter(isPlayerName) |
                 std::ranges::to<std::vector>();
  }
}

QString MprisMediaControl::id() const { return "mpris"; }

std::vector<MediaPlayer> MprisMediaControl::players() const {
  std::vector<MediaPlayer> players;

  players.reserve(m_services.size());

  for (const auto &service : m_services) {
    if (auto player = queryPlayer(service)) { players.emplace_back(std::move(*player)); }
  }

  return players;
}

bool MprisMediaControl::playPause(const QString &playerId) { return callPlayer(playerId, "PlayPause"); }

bool MprisMediaControl::next(const QString &playerId) { return callPlayer(playerId, "Next"); }

bool MprisMediaControl::previous(const QString &playerId) { return callPlayer(playerId, "Previous"); }

void MprisMediaControl::onNameOwnerChanged(const QString &service, const QString &, const QString &newOwner) {
  if (!isPlayerName(service)) return;

  const auto it = std::ranges::find(m_services, service);

  if (newOwner.isEmpty()) {
    if (it == m_services.end()) return;
    m_services.erase(it);
  } else {
    if (it != m_services.end()) return;
    m_services.emplace_back(service);
  }

  emit playersChanged();
}

void MprisMediaControl::onPropertiesChanged(const QString &interface, const QVariantMap &,
                                            const QStringList &) {
  if (interface == MPRIS_PLAYER_INTERFACE) emit playersChanged();
}

std::optional<QVariantMap> MprisMediaControl::getAllProperties(const QString &service,
                                                               const QString &interface) const {
  auto msg = QDBusMessage::createMethodCall(service, MPRIS_PATH, PROPERTIES_INTERFACE, "GetAll");

  msg << interface;

  const auto reply = m_bus.call(msg, QDBus::Block, CALL_TIMEOUT_MS);

  if (reply.type() == QDBusMessage::ErrorMessage) {
    qWarning() << "failed to read" << interface << "properties of" << service << ":" << reply.errorMessage();
    return std::nullopt;
  }

  if (reply.arguments().isEmpty()) return std::nullopt;

  return qdbus_cast<QVariantMap>(reply.arguments().constFirst());
}

std::optional<MediaPlayer> MprisMediaControl::queryPlayer(const QString &service) const {
  const auto props = getAllProperties(service, MPRIS_PLAYER_INTERFACE);

  if (!props) return std::nullopt;

  auto track = mpris::parseTrackInfo(qdbus_cast<QVariantMap>(props->value("Metadata")));
  MediaPlayer player{
      .id = service,
      .identity = service.sliced(MPRIS_PREFIX.size()),
      .title = std::move(track.title),
      .artist = std::move(track.artist),
      .status = mpris::parsePlaybackStatus(props->value("PlaybackStatus").toString()),
      .canGoNext = props->value("CanGoNext").toBool(),
      .canGoPrevious = props->value("CanGoPrevious").toBool(),
  };

  if (const auto root = getAllProperties(service, MPRIS_INTERFACE)) {
    if (auto identity = root->value("Identity").toString(); !identity.isEmpty()) {
      player.identity = std::move(identity);
    }
  }

  return player;
}

bool MprisMediaControl::callPlayer(const QString &playerId, const QString &method) {
  const auto msg = QDBusMessage::createMethodCall(playerId, MPRIS_PATH, MPRIS_PLAYER_INTERFACE, method);
  const auto reply = m_bus.call(msg, QDBus::Block, CALL_TIMEOUT_MS);

  if (reply.type() == QDBusMessage::ErrorMessage) {
    qWarning() << "mpris" << method << "failed on" << playerId << ":" << reply.errorMessage();
    return false;
  }

  setLastPlayerId(playerId);

  return true;
}
