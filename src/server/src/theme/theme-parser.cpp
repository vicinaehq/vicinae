#include <variant>
#include <set>
#include "theme-parser.hpp"
#include "lib/toml.hpp"
#include "theme/colors.hpp"
#include "theme/theme-file.hpp"
#include "common/types.hpp"

namespace fs = std::filesystem;

static const std::vector<std::pair<std::string, SemanticColor>> recognizedKeys = {
    {"core.accent", SemanticColor::Accent},
    {"core.accent_foreground", SemanticColor::AccentForeground},
    {"core.background", SemanticColor::Background},
    {"core.foreground", SemanticColor::Foreground},
    {"core.secondary_background", SemanticColor::SecondaryBackground},
    {"core.border", SemanticColor::BackgroundBorder},

    {"shortcut.border", SemanticColor::ShortcutIndicatorBorder},

    {"main_window.border", SemanticColor::MainWindowBorder},
    {"main_window.footer.background", SemanticColor::StatusBarBackground},
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
    {"text.placeholder", SemanticColor::TextPlaceholder},
    {"text.links.default", SemanticColor::LinkDefault},
    {"text.links.visited", SemanticColor::LinkVisited},

    {"text.selection.background", SemanticColor::TextSelectionBackground},
    {"text.selection.foreground", SemanticColor::TextSelectionForeground},

    {"input.border", SemanticColor::InputBorder},
    {"input.border_focus", SemanticColor::InputBorderFocus},
    {"input.border_error", SemanticColor::InputBorderError},

    {"button.primary.background", SemanticColor::ButtonPrimaryBackground},
    {"button.primary.foreground", SemanticColor::ButtonPrimaryForeground},
    {"button.primary.hover.background", SemanticColor::ButtonPrimaryHoverBackground},
    {"button.primary.hover.foreground", SemanticColor::ButtonPrimaryHoverForeground},
    {"button.primary.focus.outline", SemanticColor::ButtonPrimaryFocusOutline},

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

static ThemeVariant parseVariant(const std::string &variant) {
  if (variant == "light") return ThemeVariant::Light;
  return ThemeVariant::Dark;
}

static std::string serializeVariant(ThemeVariant variant) {
  switch (variant) {
  case ThemeVariant::Light:
    return "light";
  case ThemeVariant::Dark:
    return "dark";
  }
  return "dark";
}

static std::optional<std::string> keyFromSemantic(SemanticColor color) {
  // we generate reverse mapping automatically if we ever need this
  static std::unordered_map<SemanticColor, std::string> reverseMapping;

  if (reverseMapping.empty()) {
    reverseMapping.reserve(recognizedKeys.size());
    for (const auto &[k, v] : recognizedKeys) {
      reverseMapping[v] = k;
    }
  }

  if (auto it = reverseMapping.find(color); it != reverseMapping.end()) { return it->second; }

  return {};
}

static const std::unordered_map<std::string, SemanticColor> &keyMap() {
  static std::unordered_map<std::string, SemanticColor> map;

  if (map.empty()) { map = {recognizedKeys.begin(), recognizedKeys.end()}; }

  return map;
}

static std::optional<SemanticColor> semanticFromKey(const std::string &key) {
  if (auto it = keyMap().find(key); it != keyMap().end()) { return it->second; }
  return {};
}

static std::expected<ThemeFile::MappedColor, std::string> parseColorName(const QString &colorName) {
  if (colorName.startsWith("colors.")) {
    std::string name = colorName.sliced(7).toStdString();
    if (auto semantic = semanticFromKey(name)) { return ThemeFile::ColorRef{.color = *semantic}; }
  }

  QColor color(colorName);

  if (color.isValid()) return color;
  color = QColor("#" + colorName);
  if (color.isValid()) return color;

  return std::unexpected(colorName.toStdString() + " is not a valid color name or reference");
}

static std::expected<ThemeFile::MappedColor, std::string> parseColor(toml::node_view<toml::node> node) {
  if (auto str = node.as_string()) { return parseColorName(str->value_or("")); }

  if (auto table = node.as_table()) {
    auto name = (*table)["name"].value<std::string>();

    if (!name) { return std::unexpected("Color value expressed as a map should have a name key"); }

    auto color = parseColorName(name->c_str());

    if (!color) return color;

    auto opacity = (*table)["opacity"].value<double>();
    auto lighter = (*table)["lighter"].value<int>();
    auto darker = (*table)["darker"].value<int>();
    const auto visitor = overloads{[&](QColor &color) -> ThemeFile::MappedColor {
                                     if (opacity) color.setAlphaF(*opacity);
                                     if (lighter) color = color.lighter(*lighter);
                                     if (darker) color = color.darker(*darker);
                                     return color;
                                   },
                                   [&](ThemeFile::ColorRef &ref) -> ThemeFile::MappedColor {
                                     if (opacity) ref.opacity = opacity;
                                     if (lighter) ref.lighter = lighter;
                                     if (darker) ref.darker = darker;
                                     return ref;
                                   }};

    return std::visit(visitor, *color);
  }

  return std::unexpected("Not a valid color");
}

std::expected<ThemeFile, std::string> ThemeParser::parse(const fs::path &path) {
  try {
    ThemeFile::InitData data;
    auto file = toml::parse_file(path.c_str());
    QString filename = path.filename().c_str();

    data.id = filename.slice(0, filename.lastIndexOf('.'));

    auto metaPtr = file["meta"].as_table();

    if (!metaPtr) { return std::unexpected("a [meta] table is required"); }

    auto &meta = *metaPtr;

    auto name = meta["name"].as_string();
    auto description = meta["description"].as_string();
    auto variant = meta["variant"].as_string();

    if (!name) return std::unexpected("meta.name must be a string");
    if (!description) return std::unexpected("meta.description must be a string");
    if (!variant) return std::unexpected("meta.variant must be a string (\"light\" | \"dark\")");

    data.name = QString::fromStdString(name->value_or(""));
    data.description = QString::fromStdString(description->value_or(""));
    data.variant = parseVariant(variant->value_or(""));
    data.path = path;

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

      data.icon = iconPath;
    }

    using Traverser = std::function<void(toml::table & table, const std::string &root)>;
    std::vector<std::string> diagnostics;

    Traverser traverse = [&](toml::table &table, const std::string &root = "") {
      for (const auto &[k, v] : table) {
        if (k == "meta") continue;

        std::string path = root;
        std::string key(k.str());
        if (!path.empty()) path += '.';
        path += key;

        if (path.starts_with("colors.")) {
          std::string colorPath = path.substr(7);
          if (auto semantic = semanticFromKey(colorPath)) {
            auto color = parseColor(toml::node_view(v));
            if (!color) {
              m_diagnostics.emplace_back(path + " is not a valid color: " + color.error());
              continue;
            }
            data.semantics[*semantic] = *color;
            continue;
          }
        }

        if (auto table = v.as_table()) {
          traverse(*table, path);
          continue;
        }

        m_diagnostics.emplace_back("unused config key " + path);
      }
    };

    traverse(*file.as_table(), "");

    std::set<SemanticColor> refCallStack;
    std::function<bool(void)> fn;

    std::function<bool(const ThemeFile::ColorRef &)> checkRef = [&](const ThemeFile::ColorRef &ref) {
      if (refCallStack.contains(ref.color)) return false;
      refCallStack.insert(ref.color);
      if (auto it = data.semantics.find(ref.color); it != data.semantics.end()) {
        if (auto ref = std::get_if<ThemeFile::ColorRef>(&it->second)) { return checkRef(*ref); }
      }
      return true;
    };

    for (const auto &[k, v] : data.semantics) {
      if (auto ref = std::get_if<ThemeFile::ColorRef>(&v)) {
        refCallStack.clear();
        if (!checkRef(*ref)) {
          return std::unexpected("Detected circular binding for key " + keyFromSemantic(k).value_or("???"));
        }
      }
    }

    return ThemeFile(data);
  } catch (const std::exception &error) {
    return std::unexpected(std::string("Parsing error: ") + error.what());
  }
}

