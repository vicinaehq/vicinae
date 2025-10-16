#include "theme-file.hpp"
#include "common.hpp"
#include "expected.hpp"
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

static const std::unordered_map<std::string, SemanticColor> keyToSemantic = {
    {"core.accent", SemanticColor::Accent},
    {"core.accent_foreground", SemanticColor::AccentForeground},
    {"core.background", SemanticColor::Background},
    {"core.foreground", SemanticColor::Foreground},
    {"core.secondary_background", SemanticColor::SecondaryBackground},
    {"core.border", SemanticColor::BackgroundBorder},

    {"main_window.border", SemanticColor::MainWindowBorder},
    {"settings_window.border", SemanticColor::SettingsWindowBorder},

    {"accents.blue", SemanticColor::Blue},
    {"accents.green", SemanticColor::Green},
    {"accents.magenta", SemanticColor::Magenta},
    {"accents.orange", SemanticColor::Orange},
    {"accents.red", SemanticColor::Red},
    {"accents.yellow", SemanticColor::Yellow},
    {"accents.cyan", SemanticColor::Cyan},
    {"accents.purple", SemanticColor::Purple},

    {"text.default", SemanticColor::TextPrimary},
    {"text.muted", SemanticColor::TextMuted},
    {"text.danger", SemanticColor::TextDanger},
    {"text.success", SemanticColor::TextSuccess},
    {"text.links.default", SemanticColor::LinkDefault},
    {"text.links.visited", SemanticColor::LinkVisited},

    {"text.selection.background", SemanticColor::TextSelectionBackground},
    {"text.selection.foreground", SemanticColor::TextSelectionForeground},

    {"input.border", SemanticColor::InputBorder},
    {"input.border_focus", SemanticColor::InputBorderFocus},
    {"input.border_error", SemanticColor::InputBorderError},
    {"input.placeholder", SemanticColor::InputPlaceholder},

    {"list.item.hover.background", SemanticColor::ListItemHoverBackground},
    {"list.item.hover.foreground", SemanticColor::ListItemHoverForegroud},
    {"list.item.selection.background", SemanticColor::ListItemSelectionBackground},
    {"list.item.selection.foreground", SemanticColor::ListItemSelectionForeground},
    {"list.item.selection.secondary_background", SemanticColor::ListItemSecondarySelectionBackground},
    {"list.item.selection.secondary_foreground", SemanticColor::ListItemSecondarySelectionForeground},

    {"grid.item.selection.outline", SemanticColor::GridItemSelectionOutline},
    {"grid.item.hover.outline", SemanticColor::GridItemHoverOutline},
    {"grid.item.background", SemanticColor::GridItemBackground},

    {"scrollbars.background", SemanticColor::ScrollBarBackground},
    {"scrollbars.secondary_background", SemanticColor::SecondaryScrollBarBackground},

    {"tooltip.background", SemanticColor::TooltipBackground},
    {"tooltip.foreground", SemanticColor::TooltipForeground},
    {"tooltip.border", SemanticColor::TooltipBorder},

    {"loading.bar", SemanticColor::LoadingBar},
    {"loading.spinner", SemanticColor::DynamicToastSpinner},
};

static const std::vector<std::string> orderedGroupKeys = {
    "core",      "backgrounds", "accents", "text",           "input",   "borders",
    "selection", "scrollbars",  "links",   "text_selection", "tooltip", "activity"};

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
  if (auto it = m_data.semantics.find(color); it != m_data.semantics.end()) {
    auto visitor = overloads{[](const QColor &color) { return color; },
                             [&](const ColorRef &ref) {
                               if (ref.color == color) return QColor();
                               auto color = resolve(ref.color);
                               color.setAlphaF(ref.opacity);
                               if (ref.darker) color = color.darker(*ref.darker);
                               if (ref.lighter) color = color.lighter(*ref.lighter);
                               return color;
                             }};

    return std::visit(visitor, it->second);
  }
  return deriveSemantic(color);
}

static tl::expected<ThemeFile::MappedColor, std::string> parseColorName(const QString &colorName) {
  if (colorName.startsWith("colors.")) {
    std::string name = colorName.sliced(7).toStdString();
    if (auto it = keyToSemantic.find(name); it != keyToSemantic.end()) {
      return ThemeFile::ColorRef{.color = it->second};
    }
  }

  QColor color(colorName);

  if (color.isValid()) return color;
  color = QColor("#" + colorName);
  if (color.isValid()) return color;

  return tl::unexpected("Invalid color name");
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
  std::ostringstream doc;

  doc << "[meta]\n";
  doc << "version = 1\n";
  doc << "name = " << std::quoted(m_data.name.toStdString()) << "\n";
  doc << "description = " << std::quoted(m_data.description.toStdString()) << "\n";
  doc << "variant = " << std::quoted(serializeVariant(m_data.variant)) << "\n";

  if (m_data.icon) { doc << "icon = " << std::quoted(m_data.icon->c_str()) << "\n"; }

  /*
  for (const auto &group : orderedGroupKeys) {
    if (auto it = groups.find(group); it != groups.end()) {
      auto &colorMap = it->second;
      doc << "\n";
      doc << "[colors." << group << "]\n";
      for (const auto &[k, v] : colorMap) {
        QColor color = resolve(v);
        doc << k << " = " << std::quoted(color.name().toStdString()) << "\n";
      }
    }
  }
  */

  return doc.str();
}

