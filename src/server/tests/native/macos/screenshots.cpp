#include <QFile>
#include <QTemporaryDir>
#include <catch2/catch_test_macros.hpp>
#include "fixtures.hpp"
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

TEST_CASE("Screenshot results merge locations, remove duplicates and stale files, and sort by capture date") {
  QTemporaryDir directory;
  QTemporaryDir moved;
  REQUIRE(directory.isValid());
  REQUIRE(moved.isValid());
  const auto recent = TestFixtures::makeImage(directory, "recent.png", true);
  const auto old = TestFixtures::makeImage(moved, "moved.png", true);
  const auto now = QDateTime::fromString("2026-09-17T14:30:00Z", Qt::ISODate);
  std::vector<Screenshot> candidates{
      {.path = old.toStdString(), .createdAt = now.addDays(-1)},
      {.path = recent.toStdString(), .createdAt = now},
      {.path = directory.filePath("deleted.png").toStdString(), .createdAt = now.addDays(1)}};
  const auto result = MacosScreenshots::readFiles(std::move(candidates), directory.path(), false);
  REQUIRE(result.items.size() == 2);
  REQUIRE(result.items.front().path == recent.toStdString());
  REQUIRE(result.items.back().path == old.toStdString());
  REQUIRE(result.items.back().createdAt == now.addDays(-1));
}

TEST_CASE("Incomplete screenshots are skipped and Spotlight failure still returns folder results") {
  QTemporaryDir directory;
  REQUIRE(directory.isValid());
  const auto valid = TestFixtures::makeImage(directory, "valid.png", true);
  const auto incomplete = TestFixtures::makeImage(directory, "incomplete.png", true);
  QFile file(incomplete);
  REQUIRE(file.open(QIODevice::WriteOnly | QIODevice::Truncate));
  file.close();

  const auto result = MacosScreenshots::readFiles({}, directory.path(), true);
  REQUIRE(result.error);
  REQUIRE(result.items.size() == 1);
  REQUIRE(result.items.front().path == valid.toStdString());
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
