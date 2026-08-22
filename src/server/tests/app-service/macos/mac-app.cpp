#include "services/app-service/macos/mac-app.hpp"

#include <catch2/catch_test_macros.hpp>

#include <QTemporaryDir>

#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>

namespace fs = std::filesystem;

ImageURL::ImageURL() = default;

ImageURL ImageURL::macBundle(const fs::path &) { return {}; }

namespace {

void writeFile(const fs::path &path, std::string_view content) {
  fs::create_directories(path.parent_path());
  std::ofstream stream(path);
  REQUIRE(stream.is_open());
  stream << content;
}

fs::path createAppBundle(const QTemporaryDir &temporaryDirectory, std::string_view name,
                         std::optional<std::string_view> bundleIdentifier = std::nullopt,
                         bool createExecutable = true) {
  fs::path const bundlePath =
      fs::path(temporaryDirectory.path().toStdString()) / (std::string(name) + ".app");
  fs::path const executablePath = bundlePath / "Contents" / "MacOS" / "run.sh";

  std::string plist = R"(<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
  <key>CFBundleExecutable</key>
  <string>run.sh</string>
  <key>CFBundleName</key>
  <string>)";
  plist += name;
  plist += "</string>\n";
  if (bundleIdentifier) {
    plist += "  <key>CFBundleIdentifier</key>\n  <string>";
    plist += *bundleIdentifier;
    plist += "</string>\n";
  }
  plist += R"(  <key>CFBundlePackageType</key>
  <string>APPL</string>
</dict>
</plist>
)";

  writeFile(bundlePath / "Contents" / "Info.plist", plist);
  if (createExecutable) {
    writeFile(executablePath, "#!/bin/sh\nexit 0\n");
    fs::permissions(executablePath, fs::perms::owner_exec, fs::perm_options::add);
  }
  return bundlePath;
}

} // namespace

TEST_CASE("valid macOS app bundle without an identifier is accepted") {
  const QTemporaryDir temporaryDirectory;
  REQUIRE(temporaryDirectory.isValid());

  fs::path const bundlePath = createAppBundle(temporaryDirectory, "Identifierless");

  auto app = MacApplication::fromBundle(bundlePath);

  REQUIRE(app != nullptr);
  QString const expectedId =
      QStringLiteral("macos:path:") + QString::fromStdString(fs::canonical(bundlePath).string());
  INFO("actual id: " << app->id().toStdString());
  INFO("expected id: " << expectedId.toStdString());
  REQUIRE(app->id() == expectedId);
  REQUIRE(app->displayName() == QStringLiteral("Identifierless"));
  REQUIRE(app->program() == QStringLiteral("run.sh"));
  REQUIRE_FALSE(app->windowClass().has_value());
  REQUIRE_FALSE(app->matchesWindowClass(app->id()));
}

TEST_CASE("macOS app bundle identifier remains its application id") {
  const QTemporaryDir temporaryDirectory;
  REQUIRE(temporaryDirectory.isValid());

  fs::path const bundlePath = createAppBundle(temporaryDirectory, "Identified", "com.example.Identified");

  auto app = MacApplication::fromBundle(bundlePath);

  REQUIRE(app != nullptr);
  REQUIRE(app->id() == QStringLiteral("com.example.Identified"));
  REQUIRE(app->windowClass() == QStringLiteral("com.example.Identified"));
  REQUIRE(app->matchesWindowClass(QStringLiteral("COM.EXAMPLE.IDENTIFIED")));
}

TEST_CASE("macOS app bundle without an identifier requires an executable") {
  const QTemporaryDir temporaryDirectory;
  REQUIRE(temporaryDirectory.isValid());

  fs::path const bundlePath = createAppBundle(temporaryDirectory, "MissingExecutable", std::nullopt, false);

  REQUIRE(MacApplication::fromBundle(bundlePath) == nullptr);
}
