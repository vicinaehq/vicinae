#include "theme-file.hpp"
#include "lib/toml.hpp"
#include "theme.hpp"
#include "theme/colors.hpp"
#include <filesystem>
#include <qfont.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qlogging.h>
#include <qobjectdefs.h>
#include <sstream>

namespace fs = std::filesystem;

static const std::unordered_map<ThemeTint, std::string> tintToKey = {
    {ThemeTint::Base00, "base00"}, {ThemeTint::Base01, "base01"}, {ThemeTint::Base02, "base02"},
    {ThemeTint::Base03, "base03"}, {ThemeTint::Base04, "base04"}, {ThemeTint::Base05, "base05"},
    {ThemeTint::Base06, "base06"}, {ThemeTint::Base07, "base07"}, {ThemeTint::Base08, "base08"},
    {ThemeTint::Base09, "base09"}, {ThemeTint::Base0A, "base0A"}, {ThemeTint::Base0B, "base0B"},
    {ThemeTint::Base0C, "base0C"}, {ThemeTint::Base0D, "base0D"}, {ThemeTint::Base0E, "base0E"},
    {ThemeTint::Base0F, "base0F"},

    {ThemeTint::Base10, "base10"}, {ThemeTint::Base11, "base11"}, {ThemeTint::Base12, "base12"},
    {ThemeTint::Base13, "base13"}, {ThemeTint::Base14, "base14"}, {ThemeTint::Base15, "base15"},
    {ThemeTint::Base16, "base16"}, {ThemeTint::Base17, "base17"},
};

static const std::unordered_map<std::string, ThemeTint> keyToTint = {
    {"base00", ThemeTint::Base00}, {"base01", ThemeTint::Base01}, {"base02", ThemeTint::Base02},
    {"base03", ThemeTint::Base03}, {"base04", ThemeTint::Base04}, {"base05", ThemeTint::Base05},
    {"base06", ThemeTint::Base06}, {"base07", ThemeTint::Base07}, {"base08", ThemeTint::Base08},
    {"base09", ThemeTint::Base09}, {"base0A", ThemeTint::Base0A}, {"base0B", ThemeTint::Base0B},
    {"base0C", ThemeTint::Base0C}, {"base0D", ThemeTint::Base0D}, {"base0E", ThemeTint::Base0E},
    {"base0F", ThemeTint::Base0F}, {"base10", ThemeTint::Base10}, {"base11", ThemeTint::Base11},
    {"base12", ThemeTint::Base12}, {"base13", ThemeTint::Base13}, {"base14", ThemeTint::Base14},
    {"base15", ThemeTint::Base15}, {"base16", ThemeTint::Base16}, {"base17", ThemeTint::Base17},
};

static const std::unordered_map<ThemeTint, ThemeTint> b24tob16 = {
    {ThemeTint::Base10, ThemeTint::Base00}, {ThemeTint::Base11, ThemeTint::Base00},
    {ThemeTint::Base12, ThemeTint::Base08}, {ThemeTint::Base13, ThemeTint::Base0A},
    {ThemeTint::Base14, ThemeTint::Base0B}, {ThemeTint::Base15, ThemeTint::Base0C},
    {ThemeTint::Base16, ThemeTint::Base0D}, {ThemeTint::Base17, ThemeTint::Base0E},
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

    {"purple", SemanticColor::Purple},

	{"main_window_border", SemanticColor::MainWindowBorder},
	{"settings_window_border", SemanticColor::SettingsWindowBorder},
	{"popover_border", SemanticColor::PopoverBorder},

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

	{"link_default", SemanticColor::LinkDefault},
	{"link_visited", SemanticColor::LinkVisited},

	{"accent", SemanticColor::Accent},
};

static const std::array<ThemeTint, 16> base16Keys = {
    ThemeTint::Base00, ThemeTint::Base01, ThemeTint::Base02, ThemeTint::Base03,
    ThemeTint::Base04, ThemeTint::Base05, ThemeTint::Base06, ThemeTint::Base07,
    ThemeTint::Base08, ThemeTint::Base09, ThemeTint::Base0A, ThemeTint::Base0B,
    ThemeTint::Base0C, ThemeTint::Base0D, ThemeTint::Base0E, ThemeTint::Base0F};

static const std::array<ThemeTint, 8> base24Keys = {
    ThemeTint::Base10, ThemeTint::Base11, ThemeTint::Base12, ThemeTint::Base13,
    ThemeTint::Base14, ThemeTint::Base15, ThemeTint::Base16, ThemeTint::Base17};

// clang-format on

