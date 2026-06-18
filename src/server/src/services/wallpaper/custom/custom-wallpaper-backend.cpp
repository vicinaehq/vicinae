#include "custom-wallpaper-backend.hpp"
#include "services/wallpaper/wallpaper-command.hpp"
#include "template-engine/template-engine.hpp"
#include <QProcess>

namespace {

QString fitName(WallpaperFit fit) {
  switch (fit) {
  case WallpaperFit::Cover:
    return "cover";
  case WallpaperFit::Contain:
    return "contain";
  case WallpaperFit::Stretch:
    return "stretch";
  case WallpaperFit::Center:
    return "center";
  case WallpaperFit::Tile:
    return "tile";
  }
  return "cover";
}

} // namespace

std::expected<void, std::string>
CustomCommandWallpaperBackend::setWallpaper(const WallpaperRequest &request) {
  if (m_command.empty()) return std::unexpected("No custom wallpaper command configured");

  TemplateEngine engine;
  engine.setVar("path", QString::fromStdString(request.path));
  engine.setVar("screen", request.screen ? QString::fromStdString(*request.screen) : QString{});
  engine.setVar("fit", fitName(request.fit));

  // Split into args first, then substitute per token, so a path with spaces stays a single argument.
  QStringList resolved;
  for (const auto &token : QProcess::splitCommand(QString::fromStdString(m_command))) {
    if (auto value = engine.build(token); !value.isEmpty()) { resolved << value; }
  }

  // Append the path if the template never referenced it (e.g. `feh --bg-fill`).
  if (!m_command.contains("{path}")) { resolved << QString::fromStdString(request.path); }

  if (resolved.isEmpty()) return std::unexpected("Custom wallpaper command is empty");

  const QString program = resolved.takeFirst();
  auto res = wallpaper::runCommand(program, resolved);
  if (!res) return std::unexpected(res.error());

  return {};
}
