#include "theme-file.hpp"
#include "lib/toml.hpp"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

static const std::unordered_map<ThemeFileTint, const char *> tintToKey = {
    {ThemeFileTint::Base00, "base00"}, {ThemeFileTint::Base01, "base01"}, {ThemeFileTint::Base02, "base02"},
    {ThemeFileTint::Base03, "base03"}, {ThemeFileTint::Base04, "base04"}, {ThemeFileTint::Base05, "base05"},
    {ThemeFileTint::Base06, "base06"}, {ThemeFileTint::Base07, "base07"}, {ThemeFileTint::Base08, "base08"},
    {ThemeFileTint::Base09, "base09"}, {ThemeFileTint::Base0A, "base0A"}, {ThemeFileTint::Base0B, "base0B"},
    {ThemeFileTint::Base0C, "base0C"}, {ThemeFileTint::Base0D, "base0D"}, {ThemeFileTint::Base0E, "base0E"},
    {ThemeFileTint::Base0F, "base0F"},

    {ThemeFileTint::Base10, "base10"}, {ThemeFileTint::Base11, "base11"}, {ThemeFileTint::Base12, "base12"},
    {ThemeFileTint::Base13, "base13"}, {ThemeFileTint::Base14, "base14"}, {ThemeFileTint::Base15, "base15"},
    {ThemeFileTint::Base16, "base16"}, {ThemeFileTint::Base17, "base17"},
};

static const std::unordered_map<const char *, ThemeFileTint> keyToTint = {
    {"base00", ThemeFileTint::Base00}, {"base01", ThemeFileTint::Base01}, {"base02", ThemeFileTint::Base02},
    {"base03", ThemeFileTint::Base03}, {"base04", ThemeFileTint::Base04}, {"base05", ThemeFileTint::Base05},
    {"base06", ThemeFileTint::Base06}, {"base07", ThemeFileTint::Base07}, {"base08", ThemeFileTint::Base08},
    {"base09", ThemeFileTint::Base09}, {"base0A", ThemeFileTint::Base0A}, {"base0B", ThemeFileTint::Base0B},
    {"base0C", ThemeFileTint::Base0C}, {"base0D", ThemeFileTint::Base0D}, {"base0E", ThemeFileTint::Base0E},
    {"base0F", ThemeFileTint::Base0F}, {"base10", ThemeFileTint::Base10}, {"base11", ThemeFileTint::Base11},
    {"base12", ThemeFileTint::Base12}, {"base13", ThemeFileTint::Base13}, {"base14", ThemeFileTint::Base14},
    {"base15", ThemeFileTint::Base15}, {"base16", ThemeFileTint::Base16}, {"base17", ThemeFileTint::Base17},
};

namespace fs = std::filesystem;

static const std::array<ThemeFileTint, 16> base16Keys = {
    ThemeFileTint::Base00, ThemeFileTint::Base01, ThemeFileTint::Base02, ThemeFileTint::Base03,
    ThemeFileTint::Base04, ThemeFileTint::Base05, ThemeFileTint::Base06, ThemeFileTint::Base07,
    ThemeFileTint::Base08, ThemeFileTint::Base09, ThemeFileTint::Base0A, ThemeFileTint::Base0B,
    ThemeFileTint::Base0C, ThemeFileTint::Base0D, ThemeFileTint::Base0E, ThemeFileTint::Base0F};

static const std::array<ThemeFileTint, 8> base24Keys = {
    ThemeFileTint::Base10, ThemeFileTint::Base11, ThemeFileTint::Base12, ThemeFileTint::Base13,
    ThemeFileTint::Base14, ThemeFileTint::Base15, ThemeFileTint::Base16, ThemeFileTint::Base17};

static QColor parseColor(toml::node_view<toml::node> node) {
  if (auto str = node.as_string()) { return QColor(str->value_or("")); }

  if (auto table = node.as_table()) {
    QColor color;

    if (auto name = (*table)["name"].as_string()) { color = name->value_or(""); }
    if (auto opacity = (*table)["opacity"].as_floating_point()) { color.setAlphaF(opacity->value_or(1)); }

    return color;
  }

  return QColor();
}

tl::expected<ThemeFile, QString> ThemeFile::fromFile(const fs::path &path) {
  ThemeFile themeFile(path);
  auto file = toml::parse_file(path.c_str());

  auto metaPtr = file["meta"].as_table();

  if (!metaPtr) { return tl::unexpected("a [meta] table is required"); }

  auto &meta = *metaPtr;

  themeFile.m_name = QString::fromStdString(meta["name"].value<std::string>().value_or(""));
  themeFile.m_description = QString::fromStdString(meta["description"].value<std::string>().value_or(""));

  if (auto ptr = meta["icon"].as_string()) {
    std::string icon = ptr->value_or("");
    fs::path iconPath = icon;
    std::error_code ec;

    if (icon.starts_with('.')) { iconPath = path / icon; }
    if (!fs::is_regular_file(icon, ec)) {
      return tl::unexpected(QString("%1 does not point to a valid file").arg(iconPath.c_str()));
    }
  }

  auto tints = file["tints"].as_table();

  if (!tints) { return tl::unexpected("a [tints] table is required"); }

  for (const auto &key : base16Keys) {
    if (auto it = tintToKey.find(key); it != tintToKey.end()) {
      auto value = (*tints)[it->second];
      if (!tints->contains(it->second)) {
        return tl::unexpected(QString("tints.%1 is required").arg(it->second));
      }
      QColor color = parseColor(value);

      if (!color.isValid()) {
        return tl::unexpected(QString("tints.%1 is not a valid color").arg(it->second));
      }
    }
  }

  for (const auto &key : base24Keys) {
    if (auto it = tintToKey.find(key); it != tintToKey.end()) {
      auto value = (*tints)[it->second].as_string();
      if (!value) {}
    }
  }

  return ThemeFile(path);
}

ThemeFile::ThemeFile(const fs::path &path) {}