const QString &ThemeFile::id() const { return m_data.id; }
const QString &ThemeFile::name() const { return m_data.name; }
ThemeVariant ThemeFile::variant() const { return m_data.variant; }
const QString &ThemeFile::description() const { return m_data.description; }
const ThemeFile::Icon &ThemeFile::icon() const { return m_data.icon; }
bool ThemeFile::isLight() const { return variant() == ThemeVariant::Light; }
bool ThemeFile::isDark() const { return variant() == ThemeVariant::Dark; }

QColor ThemeFile::resolve(SemanticColor color) const {
  if (auto it = m_data.semantics.find(color); it != m_data.semantics.end()) { return it->second; }
  return deriveSemantic(color);
}

QColor ThemeFile::resolve(ThemeTint tint) const {
  if (auto it = m_data.tints.find(tint); it != m_data.tints.end()) { return it->second; }
  return QColor();
}

static QColor parseColorName(const QString &colorName, const ThemeFile::Tints &tints) {
  if (auto it = keyToTint.find(colorName.toStdString()); it != keyToTint.end()) {
    if (auto it2 = tints.find(it->second); it2 != tints.end()) { return it2->second; }
    return QColor();
  }

  return QColor(colorName);
}

QJsonDocument ThemeFile::toJson() const {
  QJsonDocument doc;
  QJsonObject obj;

  obj["version"] = 1;
  obj["name"] = m_data.name;
  obj["description"] = m_data.description;
  obj["variant"] = serializeVariant(m_data.variant).c_str();

  if (m_data.icon) { obj["icon"] = m_data.icon->c_str(); }

  {
    QJsonObject tints;
    for (uint8_t i = 0; i != 15; ++i) {
      auto tint = static_cast<ThemeTint>(i);
      if (auto key = keyFromTint(tint)) { tints[key->c_str()] = resolve(tint).name(); }
    }
    obj["tints"] = tints;
  }

  {
    QJsonObject semantics;
    for (uint8_t i = 0; i != SemanticColor::InvalidTint; ++i) {
      auto semantic = static_cast<SemanticColor>(i);
      if (auto key = keyFromSemantic(semantic)) { semantics[key->c_str()] = resolve(semantic).name(); }
    }
    obj["semantic"] = semantics;
  }

  doc.setObject(obj);
  return doc;
}

std::string ThemeFile::toToml() const {
  toml::table toml;

  {
    toml::table meta{
        {"version", "1"},
        {"name", m_data.name.toStdString()},
        {"description", m_data.description.toStdString()},
        {"variant", serializeVariant(m_data.variant)},
    };

    if (m_data.icon) { meta.insert("icon", m_data.icon->c_str()); }
    toml.insert("meta", meta);
  }

  {
    toml::table tints;

    for (uint8_t i = 0; i != 15; ++i) {
      auto tint = static_cast<ThemeTint>(i);
      if (auto key = keyFromTint(tint)) { tints.insert(*key, resolve(tint).name().toStdString()); }
    }

    toml.insert("tints", tints);
  }

  {
    toml::table semantics;

    for (uint8_t i = 0; i != SemanticColor::InvalidTint; ++i) {
      auto semantic = static_cast<SemanticColor>(i);
      if (auto key = keyFromSemantic(semantic)) {
        semantics.insert(*key, resolve(semantic).name().toStdString());
      }
    }
    toml.insert("semantic", semantics);
  }

  std::ostringstream oss;
  oss << toml;
  return oss.str();
}

ThemeFile ThemeFile::vicinaeDark() {
  InitData data;
  data.id = "vicinae-dark";
  data.name = "Vicinae Dark";
  data.description = "Default Vicinae dark palette";

  data.tints = {
      {ThemeTint::Base00, QColor("#1A1A1A")}, {ThemeTint::Base01, QColor("#242424")},
      {ThemeTint::Base02, QColor("#2E2E2E")}, {ThemeTint::Base03, QColor("#505050")},
      {ThemeTint::Base04, QColor("#B0B0B0")}, {ThemeTint::Base05, QColor("#E8E6E1")},
      {ThemeTint::Base06, QColor("#F0EFEB")}, {ThemeTint::Base07, QColor("#FFFFFF")},

      {ThemeTint::Base08, QColor("#B9543B")}, {ThemeTint::Base09, QColor("#F0883E")},
      {ThemeTint::Base0A, QColor("#BFAE78")}, {ThemeTint::Base0B, QColor("#3A9C61")},
      {ThemeTint::Base0C, QColor("#18A5B3")}, {ThemeTint::Base0D, QColor("#2F6FED")},
      {ThemeTint::Base0E, QColor("#BC8CFF")}, {ThemeTint::Base0F, QColor("#7267B0")},
  };

  return ThemeFile(data);
}

