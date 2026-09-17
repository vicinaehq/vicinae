#pragma once

#include "services/screenshots/screenshot.hpp"

namespace MacosScreenshots {

ScreenshotResult readFiles(std::vector<Screenshot> candidates, const QString &directory, bool queryFailed);

} // namespace MacosScreenshots
