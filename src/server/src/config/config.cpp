#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <ranges>
#include <QStyleHints>
#include <glaze/util/key_transformers.hpp>
#include <qlogging.h>
#include <qstylehints.h>
#include <string_view>
#include <QGuiApplication>
#include <utility>
#include "utils.hpp"
#include "config.hpp"

namespace fs = std::filesystem;

// glaze's snake_case / rename_key key transform (returns std::string) and its `modify` augment
// mechanism both silently emit wrong keys on MSVC, so the config would fall back to struct defaults.
// This was only ever fixed for Clang/GCC: https://github.com/stephenberry/glaze/issues/2219
// Plain glz::object `value` metas (which replace reflection with literal keys) DO work on MSVC, so we
// map the snake_case JSON keys explicitly. Structs whose fields are all single-word keep pure
// reflection and need no meta here.
template <> struct glz::meta<config::SystemThemeConfig> {
  static constexpr auto value =
      glz::object("name", &config::SystemThemeConfig::name, "icon_theme", &config::SystemThemeConfig::iconTheme);
};
template <> struct glz::meta<config::Partial<config::SystemThemeConfig>> {
  using T = config::Partial<config::SystemThemeConfig>;
  static constexpr auto value = glz::object("name", &T::name, "icon_theme", &T::iconTheme);
};

template <> struct glz::meta<config::LayerShellConfig> {
  using T = config::LayerShellConfig;
  static constexpr auto value = glz::object("keyboard_interactivity", &T::keyboardInteractivity, "layer",
                                            &T::layer, "enabled", &T::enabled);
};
template <> struct glz::meta<config::Partial<config::LayerShellConfig>> {
  using T = config::Partial<config::LayerShellConfig>;
  static constexpr auto value = glz::object("layer", &T::layer, "keyboard_interactivity",
                                            &T::keyboardInteractivity, "enabled", &T::enabled);
};

template <> struct glz::meta<config::WindowCSD> {
  using T = config::WindowCSD;
  static constexpr auto value = glz::object("enabled", &T::enabled, "rounding", &T::rounding, "border_width",
                                            &T::borderWidth, "shadow_size", &T::shadowSize);
};
template <> struct glz::meta<config::Partial<config::WindowCSD>> {
  using T = config::Partial<config::WindowCSD>;
  static constexpr auto value = glz::object("enabled", &T::enabled, "rounding", &T::rounding, "border_width",
                                            &T::borderWidth, "shadow_size", &T::shadowSize);
};

template <> struct glz::meta<config::WindowConfig> {
  using T = config::WindowConfig;
  static constexpr auto value = glz::object(
      "opacity", &T::opacity, "rounding", &T::rounding, "client_side_decorations", &T::clientSideDecorations,
      "size", &T::size, "screen", &T::screen, "blur", &T::blur, "compact_mode", &T::compactMode, "layer_shell",
      &T::layerShell, "material", &T::material);
};
template <> struct glz::meta<config::Partial<config::WindowConfig>> {
  using T = config::Partial<config::WindowConfig>;
  static constexpr auto value = glz::object(
      "rounding", &T::rounding, "opacity", &T::opacity, "client_side_decorations", &T::clientSideDecorations,
      "size", &T::size, "blur", &T::blur, "compact_mode", &T::compactMode, "layer_shell", &T::layerShell,
      "material", &T::material);
};

template <> struct glz::meta<config::TelemetryConfig> {
  static constexpr auto value = glz::object("system_info", &config::TelemetryConfig::systemInfo);
};
template <> struct glz::meta<config::Partial<config::TelemetryConfig>> {
  static constexpr auto value =
      glz::object("system_info", &config::Partial<config::TelemetryConfig>::systemInfo);
};

