#pragma once
#include <filesystem>
#include <qdir.h>
#include <qstandardpaths.h>
#include <qsize.h>
#include <set>
#include "theme/colors.hpp"

namespace Omnicast {

constexpr long long GB = 1e9;
constexpr long long IMAGE_DISK_CACHE_MAX_SIZE = GB * 5;
constexpr const int WINDOW_BORDER_WIDTH = 3;

static const QString GH_REPO = "https://github.com/vicinaehq/vicinae";
static const QString GH_REPO_CREATE_ISSUE = GH_REPO + "/issues/new";
static const QString GH_REPO_LICENSE = GH_REPO + "/blob/main/LICENSE";
static const QString GH_EXTENSIONS_CREATE_ISSUE = "https://github.com/vicinaehq/extensions/issues/new/choose";
static const QString MAIN_WINDOW_NAME = "Vicinae Launcher";
static const QString DOC_URL = "https://docs.vicinae.com";
static const QString DOC_TELEMETRY_URL = DOC_URL + "/telemetry";
static const QString HEADLINE = "A focused launcher for your desktop — native, fast, extensible";
static const QString APP_ID = "vicinae";
static const QString APP_SCHEME = APP_ID;
static const std::set<QString> APP_SCHEMES = {APP_SCHEME, "raycast", "com.raycast"};
static const QString DEFAULT_ICON_THEME_NAME = "vicinae";
static const QString VICINAE_NPM_API_PACKAGE = "@vicinae/api";
static const QString RAYCAST_NPM_API_PACKAGE = "@raycast/api";

/**
 * We use the http:// scheme instead of discord:// as we don't make assumptions
 * about whether discord is installed on the desktop.
 */
static const QString DISCORD_INVITE_LINK = "https://discord.gg/rP4ecD42p7";
static const QString GH_SPONSOR_LINK = "https://github.com/sponsors/vicinaehq";

static const SemanticColor ACCENT_COLOR = SemanticColor::Accent;

std::filesystem::path runtimeDir();
std::string commandSocketName();
std::filesystem::path pidFile();
std::filesystem::path dataDir();
std::filesystem::path stateDir();
std::filesystem::path configDir();
std::filesystem::path cacheDir();

std::filesystem::path dataHome();

// Read-only resources shipped with the application. On macOS this is
// Vicinae.app/Contents/Resources; on other platforms it is the install-prefix
// share directory (or empty if we cannot locate it).
std::filesystem::path bundleResourceDir();

// System-wide read-only directories that may contain Vicinae assets.
// macOS: { bundleResourceDir() }. Linux: $XDG_DATA_DIRS each suffixed with
// "vicinae". Empty entries are stripped.
std::vector<std::filesystem::path> systemDataDirs();

// Per-feature search paths in priority order: user dataDir() / subdir first,
// then each systemDataDirs() entry / subdir. Used by themes, extensions,
// scripts, etc.
std::vector<std::filesystem::path> dataSearchPaths(std::string_view subdir);

void ensureDirectories();
std::vector<std::filesystem::path> systemPaths();

static const int TOP_BAR_HEIGHT = 60;
static const int STATUS_BAR_HEIGHT = 40;

// In compact mode, only the search bar is shown when there is no input
static const QSize WINDOW_SIZE(770, 480);
} // namespace Omnicast
