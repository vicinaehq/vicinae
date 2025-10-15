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

using ColorMap = std::unordered_map<std::string, SemanticColor>;

// clang-format off
static const std::unordered_map<std::string, SemanticColor> keyToSemantic = {
	{"main_window_border", SemanticColor::MainWindowBorder},
	{"settings_window_border", SemanticColor::SettingsWindowBorder},
	{"popover_border", SemanticColor::PopoverBorder},

	{"background", SemanticColor::Background},
	{"background_border", SemanticColor::BackgroundBorder},
	{"lighter_background", SemanticColor::SecondaryBackground},
	{"lighter_background_border", SemanticColor::SecondaryBackgroundBorder},
	{"selection_background", SemanticColor::SelectionBackground},
	{"hover_background", SemanticColor::HoverBackground},

	{"foreground", SemanticColor::Foreground},
	{"light_foreground", SemanticColor::TextMuted},
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

static const std::unordered_map<std::string, ColorMap> groups = {
	{"core", {
		{"accent", SemanticColor::Accent},
		{"accent_foreground", SemanticColor::AccentForeground},
		{"foreground", SemanticColor::Foreground},
	}},

	{"accents", {
		{"blue", SemanticColor::Blue},
		{"green", SemanticColor::Green},
		{"magenta", SemanticColor::Magenta},
		{"orange", SemanticColor::Orange},
		{"red", SemanticColor::Red},
		{"yellow", SemanticColor::Yellow},
		{"cyan", SemanticColor::Cyan},
		{"purple", SemanticColor::Purple},
	}},

	{"text", {
		{"default", SemanticColor::TextPrimary},
		{"muted", SemanticColor::TextMuted},
		{"danger", SemanticColor::TextDanger},
		{"success", SemanticColor::TextSuccess}
	}},

	{"backgrounds", {
		{"main", SemanticColor::Background},
		{"secondary", SemanticColor::SecondaryBackground},
		{"secondary_border", SemanticColor::SecondaryBackgroundBorder}
	}},

	{"borders", {
		{"default", SemanticColor::BackgroundBorder},
		{"main_window", SemanticColor::MainWindowBorder},
		{"settings_window", SemanticColor::SettingsWindowBorder},
		{"input", SemanticColor::InputBorder},
		{"input_focus", SemanticColor::InputBorderFocus},
		{"input_error", SemanticColor::InputBorderError},
	}},

	{"interactive", {
		{"selection_background", SemanticColor::SelectionBackground},
		{"selection_foreground", SemanticColor::SelectionForeground},
		{"hover_background", SemanticColor::HoverBackground},
		{"hover_foreground", SemanticColor::HoverForegroud},
		{"secondary_selection_background", SemanticColor::SecondarySelectionBackground},
		{"secondary_selection_foreground", SemanticColor::SecondarySelectionForeground},
	}},

	{"scrollbars", {
		{"default", SemanticColor::ScrollBarBackground},
		{"secondary", SemanticColor::SecondaryScrollBarBackground}
	}},

	{"links", {
		{"default", SemanticColor::LinkDefault},
		{"visited", SemanticColor::LinkVisited}
	}},

	{"text_selection", {
		{"background", SemanticColor::TextSelectionBackground},
		{"foreground", SemanticColor::TextSelectionForeground}
	}},

	{"tooltip", {
		{"background", SemanticColor::TooltipBackground},
	}},

	{"activity", {
		{"loading_bar", SemanticColor::LoadingBar},
		{"spinner", SemanticColor::DynamicToastSpinner},
	}}
};

// clang-format on

void ThemeFile::setParent(const std::shared_ptr<ThemeFile> &file) { m_parent = file; }

const QString &ThemeFile::id() const { return m_data.id; }
const QString &ThemeFile::name() const { return m_data.name; }
const QString &ThemeFile::inherits() const { return m_data.inherits; }
ThemeVariant ThemeFile::variant() const { return m_data.variant; }
const QString &ThemeFile::description() const { return m_data.description; }
const ThemeFile::Icon &ThemeFile::icon() const { return m_data.icon; }
bool ThemeFile::isLight() const { return variant() == ThemeVariant::Light; }
bool ThemeFile::isDark() const { return variant() == ThemeVariant::Dark; }

QColor ThemeFile::resolve(SemanticColor color) const {
  if (auto it = m_data.semantics.find(color); it != m_data.semantics.end()) { return it->second; }
  return deriveSemantic(color);
}

static QColor parseColorName(const QString &colorName) {
  QColor color(colorName);

  if (color.isValid()) return color;

  return QColor("#" + colorName);
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

  for (const auto &[group, colorMap] : groups) {
    toml::table tbl;

    for (const auto &[k, v] : colorMap) {
      QColor color = resolve(v);
      tbl.insert(k, color.name().toStdString());
    }

    toml.insert(std::string("colors.") + group, tbl);
  }

  std::ostringstream oss;
  oss << toml;
  return oss.str();
}

ThemeFile ThemeFile::vicinaeDark() {
  InitData data;
  data.id = "vicinae-dark";
  data.name = "Vicinae Dark";
  data.variant = ThemeVariant::Dark;
  data.description = "Default Vicinae dark palette";
  data.semantics = {
      {SemanticColor::Background, QColor("#1A1A1A")},
      {SemanticColor::SelectionBackground, QColor("#2E2E2E")},
      {SemanticColor::Foreground, QColor("#E8E6E1")},
      {SemanticColor::BackgroundBorder, QColor("#2E2E2E")},
      {SemanticColor::SecondaryBackground, QColor("#242424")},
      {SemanticColor::PopoverBackground, QColor("#242424")},

      {SemanticColor::Red, QColor("#B9543B")},
      {SemanticColor::Orange, QColor("#F0883E")},
      {SemanticColor::Yellow, QColor("#BFAE78")},
      {SemanticColor::Green, QColor("#3A9C61")},
      {SemanticColor::Cyan, QColor("#18A5B3")},
      {SemanticColor::Blue, QColor("#2F6FED")},
      {SemanticColor::Magenta, QColor("#BC8CFF")},
      {SemanticColor::Purple, QColor("#BC8CFF")},
  };

  return ThemeFile(data);
}

ThemeFile ThemeFile::vicinaeLight() {
  InitData data;
  data.id = "vicinae-light";
  data.variant = ThemeVariant::Light;
  data.name = "Vicinae Light";
  data.description = "Default Vicinae light palette";
  data.semantics = {
      {SemanticColor::Background, QColor("#F4F2EE")}, {SemanticColor::SelectionBackground, QColor("#D8D6D1")},
      {SemanticColor::Foreground, QColor("#1A1A1A")},

      {SemanticColor::Red, QColor("#C25C49")},        {SemanticColor::Orange, QColor("#DA8A48")},
      {SemanticColor::Yellow, QColor("#BFAE78")},     {SemanticColor::Green, QColor("#3A9C61")},
      {SemanticColor::Cyan, QColor("#18A5B3")},       {SemanticColor::Blue, QColor("#1F6FEB")},
      {SemanticColor::Magenta, QColor("#A48ED6")},    {SemanticColor::Purple, QColor("#A48ED6")},
  };

  return ThemeFile(data);
}

static QColor parseColor(toml::node_view<toml::node> node) {
  if (auto str = node.as_string()) { return parseColorName(str->value_or("")); }
  if (auto table = node.as_table()) {
    QColor color;

    if (auto name = (*table)["name"].as_string()) { color = parseColorName(name->value_or("")); }
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

QColor ThemeFile::inherit(SemanticColor color) const {
  return m_parent ? m_parent->resolve(color) : QColor();
}

QColor ThemeFile::deriveSemantic(SemanticColor color) const {
  switch (color) {
  case SemanticColor::LinkDefault:
    return resolve(SemanticColor::Blue);
  case SemanticColor::LinkVisited:
    return resolve(SemanticColor::Purple);

  case SemanticColor::LoadingBar:
    return resolve(SemanticColor::TextMuted);
  case SemanticColor::DynamicToastSpinner:
    return resolve(SemanticColor::Foreground);

  case SemanticColor::SelectionForeground:
    return resolve(SemanticColor::Foreground);

  case SemanticColor::SecondarySelectionForeground:
    return resolve(SemanticColor::SelectionForeground);
  case SemanticColor::SecondarySelectionBackground:
    return resolve(SemanticColor::SelectionBackground);

  case SemanticColor::ScrollBarBackground:
    return resolve(SemanticColor::SelectionBackground);

  case SemanticColor::PopoverBackground:
    return resolve(SemanticColor::SecondaryBackground);
  case SemanticColor::PopoverBorder:
    return resolve(SemanticColor::BackgroundBorder);

  case SemanticColor::TooltipBackground:
    return resolve(SemanticColor::PopoverBackground);
  case SemanticColor::TooltipBorder:
    return resolve(SemanticColor::PopoverBorder);

  case SemanticColor::SecondaryBackgroundBorder:
    return resolve(SemanticColor::BackgroundBorder).lighter();
  case SemanticColor::HoverBackground:
    return withAlphaF(resolve(SemanticColor::SelectionBackground), 0.7);

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
    return withAlphaF(resolve(SemanticColor::TextPrimary), 0.6);

  case SemanticColor::Accent:
    return resolve(SemanticColor::Blue);

  case SemanticColor::TextPrimary:
    return resolve(SemanticColor::Foreground);
  case SemanticColor::TextMuted:
    return withAlphaF(resolve(SemanticColor::TextPrimary), 0.7);
  case SemanticColor::TextDanger:
    return resolve(SemanticColor::Red);
  case SemanticColor::TextSuccess:
    return resolve(SemanticColor::Green);

  case SemanticColor::MainWindowBorder:
    return resolve(SemanticColor::BackgroundBorder);
  case SemanticColor::SettingsWindowBorder:
    return resolve(SemanticColor::BackgroundBorder);

  default:
    break;
  }

  return inherit(color);
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
  QString filename = path.filename().c_str();

  data.id = filename.slice(0, filename.lastIndexOf('.'));

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
  data.variant = parseVariant(variant->value_or(""));

  if (meta.contains("inherits")) {
    data.inherits = meta.get("inherits")->as_string()->value_or("");
  } else {
    data.inherits = data.variant == ThemeVariant::Dark ? "vicinae-dark" : "vicinae-light";
  }

  if (auto ptr = meta["icon"].as_string()) {
    std::string icon = ptr->value_or("");
    fs::path iconPath = icon;
    std::error_code ec;

    if (!icon.starts_with('/')) { iconPath = path.parent_path() / icon; }
    if (!fs::is_regular_file(iconPath, ec)) {
      return tl::unexpected(QString("%1 does not point to a valid file").arg(iconPath.c_str()));
    }

    data.icon = iconPath;
  }

  if (file.contains("colors")) {
    if (auto colors = file.get("colors")->as_table()) {
      for (const auto &[group, colorMap] : groups) {
        auto groupPath = std::string("colors.") + group;
        if (!colors->contains(group)) continue;
        auto table = colors->get(group)->as_table();
        if (!table) continue;

        for (const auto &[k, v] : *table) {
          std::string key(k.str());
          std::string keyPath = groupPath + "." + key;

          if (auto it = colorMap.find(std::string(k.str())); it != colorMap.end()) {
            QColor color = parseColor(toml::node_view(v));
            if (!color.isValid()) {
              return tl::unexpected(QString("%1 is not a valid color").arg(keyPath.c_str()));
            }
            data.semantics[it->second] = color;
          }
        }
      }
    }
  }

  return ThemeFile(data);
}
