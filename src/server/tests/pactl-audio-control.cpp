#include <catch2/catch_test_macros.hpp>
#include <QFile>
#include <QTemporaryDir>
#include "services/audio-control/pactl/pactl-audio-control.hpp"

// NOLINTBEGIN(bugprone-throwing-static-initialization)
TEST_CASE("pactl keeps the default sink when a virtual sink has no active port") {
  const QTemporaryDir dir;
  REQUIRE(dir.isValid());

  const auto mockPath = dir.filePath("pactl");
  QFile mock(mockPath);
  REQUIRE(mock.open(QIODevice::WriteOnly));
  REQUIRE(mock.write(R"(#!/bin/sh
case "$1" in
  get-default-sink) printf 'physical\n' ;;
  --format=json) cat <<'JSON'
[
  {"name":"virtual","description":"Virtual sink","mute":false,"volume":{"front-left":{"value_percent":"27%"}},"ports":[],"active_port":null},
  {"name":"physical","description":"Speakers","mute":false,"volume":{"front-left":{"value_percent":"41%"}},"ports":[{"name":"speaker-output","description":"Speaker"}],"active_port":"speaker-output"}
]
JSON
    ;;
esac
)") > 0);
  mock.close();
  REQUIRE(QFile::setPermissions(mockPath,
                                QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner));

  const auto previousPath = qgetenv("PATH");
  qputenv("PATH", QFile::encodeName(dir.path()) + ':' + previousPath);
  const PactlAudioControl audio;
  qputenv("PATH", previousPath);

  const auto volume = audio.getVolume();
  CHECK(volume > 0.409f);
  CHECK(volume < 0.411f);
}
// NOLINTEND(bugprone-throwing-static-initialization)