template <> struct glz::meta<config::ConfigValue> {
  using T = config::ConfigValue;
  static constexpr auto value = glz::object(
      "$schema", &T::schema, "imports", &T::imports, "search_files_in_root", &T::searchFilesInRoot,
      "close_on_focus_loss", &T::closeOnFocusLoss, "consider_preedit", &T::considerPreedit,
      "pop_to_root_on_close", &T::popToRootOnClose, "pop_on_backspace", &T::popOnBackspace,
      "activate_on_single_click", &T::activateOnSingleClick, "encrypt_sensitive_data", &T::encryptSensitiveData,
      "escape_key_behavior", &T::escapeKeyBehavior, "favicon_service", &T::faviconService, "keybinding",
      &T::keybinding, "pixmap_cache_mb", &T::pixmapCacheMb, "input_server", &T::inputServer, "global_shortcuts",
      &T::globalShortcuts, "font", &T::font, "theme", &T::theme, "telemetry", &T::telemetry, "launcher_window",
      &T::launcherWindow, "header", &T::header, "footer", &T::footer, "keybinds", &T::keybinds, "favorites",
      &T::favorites, "fallbacks", &T::fallbacks, "providers", &T::providers);
};
template <> struct glz::meta<config::Partial<config::ConfigValue>> {
  using T = config::Partial<config::ConfigValue>;
  static constexpr auto value = glz::object(
      "$schema", &T::schema, "imports", &T::imports, "close_on_focus_loss", &T::closeOnFocusLoss,
      "consider_preedit", &T::considerPreedit, "pop_to_root_on_close", &T::popToRootOnClose, "pop_on_backspace",
      &T::popOnBackspace, "activate_on_single_click", &T::activateOnSingleClick, "encrypt_sensitive_data",
      &T::encryptSensitiveData, "escape_key_behavior", &T::escapeKeyBehavior, "favicon_service",
      &T::faviconService, "keybinding", &T::keybinding, "pixmap_cache_mb", &T::pixmapCacheMb,
      "search_files_in_root", &T::searchFilesInRoot, "input_server", &T::inputServer, "global_shortcuts",
      &T::globalShortcuts, "font", &T::font, "theme", &T::theme, "telemetry", &T::telemetry, "launcher_window",
      &T::launcherWindow, "header", &T::header, "footer", &T::footer, "keybinds", &T::keybinds, "favorites",
      &T::favorites, "fallbacks", &T::fallbacks, "providers", &T::providers);
};

