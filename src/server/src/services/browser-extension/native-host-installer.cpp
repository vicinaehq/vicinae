#include <QtGlobal>
#include <filesystem>
#include <format>
#include <fstream>
#include <glaze/glaze.hpp>
#include <qlogging.h>
#include <string>
#include <vector>

#include "common/common.hpp"
#include "services/browser-extension/native-host-installer.hpp"
#include "utils/utils.hpp"

#ifndef Q_OS_MACOS
#include "xdgpp/env/env.hpp"
#endif

namespace fs = std::filesystem;

namespace vicinae::browser {

struct ChromiumManifest {
  std::string name;
  std::string description;
  std::string path;
  std::string type;
  std::vector<std::string> allowed_origins;
};

struct GeckoManifest {
  std::string name;
  std::string description;
  std::string path;
  std::string type;
  std::vector<std::string> allowed_extensions;
};

}; // namespace vicinae::browser

namespace {

constexpr const char *MANIFEST_FILENAME = NATIVE_MESSAGING_HOST ".json";
constexpr const char *MANIFEST_DESCRIPTION = "Vicinae Native Messaging Host";

struct ManifestTarget {
  fs::path probe;
  fs::path manifestDir;
  const std::string *content = nullptr;
};

std::vector<ManifestTarget> manifestTargets(const std::string &chromium, const std::string &gecko) {
  std::vector<ManifestTarget> targets;

#ifdef Q_OS_MACOS
  const fs::path base = homeDir() / "Library" / "Application Support";

  constexpr auto CHROMIUM_DATA_DIRS = std::array{
      "Google/Chrome",
      "Google/Chrome Beta",
      "Google/Chrome Canary",
      "Chromium",
      "Thorium",
      "BraveSoftware/Brave-Browser",
      "Microsoft Edge",
      "Vivaldi",
      "com.operasoftware.Opera",
      "Arc/User Data",
      "Helium",
  };

  targets.reserve(std::size(CHROMIUM_DATA_DIRS) + 6);

  for (const auto &dir : CHROMIUM_DATA_DIRS) {
    targets.emplace_back(base / dir, base / dir / "NativeMessagingHosts", &chromium);
  }

  // gecko forks read the shared Mozilla path unless patched (librewolf); zen: zen-browser/desktop#10622
  const fs::path mozilla = base / "Mozilla" / "NativeMessagingHosts";

  targets.emplace_back(base / "Firefox", mozilla, &gecko);
  targets.emplace_back(base / "Waterfox", mozilla, &gecko);
  targets.emplace_back(base / "Floorp", mozilla, &gecko);
  targets.emplace_back(base / "zen", mozilla, &gecko);
  targets.emplace_back(base / "zen", base / "zen" / "NativeMessagingHosts", &gecko);
  targets.emplace_back(base / "LibreWolf", base / "LibreWolf" / "NativeMessagingHosts", &gecko);
#else
  const fs::path cfg = xdgpp::configHome();
  const fs::path home = homeDir();

  constexpr std::string_view CHROMIUM_DATA_DIRS[] = {
      "google-chrome",
      "google-chrome-beta",
      "google-chrome-unstable",
      "chromium",
      "thorium",
      "BraveSoftware/Brave-Browser",
      "microsoft-edge",
      "vivaldi",
      "opera",
      "net.imput.helium",
  };

  targets.reserve(std::size(CHROMIUM_DATA_DIRS) + 6);

  for (const auto &dir : CHROMIUM_DATA_DIRS) {
    targets.emplace_back(cfg / dir, cfg / dir / "NativeMessagingHosts", &chromium);
  }

  const fs::path mozilla = home / ".mozilla" / "native-messaging-hosts";

  targets.emplace_back(home / ".mozilla", mozilla, &gecko);
  targets.emplace_back(home / ".waterfox", mozilla, &gecko);
  targets.emplace_back(home / ".floorp", mozilla, &gecko);
  targets.emplace_back(home / ".zen", mozilla, &gecko);
  targets.emplace_back(home / ".zen", home / ".zen" / "native-messaging-hosts", &gecko);
  targets.emplace_back(home / ".librewolf", home / ".librewolf" / "native-messaging-hosts", &gecko);
#endif

  return targets;
}

void writeManifest(const ManifestTarget &target) {
  std::error_code ec;

  if (!fs::is_directory(target.probe, ec)) return;

  const auto file = target.manifestDir / MANIFEST_FILENAME;

  if (std::ifstream ifs(file); ifs && vicinae::slurp(ifs) == *target.content) return;

  fs::create_directories(target.manifestDir, ec);

  std::ofstream ofs(file, std::ios::trunc);

  if (!ofs || !(ofs << *target.content)) {
    qInfo() << "browser-link: skipped unwritable manifest" << file.c_str();
    return;
  }

  qInfo() << "browser-link: installed manifest" << file.c_str();
}

}; // namespace

namespace vicinae::browser {

void installNativeHostManifests() {
  const auto bin = findHelperProgram("vicinae-browser-link");

  if (!bin) {
    qWarning() << "browser-link: helper binary not found, skipping manifest installation";
    return;
  }

#ifdef Q_OS_MACOS
  // Gatekeeper translocation runs the app from a throwaway randomized path
  if (selfPath().string().starts_with("/private/var/folders")) {
    qInfo() << "browser-link: app is translocated, skipping manifest installation";
    return;
  }
#endif

  const ChromiumManifest chromium{
      .name = NATIVE_MESSAGING_HOST,
      .description = MANIFEST_DESCRIPTION,
      .path = bin->string(),
      .type = "stdio",
      .allowed_origins = {std::format("chrome-extension://{}/", CHROME_EXTENSION_ID)},
  };
  const GeckoManifest gecko{
      .name = NATIVE_MESSAGING_HOST,
      .description = MANIFEST_DESCRIPTION,
      .path = bin->string(),
      .type = "stdio",
      .allowed_extensions = {FIREFOX_EXTENSION_ID},
  };

  const auto chromiumJson = glz::write<glz::opts{.prettify = true}>(chromium);
  const auto geckoJson = glz::write<glz::opts{.prettify = true}>(gecko);

  if (!chromiumJson || !geckoJson) {
    qWarning() << "browser-link: failed to serialize native messaging manifests";
    return;
  }

  for (const auto &target : manifestTargets(*chromiumJson, *geckoJson)) {
    writeManifest(target);
  }
}

}; // namespace vicinae::browser
