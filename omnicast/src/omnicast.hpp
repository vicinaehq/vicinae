#pragma once
#include <filesystem>
#include <qdir.h>
#include <qstandardpaths.h>
#include <qsize.h>

namespace Omnicast {

constexpr long long GB = 1e9;
constexpr long long IMAGE_DISK_CACHE_MAX_SIZE = GB * 5;

std::filesystem::path runtimeDir();
std::filesystem::path commandSocketPath();
std::filesystem::path pidFile();
std::filesystem::path dataDir();
std::filesystem::path configDir();

static const int TOP_BAR_HEIGHT = 60;
static const int STATUS_BAR_HEIGHT = 40;
static const QSize WINDOW_SIZE(770, 480);
} // namespace Omnicast
