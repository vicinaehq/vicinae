#pragma once
#include "services/wallpaper/abstract-wallpaper-backend.hpp"
#include <string>
#include <utility>

// Runs the user-configured `wallpaper.command`, overriding the built-in backends when set.
class CustomCommandWallpaperBackend : public AbstractWallpaperBackend {
public:
  explicit CustomCommandWallpaperBackend(std::string command) : m_command(std::move(command)) {}

  std::string id() const override { return "custom"; }
  bool isActivatable() const override { return !m_command.empty(); }
  bool supportsPerScreen() const override { return m_command.contains("{screen}"); }
  std::expected<void, std::string> setWallpaper(const WallpaperRequest &request) override;

private:
  std::string m_command;
};
