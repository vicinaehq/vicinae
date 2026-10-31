#include <algorithm>
#include <filesystem>
#include <format>
#include <fstream>
#include <sstream>
#include <QGuiApplication>
#include <QStyleHints>
#include "theme-tools.hpp"
#include "THEME_TEMPLATE.hpp"
#include "config/config.hpp"
#include "theme/theme-db.hpp"
#include "theme/theme-file.hpp"
#include "theme/theme.hpp"
#include "vicinae.hpp"

namespace fs = std::filesystem;

namespace {

constexpr std::size_t MAX_ID_LENGTH = 64;

struct WriteThemeInput {
  std::string id;
  std::string content;
};

struct SetThemeInput {
  std::string id;
};

AI::AbstractTool::RawToolTask ready(AI::AbstractTool::RawToolResult result) {
  return {QtFuture::makeReadyValueFuture(std::move(result)), {}};
}

fs::path userThemeDir() { return Omnicast::dataDir() / "themes"; }

fs::path userThemePath(std::string_view id) { return userThemeDir() / std::format("{}.toml", id); }

bool isUserTheme(const ThemeFile &theme) {
  const auto path = theme.path();
  return path && path->parent_path().lexically_normal() == userThemeDir().lexically_normal();
}

bool validId(std::string_view id) {
  return !id.empty() && id.size() <= MAX_ID_LENGTH && id.front() != '-' &&
         std::ranges::all_of(
             id, [](char c) { return (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-'; });
}

const char *variantName(const ThemeFile &theme) { return theme.isLight() ? "light" : "dark"; }

const char *systemColorScheme() {
  return QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Light ? "light" : "dark";
}

} // namespace

std::string ReadThemesTool::description() const {
  return "List the installed Vicinae themes and the active one, and return the theme file format: a "
         "commented "
         "TOML template documenting every available key, along with the source of the user's own themes. "
         "Read it before creating a theme with write_theme.";
}

std::string ReadThemesTool::generateInputSchema() const {
  return R"({"type":"object","properties":{},"additionalProperties":false})";
}

AI::AbstractTool::RawToolTask ReadThemesTool::runRaw(std::string_view) {
  auto &service = ThemeService::instance();
  const auto &config = m_config.value().theme;
  auto themes = service.themes();
  std::ranges::sort(themes, {}, [](const auto &theme) { return theme->id(); });

  std::string out = std::format(
      "The system uses the {} color scheme. Active theme: {}. Configured themes: {} (light), {} (dark).\n\n"
      "Installed themes:\n",
      systemColorScheme(), service.theme().id().toStdString(), config.light.name, config.dark.name);

  for (const auto &theme : themes) {
    out += std::format("- {} \"{}\": {}", theme->id().toStdString(), theme->name().toStdString(),
                       variantName(*theme));
    if (!theme->inherits().isEmpty()) out += std::format(", inherits {}", theme->inherits().toStdString());
    if (isUserTheme(*theme)) out += ", user theme";
    out += '\n';
  }

  out += std::format("\nUser themes are stored in {} and can be changed with write_theme. Other themes are "
                     "read-only, but a user theme can inherit from any of them and override only some colors."
                     "\n\nTheme file template:\n\n{}\n",
                     userThemeDir().string(), THEME_TEMPLATE);

  for (const auto &theme : themes) {
    if (!isUserTheme(*theme)) continue;
    std::ifstream stream(*theme->path());
    std::stringstream content;
    content << stream.rdbuf();
    out += std::format("\nSource of user theme {}:\n\n{}\n", theme->id().toStdString(), content.str());
  }

  return ready(AI::ToolOutput{.content = std::move(out)});
}

std::string WriteThemeTool::description() const {
  return "Create or replace a user theme. Always call read_themes first. The content argument is the "
         "complete "
         "TOML theme file. Prefer inheriting from an existing theme with meta.inherits and only overriding "
         "the colors that change. The id becomes the file name and can only contain lowercase letters, "
         "digits "
         "and dashes. It cannot be the id of a theme that is not a user theme. The theme is not applied: "
         "call "
         "set_theme afterwards to use it.";
}

std::string WriteThemeTool::generateInputSchema() const {
  return R"({"type":"object","properties":{"id":{"type":"string","description":"Theme id, used as the file name."},"content":{"type":"string","description":"Complete TOML theme file."}},"required":["id","content"],"additionalProperties":false})";
}

bool WriteThemeTool::requiresApproval(std::string_view arguments) const {
  WriteThemeInput input;
  if (glz::read_json(input, arguments) || !validId(input.id)) return false;
  std::error_code ec;
  return fs::exists(userThemePath(input.id), ec);
}

std::optional<std::string> WriteThemeTool::invocationSummary(std::string_view arguments) const {
  WriteThemeInput input;
  if (glz::read_json(input, arguments)) return std::nullopt;
  return std::format("{}.toml\n\n{}", input.id, input.content);
}

AI::AbstractTool::RawToolTask WriteThemeTool::runRaw(std::string_view arguments) {
  WriteThemeInput input;
  if (const auto error = glz::read_json(input, arguments))
    return ready(std::unexpected(glz::format_error(error, arguments)));

  if (!validId(input.id))
    return ready(std::unexpected(
        std::format("Invalid theme id \"{}\": use lowercase letters, digits and dashes only.", input.id)));

  auto &service = ThemeService::instance();
  const auto id = QString::fromStdString(input.id);
  const auto path = userThemePath(input.id);

  if (const auto *existing = service.findTheme(id); existing && !isUserTheme(*existing))
    return ready(std::unexpected(
        std::format("\"{}\" is the id of a theme that is not a user theme. Choose another id.", input.id)));

  auto parsed = ThemeFile::fromString(input.content, path);
  if (!parsed) return ready(std::unexpected(std::format("Invalid theme: {}", parsed.error())));

  if (!parsed->diagnostics.empty()) {
    std::string error = "Invalid theme:";
    for (const auto &diagnostic : parsed->diagnostics)
      error += std::format("\n- {}", diagnostic);
    return ready(std::unexpected(std::move(error)));
  }

  if (const auto &inherits = parsed->file.inherits(); inherits != id && !service.findTheme(inherits))
    return ready(std::unexpected(std::format(
        "meta.inherits refers to \"{}\", which is not an installed theme.", inherits.toStdString())));

  std::error_code ec;
  fs::create_directories(userThemeDir(), ec);
  auto tmp = path;
  tmp += ".tmp";

  {
    std::ofstream stream(tmp);
    stream << input.content;
    if (!stream.flush()) {
      fs::remove(tmp, ec);
      return ready(std::unexpected(std::format("Could not write {}.", path.string())));
    }
  }

  fs::rename(tmp, path, ec);
  if (ec) {
    fs::remove(tmp, ec);
    return ready(std::unexpected(std::format("Could not write {}.", path.string())));
  }

  return ready(AI::ToolOutput{
      .content = std::format("Saved theme {} to {}. Call set_theme to use it.", input.id, path.string())});
}

std::string SetThemeTool::description() const {
  return "Use an installed theme for the color scheme the system currently uses (light or dark). The change "
         "applies immediately.";
}

std::string SetThemeTool::generateInputSchema() const {
  return R"({"type":"object","properties":{"id":{"type":"string","description":"Id of an installed theme."}},"required":["id"],"additionalProperties":false})";
}

std::optional<std::string> SetThemeTool::invocationSummary(std::string_view arguments) const {
  SetThemeInput input;
  if (glz::read_json(input, arguments)) return std::nullopt;
  return input.id;
}

AI::AbstractTool::RawToolTask SetThemeTool::runRaw(std::string_view arguments) {
  SetThemeInput input;
  if (const auto error = glz::read_json(input, arguments))
    return ready(std::unexpected(glz::format_error(error, arguments)));

  if (!ThemeService::instance().selectTheme(QString::fromStdString(input.id), m_config))
    return ready(std::unexpected(std::format("There is no installed theme with id \"{}\".", input.id)));

  return ready(AI::ToolOutput{.content = std::format("Theme {} is now used for the {} color scheme.",
                                                     input.id, systemColorScheme())});
}
