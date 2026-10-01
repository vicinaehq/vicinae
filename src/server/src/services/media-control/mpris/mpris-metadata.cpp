#include <QDBusArgument>
#include <QStringList>
#include "services/media-control/mpris/mpris-metadata.hpp"

namespace {

constexpr auto TITLE_KEY = QLatin1StringView("xesam:title");
constexpr auto ARTIST_KEY = QLatin1StringView("xesam:artist");
constexpr auto ALBUM_ARTIST_KEY = QLatin1StringView("xesam:albumArtist");

QStringList toStringList(const QVariant &value) {
  // string arrays nested in a variant reach us as an undemarshalled argument
  if (value.canConvert<QDBusArgument>()) return qdbus_cast<QStringList>(value);
  if (value.typeId() == QMetaType::QString) return {value.toString()};

  return value.toStringList();
}

QString joinArtists(const QVariant &value) {
  auto artists = toStringList(value);

  artists.removeAll(QString{});

  return artists.join(QStringLiteral(", "));
}

} // namespace

mpris::TrackInfo mpris::parseTrackInfo(const QVariantMap &metadata) {
  TrackInfo info{.title = metadata.value(TITLE_KEY).toString(),
                 .artist = joinArtists(metadata.value(ARTIST_KEY))};

  if (info.artist.isEmpty()) { info.artist = joinArtists(metadata.value(ALBUM_ARTIST_KEY)); }

  return info;
}

PlaybackStatus mpris::parsePlaybackStatus(QStringView status) {
  if (status == QLatin1StringView("Playing")) return PlaybackStatus::Playing;
  if (status == QLatin1StringView("Paused")) return PlaybackStatus::Paused;

  return PlaybackStatus::Stopped;
}