ThemeFile ThemeFile::vicinaeLight() {
  InitData data;
  data.id = "vicinae-light";
  data.name = "Vicinae Light";
  data.description = "Default Vicinae light palette";

  data.tints = {
      {ThemeTint::Base00, QColor("#F4F2EE")}, {ThemeTint::Base01, QColor("#E6E4DF")},
      {ThemeTint::Base02, QColor("#D8D6D1")}, {ThemeTint::Base03, QColor("#A0A0A0")},
      {ThemeTint::Base04, QColor("#707070")}, {ThemeTint::Base05, QColor("#1A1A1A")},
      {ThemeTint::Base06, QColor("#000000")}, {ThemeTint::Base07, QColor("#FFFFFF")},

      {ThemeTint::Base08, QColor("#C25C49")}, {ThemeTint::Base09, QColor("#DA8A48")},
      {ThemeTint::Base0A, QColor("#BFAE78")}, {ThemeTint::Base0B, QColor("#3A9C61")},
      {ThemeTint::Base0C, QColor("#18A5B3")}, {ThemeTint::Base0D, QColor("#1F6FEB")},
      {ThemeTint::Base0E, QColor("#A48ED6")}, {ThemeTint::Base0F, QColor("#8374B7")},
  };

  return ThemeFile(data);
}

