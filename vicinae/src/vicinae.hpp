#pragma once
#include <filesystem>
#include <qdir.h>
#include <qstandardpaths.h>
#include <qsize.h>
#include <set>
#include "theme.hpp"

namespace Omnicast {

constexpr long long GB = 1e9;
constexpr long long IMAGE_DISK_CACHE_MAX_SIZE = GB * 5;
constexpr const int WINDOW_BORDER_WIDTH = 3;

static const QString GH_REPO = "https://github.com/vicinaehq/vicinae";
static const QString GH_REPO_CREATE_ISSUE = GH_REPO + "/issues/new";
static const QString GH_REPO_LICENSE = GH_REPO + "/blob/main/LICENSE";
static const QString MAIN_WINDOW_NAME = "Vicinae Launcher";
static const QString DOC_URL = "https://docs.vicinae.com";
static const QString HEADLINE = "A focused launcher for your desktop — native, fast, extensible";
static const QString APP_ID = "vicinae";
static const QString APP_SCHEME = APP_ID;
static const std::set<QString> APP_SCHEMES = {APP_SCHEME, "raycast"};
static const QString DEFAULT_FAVICON_SERVICE = "twenty";
static const QString DEFAULT_ICON_THEME_NAME = "vicinae";

static const QString VICINAE_NPM_API_PACKAGE = "@vicinae/api";
static const QString RAYCAST_NPM_API_PACKAGE = "@raycast/api";

/**
 * We use the http:// scheme instead of discord:// as we don't make assumptions
 * about whether discord is installed on the desktop.
 */
static const QString DISCORD_INVITE_LINK = "https://discord.gg/rP4ecD42p7";
static const QString GH_SPONSOR_LINK = "https://github.com/sponsors/vicinaehq";

static const SemanticColor ACCENT_COLOR = SemanticColor::Blue;

std::filesystem::path runtimeDir();
std::filesystem::path commandSocketPath();
std::filesystem::path pidFile();
std::filesystem::path dataDir();
std::filesystem::path configDir();

std::vector<std::filesystem::path> systemPaths();

/**
 * Reads XDG_CONFIG_HOME and XDG_CONFIG_DIRS and returns a vector such as
 * [XDG_CONFIG_HOME, ...XDG_CONFIG_DIRS].
 * Duplicate paths are automatically removed.
 * The order in which the paths have been scanned is preserved. For duplicate paths, only the
 * position of the first one is meaningful: subsequent occurences are fully ignored.
 */
std::vector<std::filesystem::path> xdgConfigDirs();
std::vector<std::filesystem::path> xdgDataDirs();

static const int TOP_BAR_HEIGHT = 60;
static const int STATUS_BAR_HEIGHT = 40;

// In compact mode, only the search bar is shown when there is no input
static const QSize WINDOW_SIZE(770, 480);
} // namespace Omnicast