ThemeParser::DiagnosticList ThemeParser::diagnostics() const { return m_diagnostics; }

std::string ThemeSerializer::toToml(const ThemeFile &file) const {
  std::ostringstream doc;

  doc << "[meta]\n";
  doc << "name = " << std::quoted(file.name().toStdString()) << "\n";
  doc << "description = " << std::quoted(file.description().toStdString()) << "\n";
  doc << "variant = " << std::quoted(serializeVariant(file.variant())) << "\n";
  doc << "inherits = " << std::quoted(file.inherits().toStdString()) << "\n";

  if (file.icon()) { doc << "icon = " << std::quoted(file.icon()->c_str()) << "\n"; }

  std::map<std::string, std::vector<std::pair<std::string, SemanticColor>>> mapped;
  std::vector<std::string> tableNames;

  for (const auto &[k, v] : recognizedKeys) {
    auto pos = k.find_last_of('.');
    std::string tableName = k.substr(0, pos);
    std::string key = k.substr(pos + 1);
    if (!mapped.contains(tableName)) { tableNames.emplace_back(tableName); }
    mapped[tableName].emplace_back(std::pair<std::string, SemanticColor>({key, v}));
  }

  for (const auto &tableName : tableNames) {
    doc << "\n";
    doc << "[colors." << tableName << "]\n";
    for (const auto &[k, v] : mapped[tableName]) {
      std::string full = "colors." + tableName + "." + k;
      QColor color = file.resolve(v);
      QString colorName = color.name(color.alpha() == 0xFF ? QColor::NameFormat::HexRgb : QColor::HexArgb);
      doc << k << " = " << std::quoted(colorName.toStdString()) << "\n";
    }
  }

  return doc.str();
}
