#pragma once
#include <filesystem>
#include <vector>

/**
 * Used to resolve relative assets
 * Typically, extension commands push their asset path to the asset resolver and remove it
 * when they unload.
 */

class RelativeAssetResolver {
  std::vector<std::filesystem::path> m_paths;

public:
  void addPath(const std::filesystem::path &path) { m_paths.emplace_back(path); }
  void removePath(const std::filesystem::path &path) {
    if (auto it = std::ranges::find(m_paths, path); it != m_paths.end()) { m_paths.erase(it); }
  }

  std::optional<std::filesystem::path> resolve(const std::filesystem::path &relative) {
    for (const auto &base : m_paths) {
      std::filesystem::path path = base / relative;

      if (std::filesystem::exists(path)) return path;
    }

    return std::nullopt;
  }

  static RelativeAssetResolver *instance() {
    static RelativeAssetResolver resolver;

    return &resolver;
  }
};
