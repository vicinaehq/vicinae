#include "theme-file.hpp"
#include "lib/toml.hpp"
#include <fstream>

namespace fs = std::filesystem;

static const std::array<const char *, 16> base16Keys = {
    "base00", "base01", "base02", "base03", "base04", "base05", "base06", "base07",
    "base08", "base09", "base0A", "base0B", "base0C", "base0D", "base0E", "base0F"};

static const std::array<const char *, 8> base24Keys = {
    "base10", "base11", "base12", "base13", "base14", "base15", "base16", "base17",
};

tl::expected<ThemeFile, QString> ThemeFile::fromFile(const fs::path &path) {
  auto file = toml::parse_file(path.c_str());
  auto tints = file["tints"].as_table();

  if (!tints) { return tl::unexpected("a [tints] table is required"); }

  for (const auto &key : base16Keys) {
    auto value = (*tints)[key].as_string();
    if (!value) return tl::unexpected(QString("tints.%1 is required").arg(key));
  }

  for (const auto &key : base24Keys) {}

  return ThemeFile(path);
}

ThemeFile::ThemeFile(const fs::path &path) {}
