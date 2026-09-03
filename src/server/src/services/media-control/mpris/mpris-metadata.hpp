#pragma once
#include <QString>
#include <QVariantMap>
#include "services/media-control/abstract-media-control.hpp"

namespace mpris {

struct TrackInfo {
  QString title;
  QString artist;
};

/**
 * Reads the track of an MPRIS `Metadata` map.
 *
 * `xesam:artist` is specified as a string array but players are inconsistent about it:
 * some send a bare string, some only fill `xesam:albumArtist`.
 */
TrackInfo parseTrackInfo(const QVariantMap &metadata);

PlaybackStatus parsePlaybackStatus(QStringView status);

} // namespace mpris