static QColor parseColor(toml::node_view<toml::node> node, const ThemeFile::Tints &tints) {
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

QColor ThemeFile::deriveSemantic(SemanticColor color) const {
  switch (color) {
  case SemanticColor::Red:
    return resolve(ThemeTint::Base08);
  case SemanticColor::Orange:
    return resolve(ThemeTint::Base09);
  case SemanticColor::Yellow:
    return resolve(ThemeTint::Base0A);
  case SemanticColor::Green:
    return resolve(ThemeTint::Base0B);
  case SemanticColor::Cyan:
    return resolve(ThemeTint::Base0C);
  case SemanticColor::Blue:
    return resolve(ThemeTint::Base0D);
  case SemanticColor::Magenta:
    return resolve(ThemeTint::Base0E);
  case SemanticColor::Purple:
    return resolve(SemanticColor::Magenta);

  case SemanticColor::LinkDefault:
    return resolve(SemanticColor::Blue);
  case SemanticColor::LinkVisited:
    return resolve(SemanticColor::Purple);

  case SemanticColor::LoadingBar:
    return resolve(SemanticColor::TextSecondary);
  case SemanticColor::DynamicToastSpinner:
    return resolve(SemanticColor::Foreground);

  case SemanticColor::ScrollBarBackground:
    return resolve(SemanticColor::SelectionBackground);

  case SemanticColor::PopoverBackground:
    return resolve(SemanticColor::LighterBackground);
  case SemanticColor::PopoverBorder:
    return resolve(SemanticColor::BackgroundBorder);

  case SemanticColor::TooltipBackground:
    return resolve(SemanticColor::PopoverBackground);
  case SemanticColor::TooltipBorder:
    return resolve(SemanticColor::PopoverBorder);

  case SemanticColor::Background:
    return resolve(ThemeTint::Base00);
  case SemanticColor::BackgroundBorder:
    return resolve(ThemeTint::Base02);
  case SemanticColor::LighterBackground:
    return resolve(ThemeTint::Base01);
  case SemanticColor::LighterBackgroundBorder:
    return resolve(ThemeTint::Base02);
  case SemanticColor::SelectionBackground:
    return resolve(ThemeTint::Base02);
  case SemanticColor::HoverBackground: {
    return withAlphaF(resolve(SemanticColor::SelectionBackground), 0.7);
  }
  case SemanticColor::LighterSelectionBackground:
    return QColor();
  case SemanticColor::LighterHoverBackground:
    return QColor();

  case SemanticColor::Foreground:
    return resolve(ThemeTint::Base05);
  case SemanticColor::DarkForeground:
    return resolve(ThemeTint::Base04);
  case SemanticColor::LightForeground:
    return resolve(ThemeTint::Base06);
  case SemanticColor::LightestForeground:
    return resolve(ThemeTint::Base07);
  case SemanticColor::AccentForeground:
    return QColor("#FFFFFF");

  case SemanticColor::TextSelectionBackground:
    return resolve(SemanticColor::Accent);
  case SemanticColor::TextSelectionForeground:
    return resolve(SemanticColor::AccentForeground);

  case SemanticColor::InputBorder:
    return resolve(SemanticColor::BackgroundBorder);
  case SemanticColor::InputBorderFocus:
    return resolve(SemanticColor::Accent);
  case SemanticColor::InputBorderError:
    return resolve(SemanticColor::Red);
  case SemanticColor::InputPlaceholder:
    return withAlphaF(resolve(SemanticColor::Foreground), 0.6);

  case SemanticColor::TextPrimary:
    return resolve(SemanticColor::Foreground);
  case SemanticColor::TextSecondary:
    return withAlphaF(resolve(SemanticColor::TextPrimary), 0.7);
  case SemanticColor::TextError:
    return resolve(SemanticColor::Red);
  case SemanticColor::TextSuccess:
    return resolve(SemanticColor::TextSuccess);

  case SemanticColor::MainWindowBorder:
    return resolve(SemanticColor::BackgroundBorder);
  case SemanticColor::SettingsWindowBorder:
    return resolve(SemanticColor::BackgroundBorder);

  case SemanticColor::Accent:
    return resolve(SemanticColor::Blue);

  default:
    break;
  }

  return QColor();
}

QColor ThemeFile::withAlphaF(const QColor &color, float alpha) {
  QColor col = color;
  col.setAlphaF(alpha);
  return col;
}

std::optional<std::string> ThemeFile::keyFromSemantic(SemanticColor color) {
  // we generate reverse mapping automatically if we ever need this
  static std::unordered_map<SemanticColor, std::string> reverseMapping;

  if (reverseMapping.empty()) {
    reverseMapping.reserve(keyToSemantic.size());
    for (const auto &[k, v] : keyToSemantic) {
      reverseMapping[v] = k;
    }
  }

  if (auto it = reverseMapping.find(color); it != reverseMapping.end()) { return it->second; }

  return {};
}

std::optional<SemanticColor> ThemeFile::semanticFromKey(const std::string &key) {
  if (auto it = keyToSemantic.find(key); it != keyToSemantic.end()) { return it->second; }
  return {};
}

std::optional<ThemeTint> ThemeFile::tintFromKey(const std::string &key) {
  if (auto it = keyToTint.find(key); it != keyToTint.end()) { return it->second; }
  return {};
}

std::optional<std::string> ThemeFile::keyFromTint(ThemeTint tint) {
  if (auto it = tintToKey.find(tint); it != tintToKey.end()) { return it->second; }
  return {};
}

ThemeVariant ThemeFile::parseVariant(const std::string &variant) {
  if (variant == "light") return ThemeVariant::Light;
  return ThemeVariant::Dark;
}

std::string ThemeFile::serializeVariant(ThemeVariant variant) {
  switch (variant) {
  case ThemeVariant::Light:
    return "light";
  case ThemeVariant::Dark:
    return "dark";
  }
  return "dark";
}

tl::expected<ThemeFile, QString> ThemeFile::fromFile(const fs::path &path) {
  ThemeFile::InitData data;
  auto file = toml::parse_file(path.c_str());

  data.id = path.filename().c_str();

  auto metaPtr = file["meta"].as_table();

  if (!metaPtr) { return tl::unexpected("a [meta] table is required"); }

  auto &meta = *metaPtr;

  auto version = meta["version"].as_string();
  auto name = meta["name"].as_string();
  auto description = meta["description"].as_string();
  auto variant = meta["variant"].as_string();

  if (!name) return tl::unexpected("meta.name must be a string");
  if (!description) return tl::unexpected("meta.description must be a string");
  if (!variant) return tl::unexpected("meta.variant must be a string (\"light\" | \"dark\")");
  if (!version) return tl::unexpected("meta.version must should be set to \"1\"");

  data.name = QString::fromStdString(name->value_or(""));
  data.description = QString::fromStdString(description->value_or(""));
  data.variant = parseVariant(description->value_or(""));

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

  for (const auto &tint : base16Keys) {
    if (auto key = keyFromTint(tint)) {
      if (!tintTable->contains(*key)) {
        return tl::unexpected(QString("tints.%1 is required").arg(key->c_str()));
      }
      auto value = (*tintTable)[*key];
      QColor color = parseColor(value, data.tints);
      if (!color.isValid()) {
        return tl::unexpected(QString("tints.%1 is not a valid color").arg(key->c_str()));
      }
      data.tints[tint] = color;
    }
  }

  if (auto table = file["semantic"].as_table()) {
    for (const auto &[k, v] : *table) {
      std::string key(k.str());
      auto semantic = semanticFromKey(std::string(k.str()));

      if (!semantic) { return tl::unexpected(QString("%1 is not a valid semantic color").arg(key)); }

      QColor color = parseColor(toml::node_view(v), data.tints);

      if (!color.isValid()) { return tl::unexpected(QString("semantic.%1 is not a valid color").arg(key)); }

      data.semantics[*semantic] = color;
    }
  }

  return ThemeFile(data);
}