ThemeFile ThemeFile::vicinaeDark() {
  InitData data;
  data.id = "vicinae-dark";
  data.name = "Vicinae Dark";
  data.variant = ThemeVariant::Dark;
  data.description = "Default Vicinae dark palette";
  data.semantics = {
      {SemanticColor::Background, QColor("#1A1A1A")},
      {SemanticColor::ListItemSelectionBackground, QColor("#2E2E2E")},
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
      {SemanticColor::Background, QColor("#F4F2EE")},
      {SemanticColor::ListItemSelectionBackground, QColor("#D8D6D1")},
      {SemanticColor::Foreground, QColor("#1A1A1A")},

      {SemanticColor::Red, QColor("#C25C49")},
      {SemanticColor::Orange, QColor("#DA8A48")},
      {SemanticColor::Yellow, QColor("#BFAE78")},
      {SemanticColor::Green, QColor("#3A9C61")},
      {SemanticColor::Cyan, QColor("#18A5B3")},
      {SemanticColor::Blue, QColor("#1F6FEB")},
      {SemanticColor::Magenta, QColor("#A48ED6")},
      {SemanticColor::Purple, QColor("#A48ED6")},
  };

  return ThemeFile(data);
}

static tl::expected<ThemeFile::MappedColor, std::string> parseColor(toml::node_view<toml::node> node) {
  if (auto str = node.as_string()) { return parseColorName(str->value_or("")); }

  if (auto table = node.as_table()) {
    auto name = (*table)["name"].value<std::string>();

    if (!name) { return tl::unexpected("Color value expressed as a map should have a name key"); }

    auto color = parseColorName(name->c_str());

    if (!color) return color;

    float opacity = (*table)["opacity"].value_or(1.0f);
    auto lighter = (*table)["lighter"].value<int>();
    std::optional<int> darker = (*table)["darker"].value<int>();
    const auto visitor = overloads{[&](QColor &color) -> ThemeFile::MappedColor {
                                     color.setAlphaF(opacity);
                                     if (lighter) color = color.lighter(*lighter);
                                     if (darker) color = color.darker(*darker);
                                     return color;
                                   },
                                   [&](ThemeFile::ColorRef &ref) -> ThemeFile::MappedColor {
                                     ref.opacity = opacity;
                                     if (lighter) ref.lighter = lighter;
                                     if (darker) ref.darker = darker;
                                     return ref;
                                   }};

    return std::visit(visitor, *color);
  }

  return tl::unexpected("Not a valid color");
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

  case SemanticColor::ListItemSelectionForeground:
    return resolve(SemanticColor::Foreground);

  case SemanticColor::ListItemSecondarySelectionForeground:
    return resolve(SemanticColor::ListItemSelectionForeground);
  case SemanticColor::ListItemSecondarySelectionBackground:
    return resolve(SemanticColor::ListItemSelectionBackground);
  case SemanticColor::ListItemSecondaryHoverBackground:
    return withAlphaF(resolve(SemanticColor::ListItemSecondarySelectionBackground), 0.7);

  case SemanticColor::ScrollBarBackground:
    return resolve(SemanticColor::ListItemSelectionBackground);

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
  case SemanticColor::ListItemHoverBackground:
    return withAlphaF(resolve(SemanticColor::ListItemSelectionBackground), 0.7);
  case SemanticColor::ListItemHoverForegroud:
    return resolve(SemanticColor::Foreground);

  case SemanticColor::GridItemBackground:
    return resolve(SemanticColor::SecondaryBackground);
  case SemanticColor::GridItemSelectionOutline:
    return resolve(SemanticColor::Foreground);
  case SemanticColor::GridItemHoverOutline:
    return withAlphaF(resolve(SemanticColor::GridItemSelectionOutline));

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

  using Traverser = std::function<void(toml::table & table, const std::string &root)>;
  std::vector<std::string> diagnostics;

  Traverser traverse = [&](toml::table &table, const std::string &root = "") {
    for (const auto &[k, v] : table) {
      std::string path = root;
      std::string key(k.str());
      if (!path.empty()) path += '.';
      path += key;

      if (path.starts_with("colors.")) {
        std::string colorPath = path.substr(7);
        if (auto it = keyToSemantic.find(colorPath); it != keyToSemantic.end()) {
          auto color = parseColor(toml::node_view(v));
          if (!color) {
            qWarning() << path << "is not a valid color:" << color.error();
            continue;
          }
          data.semantics[it->second] = *color;
          continue;
        }
      }

      if (auto table = v.as_table()) {
        traverse(*table, path);
        continue;
      }

      qWarning() << "unused config key" << path;
    }
  };

  traverse(*file.as_table(), "");

  return ThemeFile(data);
}