namespace config {
static constexpr const char *TOP_COMMENT =
    R"(// This configuration is merged with the default vicinae configuration file, which you can obtain by running the `vicinae config default` command.
// Every item defined in this file takes precedence over the values defined in the default config or any other imported file.
//
// You can make manual edits to this file, however you should keep in mind that this file may be written to by vicinae when a configuration change is made through the GUI.
// When that happens, any custom comments or formatting will be lost.
//
// If you want to maintain a configuration file with your own comments and formatting, you should create a separate file and add it to the 'imports' array.
//
// Learn more about configuration at https://docs.vicinae.com/config)";

template <typename T> T static merge(const auto &v1, const auto &v2) {
  std::string buf;
  if (auto error = glz::write_json(glz::merge{v1, v2}, buf)) {
    std::cerr << "Failed to merge " << glz::format_error(error);
    // todo: do smth about that
  }
  T cfg;
  if (auto error = glz::read_json(cfg, buf)) {
    qWarning() << "Failed to read merged " << glz::format_error(error);
  }
  return cfg;
}

const SystemThemeConfig &ConfigValue::systemTheme() const {
  switch (QGuiApplication::styleHints()->colorScheme()) {
  case Qt::ColorScheme::Light:
    return theme.light;
  default:
    return theme.dark;
  }
}

Manager::Manager(fs::path path) : m_userPath(std::move(path)) {
  auto file = QFile(":config.jsonc");

  if (!file.open(QIODevice::ReadOnly)) { throw std::runtime_error("Failed to open default config"); }

  m_defaultData = file.readAll().toStdString();

  if (auto error =
          glz::read<glz::opts{.comments = true, .error_on_unknown_keys = false}>(m_defaultConfig, m_defaultData)) {
    throw std::runtime_error(
        std::format("Failed to parse default config file: {}", glz::format_error(error, m_defaultData)));
  }

  m_fsDebounce.setSingleShot(true);
  m_fsDebounce.setInterval(100);

  if (const char *envOverrides = std::getenv("VICINAE_OVERRIDES")) {
    m_envOverrides =
        std::string_view{envOverrides} | std::views::split(':') |
        std::views::transform([](const auto &part) { return std::string{part.begin(), part.end()}; }) |
        std::ranges::to<std::vector<std::string>>();

    qInfo() << "Loaded" << m_envOverrides.size() << "path(s) from VICINAE_OVERRIDES";

    for (const auto &override : m_envOverrides) {
      qInfo() << override;
    }
  }

  initConfig();
  reloadConfig();

  connect(&m_watcher, &QFileSystemWatcher::fileChanged, this, [this](const QString &path) {
    m_fsDebounce.start();
    m_watcher.addPath(QString::fromStdString(m_userPath.string()));
  });
  connect(&m_fsDebounce, &QTimer::timeout, this, [this]() { reloadConfig(); });
}

ConfigValue Manager::defaultConfig() const { return m_defaultConfig; }
const char *Manager::defaultConfigData() const { return m_defaultData.c_str(); }

bool Manager::mergeProviderWithUser(std::string_view id, const Partial<ProviderData> &data) {
  return mergeWithUser({.providers = std::map<std::string, Partial<ProviderData>>{{std::string{id}, data}}});
}

bool Manager::updateUser(const std::function<void(Partial<ConfigValue> &value)> &updater) {
  std::string buf;
  Partial<ConfigValue> user;

  if (auto error =
          glz::read_file_jsonc<glz::opts{.error_on_unknown_keys = false}>(user, m_userPath.string(), buf)) {
    qWarning() << "Failed to read user config as partial";
    return false;
  }

  updater(user);
  return writeUser(user);
}

bool Manager::mergeEntrypointWithUser(const EntrypointId &id, const ProviderItemData &data) {
  std::map<std::string, Partial<ProviderData>> providers;
  providers[id.provider] =
      Partial<ProviderData>{.entrypoints = std::map<std::string, ProviderItemData>{{id.entrypoint, data}}};
  return mergeWithUser({.providers = providers});
}

bool Manager::mergeThemeConfig(const config::Partial<config::SystemThemeConfig> &cfg) {
  switch (QGuiApplication::styleHints()->colorScheme()) {
  case Qt::ColorScheme::Light:
    return mergeWithUser({.theme = config::Partial<config::ThemeConfig>{.light = cfg}});
  default:
    return mergeWithUser({.theme = config::Partial<config::ThemeConfig>{.dark = cfg}});
  }
}

bool Manager::mergeWithUser(const Partial<ConfigValue> &patch) {
  std::string buf;
  Partial<ConfigValue> user;

  if (auto error =
          glz::read_file_jsonc<glz::opts{.error_on_unknown_keys = false}>(user, m_userPath.string(), buf)) {
    qWarning() << "Failed to read user config as partial, config changes haven't been applied.";
    return false;
  }

  if (auto error = glz::write_json(glz::merge{user, patch}, buf)) {
    qWarning() << "Failed to merge partials: config changes haven't been applied";
    return false;
  }

  if (auto error = glz::read_json(user, buf)) {
    qWarning() << "Failed to read merged partials: config changes haven't been applied";
    return false;
  }

  prunePartial(user);

  return writeUser(user);
}

bool Manager::writeUser(const Partial<ConfigValue> &cfg) {
  std::string buf;

  if (auto error = glz::write_json(cfg, buf)) {
    qWarning() << "Failed to write json" << glz::format_error(error);
    return false;
  }

  {
    std::ofstream ofs(m_userPath);
    ofs << TOP_COMMENT << "\n\n" << glz::prettify_json(buf);
  }

  reloadConfig();

  return true;
}

Manager::ConfigResult Manager::loadUser(const LoadingOptions &opts) {
  return load(m_userPath, opts).transform([&](auto &&res) {
    return merge<ConfigValue>(defaultConfig(), res);
  });
}

void Manager::reloadConfig() {
  std::error_code ec;
  if (!std::filesystem::is_regular_file(m_userPath, ec)) return;

  std::unordered_set<std::filesystem::path> visited;
  auto res = loadUser({.resolveImports = true, .visited = visited});

  if (!res) {
    emit configLoadingError(res.error());
    return;
  }

  ConfigValue const prev = std::move(m_user);
  m_user = std::move(res.value());

  std::string prevJson;
  std::string nextJson;
  bool const comparable = !glz::write_json(prev, prevJson) && !glz::write_json(m_user, nextJson);
  if (comparable && prevJson == nextJson) return;

  emit configChanged(m_user, prev);
}

void Manager::initConfig() {
  std::error_code ec;

  if (!fs::is_regular_file(m_userPath, ec)) {
    fs::create_directories(m_userPath.parent_path());
    writeUser({});
  }
}

std::filesystem::path Manager::resolvePath(const std::filesystem::path &path,
                                           const std::filesystem::path &cwd) {
  std::string importPath = expandPath(path).string();

  if (!importPath.starts_with('/')) { importPath = (cwd.parent_path() / importPath).string(); }

  return std::filesystem::weakly_canonical(importPath);
}

Manager::PartialConfigResult Manager::load(const std::filesystem::path &path, const LoadingOptions &opts) {
  m_watcher.addPath(QString::fromStdString(path.string()));

  std::string buf;
  Partial<ConfigValue> cfg;
  auto glzError = glz::read_file_jsonc<glz::opts{.error_on_unknown_keys = false}>(cfg, path.string(), buf);

  if (glzError) {
    std::string glzErrMsg = glz::format_error(glzError);
    return std::unexpected(std::format("Failed to read JSONC file at {}: {}", path.string(), glzErrMsg));
  }

  auto importFile = [this, &opts](Partial<ConfigValue> &cfg, const std::string &importPath,
                                  bool override) -> std::expected<Partial<ConfigValue>, std::string> {
    if (opts.visited.contains(importPath)) {
      qWarning().nospace() << "Circular import detected for " << importPath << ", ignoring...";
      return cfg;
    }

    opts.visited.insert(importPath);

    if (std::filesystem::exists(importPath)) {
      PartialConfigResult imported = load(importPath, opts);

      if (!imported) {
        return std::unexpected(std::format("Failed to import file \"{}\": {}", importPath, imported.error()));
      }

      if (override) return merge<Partial<ConfigValue>>(cfg, imported);
      return merge<Partial<ConfigValue>>(imported, cfg);
    } else {
      qWarning().nospace() << "Imported config file not found: " << importPath;
      return cfg;
    }
  };

  if (opts.resolveImports) {
    for (const auto &imp : cfg.imports.value_or(std::vector<std::string>{})) {
      auto result = importFile(cfg, resolvePath(imp, path).string(), false);
      if (!result) return result;
      cfg = std::move(result).value();
    }

    for (const auto &overridePath : m_envOverrides) {
      auto result = importFile(cfg, resolvePath(overridePath, path).string(), true);
      if (!result) return result;
      cfg = std::move(result).value();
    }
  }

  return cfg;
}

void Manager::prunePartial(Partial<ConfigValue> &user) {
  auto prunePreferences = [](glz::generic::object_t &obj) {
    for (auto it = obj.begin(); it != obj.end();) {
      if (it->second.is_null()) {
        it = obj.erase(it);
      } else {
        ++it;
      }
    }
  };

  if (user.providers) {
    auto &pvd = user.providers.value();

    for (auto it = pvd.begin(); it != pvd.end();) {
      auto &v = it->second;
      auto currentIt = it++;

      if (v.preferences) {
        prunePreferences(v.preferences.value());
        if (v.preferences.value().empty()) { v.preferences.reset(); }
      }

      if (v.entrypoints) {
        auto &entrypoints = *v.entrypoints;
        for (auto it2 = entrypoints.begin(); it2 != entrypoints.end();) {
          auto currentIt = it2++;
          ProviderItemData &vi = currentIt->second;

          if (vi.preferences) {
            prunePreferences(vi.preferences.value());
            if (vi.preferences->empty()) { vi.preferences.reset(); }
          }

          if (vi.alias && vi.alias->empty()) { vi.alias.reset(); }
          if (vi.shortcut && vi.shortcut->empty()) { vi.shortcut.reset(); }
          if (!vi.enabled.has_value() && vi.preferences.value_or(glz::generic::object_t{}).empty() &&
              !vi.alias && !vi.shortcut) {
            entrypoints.erase(currentIt);
          }
        }

        if (entrypoints.empty()) { v.entrypoints.reset(); }
      }

      if (!v.enabled && v.preferences.value_or(glz::generic::object_t{}).empty() && !v.entrypoints) {
        pvd.erase(currentIt);
      }
    }

    if (pvd.empty()) { user.providers.reset(); }
  }
}

}; // namespace config
