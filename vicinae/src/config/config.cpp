#include <filesystem>
#include <format>
#include <fstream>
#include <QStyleHints>
#include <glaze/util/key_transformers.hpp>
#include <qstylehints.h>
#include <string_view>
#include <QApplication>
#include "utils.hpp"
#include "config.hpp"

namespace fs = std::filesystem;

#define SNAKE_CASIFY(T)                                                                                      \
  template <> struct glz::meta<T> : glz::snake_case {};                                                      \
  template <> struct glz::meta<config::Partial<T>> : glz::snake_case {};

SNAKE_CASIFY(config::LayerShellConfig);
SNAKE_CASIFY(config::WindowConfig);
SNAKE_CASIFY(config::SystemThemeConfig);
SNAKE_CASIFY(config::ThemeConfig);
SNAKE_CASIFY(config::WindowCSD);

struct ConfigTransformer : glz::snake_case {
  static constexpr std::string rename_key(const std::string_view key) {
    if (key == "schema") return "$schema";
    return glz::to_snake_case(key);
  }
};

template <> struct glz::meta<config::ConfigValue> : ConfigTransformer {};
template <> struct glz::meta<config::Partial<config::ConfigValue>> : ConfigTransformer {};

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
  switch (QApplication::styleHints()->colorScheme()) {
  case Qt::ColorScheme::Light:
    return theme.light;
  default:
    return theme.dark;
  }
}

Manager::Manager(fs::path path) : m_userPath(path) {
  auto file = QFile(":config.jsonc");

  if (!file.open(QIODevice::ReadOnly)) { throw std::runtime_error("Failed to open default config"); }

  m_defaultData = file.readAll().toStdString();

  if (auto error = glz::read_jsonc(m_defaultConfig, m_defaultData)) {
    throw std::runtime_error(
        std::format("Failed to parse default config file: {}", glz::format_error(error)));
  }

  m_fsDebounce.setSingleShot(true);
  m_fsDebounce.setInterval(100);

  initConfig();
  reloadConfig();

  connect(&m_watcher, &QFileSystemWatcher::fileChanged, this, [this](const QString &path) {
    m_fsDebounce.start();
    m_watcher.addPath(m_userPath.c_str());
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
          glz::read_file_jsonc<glz::opts{.error_on_unknown_keys = false}>(user, m_userPath.c_str(), buf)) {
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
  switch (QApplication::styleHints()->colorScheme()) {
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
          glz::read_file_jsonc<glz::opts{.error_on_unknown_keys = false}>(user, m_userPath.c_str(), buf)) {
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

  auto res = loadUser({.resolveImports = true});

  if (!res) {
    emit configLoadingError(res.error());
    return;
  }

  ConfigValue prev = std::move(m_user);
  m_user = std::move(res.value());
  emit configChanged(m_user, prev);
}

void Manager::initConfig() {
  std::error_code ec;

  if (!fs::is_regular_file(m_userPath, ec)) {
    fs::create_directories(m_userPath.parent_path());
    writeUser({});
  }
}

Manager::PartialConfigResult Manager::load(const std::filesystem::path &path, const LoadingOptions &opts) {
  m_watcher.addPath(path.c_str());

  std::string buf;
  Partial<ConfigValue> cfg;
  auto glzError = glz::read_file_jsonc<glz::opts{.error_on_unknown_keys = false}>(cfg, path.c_str(), buf);

  if (glzError) {
    std::string glzErrMsg = glz::format_error(glzError);
    return std::unexpected(std::format("Failed to read JSONC file at {}: {}", path.c_str(), glzErrMsg));
  }

  if (opts.resolveImports) {
    for (const auto &imp : cfg.imports.value_or({})) {
      std::string importPath = expandPath(imp);

      if (!importPath.starts_with('/')) { importPath = path.parent_path() / importPath; }
      if (std::filesystem::exists(importPath)) {
        PartialConfigResult imported = load(importPath, opts);

        if (!imported) {
          return std::unexpected(
              std::format("Failed to import file \"{}\": {}", importPath, imported.error()));
        }

        cfg = merge<Partial<ConfigValue>>(imported, cfg);
      } else {
        qWarning() << "config file at" << importPath << "could not be found";
      }
    }
  }

  return cfg;
}

void Manager::prunePartial(Partial<ConfigValue> &user) {
  auto prunePreferences = [](glz::generic::object_t &obj) {
    for (auto it = obj.begin(); it != obj.end();) {
      auto cur = it++;
      if (cur->second.is_null()) { obj.erase(cur); }
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
        for (auto it2 = v.entrypoints->begin(); it2 != v.entrypoints->end();) {
          auto currentIt = it2++;
          ProviderItemData &vi = currentIt->second;

          if (vi.preferences) {
            prunePreferences(vi.preferences.value());
            if (vi.preferences->empty()) { vi.preferences.reset(); }
          }

          if (vi.alias && vi.alias->empty()) { vi.alias.reset(); }
          if (!vi.enabled.has_value() && vi.preferences.value_or({}).empty() && !vi.alias) {
            v.entrypoints->erase(currentIt);
          }
        }

        if (v.entrypoints->empty()) { v.entrypoints.reset(); }
      }

      if (!v.enabled && v.preferences.value_or({}).empty() && !v.entrypoints) { pvd.erase(currentIt); }
    }

    if (pvd.empty()) { user.providers.reset(); }
  }
}

}; // namespace config
