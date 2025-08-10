#pragma once
#include <filesystem>
#include <qdir.h>
#include <qstandardpaths.h>
#include <qsize.h>
#include "theme.hpp"

namespace Omnicast {

constexpr long long GB = 1e9;
constexpr long long IMAGE_DISK_CACHE_MAX_SIZE = GB * 5;

static const QString GH_REPO = "https://github.com/vicinaehq/vicinae";
static const QString GH_REPO_CREATE_ISSUE = GH_REPO + "/issues/new";
static const QString GH_REPO_LICENSE = GH_REPO + "/blob/main/LICENSE";
static const QString MAIN_WINDOW_NAME = "Vicinae Launcher";
static const QString DOC_URL = "https://docs.vicinae.com";
static const QString HEADLINE = "A focused launcher for your desktop — native, fast, extensible";
static const QString APP_ID = "vicinae";
static const QString APP_SCHEME = APP_ID;
static const std::array<QString, 2> APP_SCHEMES = {APP_SCHEME, "raycast"};

static const SemanticColor ACCENT_COLOR = SemanticColor::Red;

std::filesystem::path runtimeDir();
std::filesystem::path commandSocketPath();
std::filesystem::path pidFile();
std::filesystem::path dataDir();
std::filesystem::path configDir();

static const int TOP_BAR_HEIGHT = 60;
static const int STATUS_BAR_HEIGHT = 40;

// In compact mode, only the search bar is shown when there is no input
static const QSize WINDOW_SIZE(770, 480);
} // namespace Omnicast
