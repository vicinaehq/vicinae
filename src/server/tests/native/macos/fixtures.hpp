#pragma once

#include <QDir>
#include <QFile>
#include <QImage>
#include <QTemporaryDir>
#include <catch2/catch_test_macros.hpp>
#include <string_view>
#include <sys/xattr.h>

namespace TestFixtures {

inline QString makeImage(const QTemporaryDir &directory, const QString &name, bool screenshot) {
  const auto path = directory.filePath(name);
  QImage image(32, 16, QImage::Format_RGB32);
  image.fill(Qt::red);
  REQUIRE(image.save(path));
  if (screenshot) {
    constexpr std::string_view ATTRIBUTE = "<plist version=\"1.0\"><true/></plist>";
    REQUIRE(setxattr(QFile::encodeName(path).constData(), "com.apple.metadata:kMDItemIsScreenCapture",
                     ATTRIBUTE.data(), ATTRIBUTE.size(), 0, 0) == 0);
  }
  return path;
}

inline QString makeRecording(const QTemporaryDir &directory, const QString &name, bool capture) {
  const auto path = directory.filePath(name);
  const auto fixture = QDir(QString::fromUtf8(VICINAE_TEST_FIXTURE_DIR)).filePath("screen-recording.mov");
  REQUIRE(QFile::copy(fixture, path));
  if (capture) {
    constexpr std::string_view ATTRIBUTE = "<plist version=\"1.0\"><string>display</string></plist>";
    REQUIRE(setxattr(QFile::encodeName(path).constData(), "com.apple.metadata:kMDItemScreenCaptureType",
                     ATTRIBUTE.data(), ATTRIBUTE.size(), 0, 0) == 0);
  }
  return path;
}

} // namespace TestFixtures
