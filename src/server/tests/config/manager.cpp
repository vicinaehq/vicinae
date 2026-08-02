#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <QTemporaryDir>
#include <catch2/catch_test_macros.hpp>
#include "config/config.hpp"

namespace Omnicast {
std::filesystem::path configDir() { return {}; }
} // namespace Omnicast

TEST_CASE("persists and prunes a standalone hotkey exclusion") {
  QTemporaryDir const directory;
  REQUIRE(directory.isValid());

  std::filesystem::path const configPath =
      std::filesystem::path{directory.path().toStdString()} / "settings.json";
  EntrypointId const moonlight{"applications", "com.moonlight-stream.Moonlight"};

  {
    config::Manager manager{configPath};
    REQUIRE(manager.mergeEntrypointWithUser(moonlight, {.hotkeyExcluded = true}));
  }

  {
    std::ifstream configFile{configPath};
    REQUIRE(configFile);
    std::string const configText{std::istreambuf_iterator<char>{configFile},
                                 std::istreambuf_iterator<char>{}};
    REQUIRE(configText.contains("\"hotkey_excluded\""));
    REQUIRE_FALSE(configText.contains("\"hotkeyExcluded\""));
  }

  {
    config::Manager const reloaded{configPath};
    auto const provider = reloaded.value().providers.find(moonlight.provider);
    REQUIRE(provider != reloaded.value().providers.end());

    auto const entrypoint = provider->second.entrypoints.find(moonlight.entrypoint);
    REQUIRE(entrypoint != provider->second.entrypoints.end());
    REQUIRE(entrypoint->second.hotkeyExcluded.value_or(false));
  }

  {
    config::Manager manager{configPath};
    REQUIRE(manager.mergeEntrypointWithUser(moonlight, {.hotkeyExcluded = false}));
  }

  std::ifstream configFile{configPath};
  REQUIRE(configFile);
  std::string const configText{std::istreambuf_iterator<char>{configFile}, std::istreambuf_iterator<char>{}};
  REQUIRE_FALSE(configText.contains("hotkey_excluded"));
  REQUIRE_FALSE(configText.contains(moonlight.entrypoint));
}
