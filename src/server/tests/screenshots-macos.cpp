#include <QFile>
#include <QTemporaryDir>
#include <catch2/catch_test_macros.hpp>
#include "macos-fixtures.hpp"
#include "services/screenshots/macos/screenshot-files.hpp"

TEST_CASE("Screenshot folder fallback uses metadata regardless of filename") {
  QTemporaryDir directory;
  REQUIRE(directory.isValid());
  const auto renamed = TestFixtures::makeImage(directory, "renamed image.png", true);
  TestFixtures::makeImage(directory, "Screenshot 2026-09-17.png", false);

  const auto result = MacosScreenshots::readFiles({}, directory.path(), false);
  REQUIRE_FALSE(result.error);
  REQUIRE(result.items.size() == 1);
  REQUIRE(result.items.front().path == renamed.toStdString());
  REQUIRE(result.items.front().size == QSize(32, 16));
}

TEST_CASE("Screen recordings are discovered through capture metadata without matching ordinary videos") {
  QTemporaryDir directory;
  REQUIRE(directory.isValid());
  const auto recording = TestFixtures::makeRecording(directory, "renamed recording.mov", true);
  TestFixtures::makeRecording(directory, "Screen Recording without metadata.mov", false);
  const auto incomplete = TestFixtures::makeRecording(directory, "incomplete.mov", true);
  QFile file(incomplete);
  REQUIRE(file.open(QIODevice::WriteOnly | QIODevice::Truncate));
  file.close();

  const auto result = MacosScreenshots::readFiles({}, directory.path(), false);
  REQUIRE_FALSE(result.error);
  REQUIRE(result.items.size() == 1);
  REQUIRE(result.items.front().path == recording.toStdString());
  REQUIRE(result.items.front().kind == Screenshot::Kind::Recording);
  REQUIRE_FALSE(result.latestImage());
}

TEST_CASE("Spotlight and folder scans do not duplicate recordings with normalized Unicode paths") {
  QTemporaryDir directory;
  REQUIRE(directory.isValid());
  const auto path = TestFixtures::makeRecording(
      directory, QString::fromUtf8("Enregistrement de l’écran.mov").normalized(QString::NormalizationForm_D),
      true);
  const auto spotlightPath = path.normalized(QString::NormalizationForm_C);
  REQUIRE(spotlightPath != path);
  const auto result =
      MacosScreenshots::readFiles({{.path = spotlightPath.toStdString()}}, directory.path(), false);
  REQUIRE(result.items.size() == 1);
  REQUIRE(result.items.front().kind == Screenshot::Kind::Recording);
}
