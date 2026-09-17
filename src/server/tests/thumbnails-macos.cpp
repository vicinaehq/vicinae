#include <QEventLoop>
#include <QFutureWatcher>
#include <QTemporaryDir>
#include <QTimer>
#include <catch2/catch_test_macros.hpp>
#include "macos-fixtures.hpp"
#include "ui/image/mac-file-thumbnail-loader.hpp"

TEST_CASE("Native recording thumbnails return a correctly oriented video frame") {
  QTemporaryDir directory;
  REQUIRE(directory.isValid());
  const auto path = TestFixtures::makeRecording(directory, "recording.mov", true);
  auto request = renderMacFileThumbnail(path, QSize(128, 128));
  QFutureWatcher<QImage> watcher;
  QEventLoop loop;
  QTimer timeout;
  timeout.setSingleShot(true);
  QObject::connect(&timeout, &QTimer::timeout, &loop, &QEventLoop::quit);
  QObject::connect(&watcher, &QFutureWatcher<QImage>::finished, &loop, &QEventLoop::quit);
  watcher.setFuture(request.future);
  timeout.start(10000);
  loop.exec();
  if (!watcher.isFinished()) request.cancel();
  REQUIRE(watcher.isFinished());
  const auto image = watcher.result();
  REQUIRE_FALSE(image.isNull());
  REQUIRE(image.width() <= 128);
  REQUIRE(image.height() <= 128);
  const auto top = image.pixelColor(image.width() / 2, image.height() / 4);
  const auto bottom = image.pixelColor(image.width() / 2, image.height() * 3 / 4);
  REQUIRE(top.red() > top.blue());
  REQUIRE(bottom.blue() > bottom.red());
}
