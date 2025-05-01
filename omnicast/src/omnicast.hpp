#pragma once
#include <filesystem>
#include <qdir.h>
#include <qstandardpaths.h>
#include <qsize.h>

namespace Omnicast {
std::filesystem::path runtimeDir();
std::filesystem::path commandSocketPath();
std::filesystem::path pidFile();
static const int TOP_BAR_HEIGHT = 55;
static const int STATUS_BAR_HEIGHT = 40;
static const QSize WINDOW_SIZE(800, 500);
} // namespace Omnicast
