#include "theme-file.hpp"
#include "theme.hpp"
#include <filesystem>
#include <fstream>
#include <qlogging.h>

namespace fs = std::filesystem;

static const std::unordered_map<ThemeFileTint, std::string> tintToKey = {
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

static const std::unordered_map<std::string, ThemeFileTint> keyToTint = {
    {"base00", ThemeFileTint::Base00}, {"base01", ThemeFileTint::Base01}, {"base02", ThemeFileTint::Base02},
    {"base03", ThemeFileTint::Base03}, {"base04", ThemeFileTint::Base04}, {"base05", ThemeFileTint::Base05},
    {"base06", ThemeFileTint::Base06}, {"base07", ThemeFileTint::Base07}, {"base08", ThemeFileTint::Base08},
    {"base09", ThemeFileTint::Base09}, {"base0A", ThemeFileTint::Base0A}, {"base0B", ThemeFileTint::Base0B},
    {"base0C", ThemeFileTint::Base0C}, {"base0D", ThemeFileTint::Base0D}, {"base0E", ThemeFileTint::Base0E},
    {"base0F", ThemeFileTint::Base0F}, {"base10", ThemeFileTint::Base10}, {"base11", ThemeFileTint::Base11},
    {"base12", ThemeFileTint::Base12}, {"base13", ThemeFileTint::Base13}, {"base14", ThemeFileTint::Base14},
    {"base15", ThemeFileTint::Base15}, {"base16", ThemeFileTint::Base16}, {"base17", ThemeFileTint::Base17},
};

static const std::unordered_map<ThemeFileTint, ThemeFileTint> b24tob16 = {
    {ThemeFileTint::Base10, ThemeFileTint::Base00}, {ThemeFileTint::Base11, ThemeFileTint::Base00},
    {ThemeFileTint::Base12, ThemeFileTint::Base08}, {ThemeFileTint::Base13, ThemeFileTint::Base0A},
    {ThemeFileTint::Base14, ThemeFileTint::Base0B}, {ThemeFileTint::Base15, ThemeFileTint::Base0C},
    {ThemeFileTint::Base16, ThemeFileTint::Base0D}, {ThemeFileTint::Base17, ThemeFileTint::Base0E},
};

// clang-format off
static const std::unordered_map<std::string, SemanticColor> keyToSemantic = {
    {"red", SemanticColor::Red},
    {"orange", SemanticColor::Orange},
    {"yellow", SemanticColor::Yellow},
    {"green", SemanticColor::Green},
    {"cyan", SemanticColor::Cyan},
    {"blue", SemanticColor::Blue},
    {"magenta", SemanticColor::Magenta},

	{"background", SemanticColor::Background},
	{"background_border", SemanticColor::BackgroundBorder},
	{"lighter_background", SemanticColor::LighterBackground},
	{"lighter_background_border", SemanticColor::LighterBackgroundBorder},
	{"selection_background", SemanticColor::SelectionBackground},
	{"hover_background", SemanticColor::HoverBackground},
	{"lighter_selection_background", SemanticColor::LighterSelectionBackground},
	{"lighter_hover_background", SemanticColor::LighterHoverBackground},

	{"foreground", SemanticColor::Foreground},
	{"dark_foreground", SemanticColor::DarkForeground},
	{"light_foreground", SemanticColor::LightForeground},
	{"lightest_foreground", SemanticColor::LightestForeground},
	{"accent_foreground", SemanticColor::AccentForeground},

	{"text_selection_background", SemanticColor::TextSelectionBackground},
	{"text_selection_foreground", SemanticColor::TextSelectionForeground},

	{"input_placeholder", SemanticColor::InputPlaceholder},
	{"input_border", SemanticColor::InputBorder},
	{"input_border_focus", SemanticColor::InputBorderFocus},
	{"input_border_error", SemanticColor::InputBorderError},

};
// clang-format on

namespace fs = std::filesystem;

static const std::array<ThemeFileTint, 16> base16Keys = {
    ThemeFileTint::Base00, ThemeFileTint::Base01, ThemeFileTint::Base02, ThemeFileTint::Base03,
    ThemeFileTint::Base04, ThemeFileTint::Base05, ThemeFileTint::Base06, ThemeFileTint::Base07,
    ThemeFileTint::Base08, ThemeFileTint::Base09, ThemeFileTint::Base0A, ThemeFileTint::Base0B,
    ThemeFileTint::Base0C, ThemeFileTint::Base0D, ThemeFileTint::Base0E, ThemeFileTint::Base0F};

static const std::array<ThemeFileTint, 8> base24Keys = {
    ThemeFileTint::Base10, ThemeFileTint::Base11, ThemeFileTint::Base12, ThemeFileTint::Base13,
    ThemeFileTint::Base14, ThemeFileTint::Base15, ThemeFileTint::Base16, ThemeFileTint::Base17};

QColor ThemeFile::parseColorName(const QString &colorName, const Tints &tints) {
  if (auto it = keyToTint.find(colorName.toStdString()); it != keyToTint.end()) {
    if (auto it2 = tints.find(it->second); it2 != tints.end()) { return it2->second; }
    return QColor();
  }

  return QColor(colorName);
}

QColor ThemeFile::parseColor(toml::node_view<toml::node> node, const Tints &tints) {
  if (auto str = node.as_string()) { return parseColorName(str->value_or(""), tints); }
  if (auto table = node.as_table()) {
    QColor color;

    if (auto name = (*table)["name"].as_string()) { color = parseColorName(name->value_or(""), tints); }
    if (auto opacity = (*table)["opacity"].as_floating_point()) { color.setAlphaF(opacity->value_or(1)); }
    if (auto lighter = (*table)["lighter"].as_floating_point()) {
      color = color.lighter(lighter->value_or(1) * 100);
    }
    if (auto darker = (*table)["darker"].as_floating_point()) {
      color = color.darker(darker->value_or(1) * 100);
    }

    return color;
  }

  return QColor();
}

QColor ThemeFile::deriveSemantic(SemanticColor color) {
  switch (color) {
  case SemanticColor::Red:
    return resolveTint(ThemeFileTint::Base08);
  case SemanticColor::Orange:
    return resolveTint(ThemeFileTint::Base09);
  case SemanticColor::Yellow:
    return resolveTint(ThemeFileTint::Base0A);
  case SemanticColor::Green:
    return resolveTint(ThemeFileTint::Base0B);
  case SemanticColor::Cyan:
    return resolveTint(ThemeFileTint::Base0C);
  case SemanticColor::Blue:
    return resolveTint(ThemeFileTint::Base0D);
  case SemanticColor::Magenta:
    return resolveTint(ThemeFileTint::Base0E);
  case SemanticColor::Purple:
    return resolveSemantic(SemanticColor::Magenta);

  case SemanticColor::Background:
    return resolveTint(ThemeFileTint::Base00);
  case SemanticColor::BackgroundBorder:
    return resolveTint(ThemeFileTint::Base01);
  case SemanticColor::LighterBackground:
    return resolveTint(ThemeFileTint::Base01);
  case SemanticColor::LighterBackgroundBorder:
    return resolveTint(ThemeFileTint::Base02);
  case SemanticColor::SelectionBackground:
    return resolveTint(ThemeFileTint::Base02);
  case SemanticColor::HoverBackground: {
    QColor color = resolveSemantic(SemanticColor::SelectionBackground);
    color.setAlphaF(0.8);
    return color;
  }
  case SemanticColor::LighterSelectionBackground:
    return QColor();
  case SemanticColor::LighterHoverBackground:
    return QColor();

  case SemanticColor::Foreground:
    return resolveTint(ThemeFileTint::Base05);
  case SemanticColor::DarkForeground:
    return resolveTint(ThemeFileTint::Base04);
  case SemanticColor::LightForeground:
    return resolveTint(ThemeFileTint::Base06);
  case SemanticColor::LightestForeground:
    return resolveTint(ThemeFileTint::Base07);
  case SemanticColor::AccentForeground:
    return QColor("#FFFFFF");

  case SemanticColor::TextSelectionBackground:
    return resolveSemantic(SemanticColor::Blue);
  case SemanticColor::TextSelectionForeground:
    return resolveSemantic(SemanticColor::AccentForeground);

  case SemanticColor::InputBorder:
    return resolveSemantic(SemanticColor::BackgroundBorder);
  case SemanticColor::InputBorderFocus:
    return resolveSemantic(SemanticColor::Blue);
  case SemanticColor::InputBorderError:
    return resolveSemantic(SemanticColor::Red);
  case SemanticColor::InputPlaceholder:
    return resolveSemantic(SemanticColor::LightestForeground);
  default:
    break;
  }

  return QColor();
}

tl::expected<ThemeFile, QString> ThemeFile::fromFile(const fs::path &path) {
  ThemeFile::InitData data;
  auto file = toml::parse_file(path.c_str());

  auto metaPtr = file["meta"].as_table();

  if (!metaPtr) { return tl::unexpected("a [meta] table is required"); }

  auto &meta = *metaPtr;

  data.name = QString::fromStdString(meta["name"].value<std::string>().value_or(""));
  data.description = QString::fromStdString(meta["description"].value<std::string>().value_or(""));

  if (auto ptr = meta["icon"].as_string()) {
    std::string icon = ptr->value_or("");
    fs::path iconPath = icon;
    std::error_code ec;

    if (icon.starts_with('.')) { iconPath = path / icon; }
    if (!fs::is_regular_file(icon, ec)) {
      return tl::unexpected(QString("%1 does not point to a valid file").arg(iconPath.c_str()));
    }

    data.icon = iconPath;
  }

  auto tintTable = file["tints"].as_table();

  if (!tintTable) { return tl::unexpected("a [tints] table is required"); }

  for (const auto &key : base16Keys) {
    if (auto it = tintToKey.find(key); it != tintToKey.end()) {
      if (!tintTable->contains(it->second)) {
        return tl::unexpected(QString("tints.%1 is required").arg(it->second));
      }
      auto value = (*tintTable)[it->second];
      QColor color = parseColor(value, data.tints);
      if (!color.isValid()) {
        return tl::unexpected(QString("tints.%1 is not a valid color").arg(it->second));
      }
      data.tints[key] = color;
    }
  }

  for (const auto &key : base24Keys) {
    if (auto it = tintToKey.find(key); it != tintToKey.end()) {
      // map b24 value to its lossy b16 equivalent, as specified in the spec
      if (!tintTable->contains(it->second)) {
        if (auto it2 = b24tob16.find(key); it2 != b24tob16.end()) {
          data.tints[key] = data.tints[it2->second];
        }
      }

      auto value = (*tintTable)[it->second];
      QColor color = parseColor(value, data.tints);
      if (!color.isValid()) {
        return tl::unexpected(QString("tints.%1 is not a valid color").arg(it->second));
      }
      data.tints[key] = color;
    }
  }

  if (auto table = file["semantic"].as_table()) {
    for (const auto &[k, v] : *table) {
      auto it = keyToSemantic.find(std::string(k.str()));
      if (it == keyToSemantic.end()) {
        return tl::unexpected(QString("%1 is not a valid semantic color").arg(std::string(k.str()).c_str()));
      }
      QColor color = parseColor(toml::node_view(v), data.tints);
      if (!color.isValid()) {
        return tl::unexpected(QString("tints.%1 is not a valid color").arg(it->second));
      }
      data.semantics[it->second] = color;
    }
  }

  return ThemeFile(data);
}
