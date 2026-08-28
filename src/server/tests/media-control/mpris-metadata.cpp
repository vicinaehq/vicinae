#include "services/media-control/mpris/mpris-metadata.hpp"
#include <catch2/catch_test_macros.hpp>

namespace {

QVariantMap trackMetadata(const QVariant &artist) {
  return {{"mpris:trackid", "/track/1"}, {"xesam:title", "Cygnus X-1"}, {"xesam:artist", artist}};
}

} // namespace

TEST_CASE("parseTrackInfo: reads title and artist array") {
  const auto info = mpris::parseTrackInfo(trackMetadata(QStringList{"Rush"}));

  REQUIRE(info.title == "Cygnus X-1");
  REQUIRE(info.artist == "Rush");
}

TEST_CASE("parseTrackInfo: joins multiple artists") {
  const auto info = mpris::parseTrackInfo(trackMetadata(QStringList{"Rush", "Geddy Lee"}));

  REQUIRE(info.artist == "Rush, Geddy Lee");
}

TEST_CASE("parseTrackInfo: accepts an artist list of variants") {
  const auto info = mpris::parseTrackInfo(trackMetadata(QVariantList{"Rush", "Geddy Lee"}));

  REQUIRE(info.artist == "Rush, Geddy Lee");
}

TEST_CASE("parseTrackInfo: accepts a bare artist string") {
  const auto info = mpris::parseTrackInfo(trackMetadata("Rush"));

  REQUIRE(info.artist == "Rush");
}

TEST_CASE("parseTrackInfo: skips empty artist entries") {
  const auto info = mpris::parseTrackInfo(trackMetadata(QStringList{"", "Rush", ""}));

  REQUIRE(info.artist == "Rush");
}

TEST_CASE("parseTrackInfo: falls back to the album artist") {
  QVariantMap metadata = trackMetadata(QStringList{});

  metadata["xesam:albumArtist"] = QStringList{"Rush"};

  REQUIRE(mpris::parseTrackInfo(metadata).artist == "Rush");
}

TEST_CASE("parseTrackInfo: tolerates missing keys") {
  const auto info = mpris::parseTrackInfo({});

  REQUIRE(info.title.isEmpty());
  REQUIRE(info.artist.isEmpty());
}

TEST_CASE("parsePlaybackStatus: maps the spec values") {
  REQUIRE(mpris::parsePlaybackStatus(u"Playing") == PlaybackStatus::Playing);
  REQUIRE(mpris::parsePlaybackStatus(u"Paused") == PlaybackStatus::Paused);
  REQUIRE(mpris::parsePlaybackStatus(u"Stopped") == PlaybackStatus::Stopped);
}

TEST_CASE("parsePlaybackStatus: unknown values are stopped") {
  REQUIRE(mpris::parsePlaybackStatus(u"") == PlaybackStatus::Stopped);
  REQUIRE(mpris::parsePlaybackStatus(u"playing") == PlaybackStatus::Stopped);
}
