#pragma once
#include <expected>
#include <filesystem>
#include "common.hpp"
#include "glaze/glaze.hpp"
#include "utils.hpp"
#include "vicinae.hpp"
#include <fstream>
#include <glaze/core/common.hpp>
#include <glaze/core/reflect.hpp>
#include <glaze/json/prettify.hpp>
#include <glaze/json/ptr.hpp>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include <glaze/util/key_transformers.hpp>
#include <ios>
#include <iostream>
#include <qdnslookup.h>
#include <qfilesystemwatcher.h>
#include <qmargins.h>
#include <string>
#include <string_view>

namespace config {

struct ProviderItemData {
  std::optional<std::string> alias;
  std::optional<bool> enabled;
  std::optional<glz::generic::object_t> preferences;
};

struct ProviderData {
  std::optional<bool> enabled;
  std::optional<glz::generic::object_t> preferences;
  std::map<std::string, ProviderItemData> items;
};

// TODO: populate this

template <typename T> struct Partial;

struct LayerShellConfig {
  enum class Layer { Top, Overlay };

  std::string scope = "vicinae";
  Layer layer = Layer::Top;
  bool enabled = true;
};

template <> struct Partial<LayerShellConfig> {
  std::optional<std::string> scope;
  std::optional<LayerShellConfig::Layer> layer;
  std::optional<bool> enabled;
};

struct Size {
  int width;
  int height;
};

template <> struct Partial<Size> {
  std::optional<int> width;
  std::optional<int> height;
};

struct WindowConfig {
  static const constexpr Size DFLT_SIZE = {770, 480};
  static const constexpr int DFLT_ROUNDING = 6;
  static const constexpr float DFLT_OPACITY = 0.98;
  static const constexpr bool DFLT_CSD = true;

  int rounding = DFLT_ROUNDING;
  float opacity = DFLT_OPACITY;
  bool csd = DFLT_CSD;
  Size size = DFLT_SIZE;

  LayerShellConfig layerShell;
};

template <> struct Partial<WindowConfig> {
  std::optional<int> rounding;
  std::optional<float> opacity;
  std::optional<bool> csd;
  std::optional<Partial<Size>> size;
  std::optional<Partial<LayerShellConfig>> layerShell;
};

struct FontConfig {
  std::optional<std::string> normal;
  float size = 10.5;
};

template <> struct Partial<FontConfig> {
  std::optional<std::string> normal;
  std::optional<float> size;
};

struct ThemeConfig {
  std::optional<std::string> iconTheme;
  std::string name = "vicinae-dark";
};

template <> struct Partial<ThemeConfig> {
  std::optional<std::string> iconTheme;
  std::optional<std::string> name;
};

struct Meta {
  std::vector<std::string> imports;
  bool insecurelySerializePasswordPreferences = false;
};

template <> struct Partial<Meta> {
  std::optional<bool> insecurelySerializePasswordPreferences = false;
  std::optional<std::vector<std::string>> imports;
};

struct Margin {
  int left;
  int top;
  int right;
  int bottom;

  operator QMargins() const { return QMargins{left, top, right, bottom}; }
};

struct Header {
  Margin margins = {15, 5, 15, 5};
};

template <> struct Partial<Header> {
  std::optional<Margin> margins;
};

using KeybindMap = std::map<std::string, std::string>;
using ProviderMap = std::map<std::string, ProviderData>;

struct ConfigValue {
  static const constexpr bool DFLT_CLOSE_ON_FOCUS_LOSS = true;
  static const constexpr bool DFLT_CONSIDER_PRE_EDIT = false;
  static const constexpr bool DFLT_POP_TO_ROOT_ON_CLOSE = false;

  Meta meta;
  bool closeOnFocusLoss = DFLT_CLOSE_ON_FOCUS_LOSS;
  bool considerPreedit = DFLT_CONSIDER_PRE_EDIT;
  bool popToRootOnClose = DFLT_POP_TO_ROOT_ON_CLOSE;
  std::string faviconService = "google";
  std::string keybinding = "default";

  FontConfig font;
  ThemeConfig theme;

  std::vector<std::string> favorites;
  std::vector<std::string> fallbacks;
  WindowConfig window;
  Header header;

  // we use maps to keep serialized output predictable
  KeybindMap keybinds;
  ProviderMap providers;

  std::optional<glz::generic::object_t> providerPreferences(std::string_view id) const {
    if (auto it = providers.find(std::string{id}); it != providers.end()) { return it->second.preferences; }
    return std::nullopt;
  }

  glz::generic::object_t preferences(const EntrypointId &id) const {
    glz::generic::object_t prefs;
    std::string buf;

    if (auto it = providers.find(id.provider); it != providers.end()) {
      prefs = it->second.preferences.value_or({});

      if (auto it2 = it->second.items.find(id.entrypoint); it2 != it->second.items.end()) {
        if (auto preferences = it2->second.preferences) {
          auto merged = glz::merge{prefs, preferences.value()};
          (void)glz::write_json(merged, buf);
          (void)glz::read_json(prefs, buf);
        }
      }
    }

    return prefs;
  }
};

template <> struct Partial<ConfigValue> {
  std::optional<Partial<Meta>> meta;
  std::optional<bool> closeOnFocusLoss;
  std::optional<bool> considerPreedit;
  std::optional<bool> popToRootOnClose;
  std::optional<std::string> faviconService;
  std::optional<std::string> keybinding;

  std::optional<Partial<FontConfig>> font;
  std::optional<Partial<ThemeConfig>> theme;

  std::optional<std::vector<std::string>> favorites;
  std::optional<std::vector<std::string>> fallbacks;
  std::optional<Partial<WindowConfig>> window;
  std::optional<Partial<Header>> header;

  std::optional<KeybindMap> keybinds;
  std::optional<ProviderMap> providers;
};

class Manager : public QObject {
  Q_OBJECT

signals:
  void configChanged(const ConfigValue &next, const ConfigValue &prev) const;
  void configLoadingError(std::string_view message) const;

private:
  static constexpr const char *TOP_COMMENT = R"(// This is the main vicinae user config file.
//
// You can make manual edits to this file, however you should keep in mind that this file may be written to by vicinae directly,
// typically when a configuration change is made through the GUI.
// When that happens, any custom comments or formatting will be lost.
//
// If you want to maintain a configuration file with your own comments and formatting, you should create a separate one
// and import it here using the `$import` meta key.
//
// Learn more about configuration at https://docs.vicinae.com/config)";

  struct LoadingOptions {
    bool resolveImports;
  };

  template <typename T> T merge(const auto &v1, const auto &v2) {
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

  Partial<ConfigValue> load(const std::filesystem::path &path,
                            const LoadingOptions &opts = {.resolveImports = true}) {
    m_watcher.addPath(path.parent_path().c_str());

    std::cout << "loading" << path << std::endl;

    std::string buf;
    Partial<ConfigValue> cfg;
    auto res = glz::read_file_jsonc<glz::opts{.error_on_unknown_keys = false}>(cfg, path.c_str(), buf);

    if (res) { std::cerr << "Failed to load " << path << glz::format_error(res) << std::endl; }

    if (opts.resolveImports) {
      if (cfg.meta && cfg.meta->imports) {
        for (const auto &imp : cfg.meta->imports.value()) {
          std::string importPath = expandPath(imp);

          if (!importPath.starts_with('/')) { importPath = path.parent_path() / importPath; }
          if (std::filesystem::exists(importPath)) {
            Partial<ConfigValue> imported = load(importPath, opts);
            cfg = merge<Partial<ConfigValue>>(imported, cfg);
          } else {
            qWarning() << "config file at" << importPath << "could not be found";
          }
        }
      }
    }

    return cfg;
  }

  ConfigValue loadUser(const LoadingOptions &opts) {
    auto user = load(m_userPath, opts);
    return merge<ConfigValue>(defaultConfig(), user);
  }

public:
  Manager(std::filesystem::path path = Omnicast::configDir() / "vicinae.json") : m_userPath(path) {
    using namespace std::chrono_literals;
    m_watcherDebounce.setInterval(100ms);
    m_watcherDebounce.setSingleShot(true);
    m_user = loadUser({.resolveImports = true});

    connect(&m_watcherDebounce, &QTimer::timeout, this, [this]() {
      ConfigValue prev = std::move(m_user);
      m_user = loadUser({.resolveImports = true});
      emit configChanged(m_user, prev);
    });
    connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this, [this]() { m_watcherDebounce.start(); });
  }

  ConfigValue defaultConfig() const { return {}; }

  bool mergeProviderWithUser(std::string_view id, ProviderData &&data) {
    return mergeWithUser({.providers = ProviderMap{{std::string{id}, data}}});
  }

  bool mergeEntrypointWithUser(const EntrypointId &id, ProviderItemData &&data) {
    ProviderMap providers;
    providers[id.provider] = {.items = {{id.entrypoint, data}}};
    return mergeWithUser({.providers = providers});
  }

  bool mergeWithUser(const Partial<ConfigValue> &patch) {
    std::string buf;
    Partial<ConfigValue> user;

    if (auto error =
            glz::read_file_jsonc<glz::opts{.error_on_unknown_keys = false}>(user, m_userPath.c_str(), buf)) {
      qWarning() << "Failed to read user config as partial";
      return false;
    }

    if (auto error = glz::write_json(glz::merge{user, patch}, buf)) {
      qWarning() << "Failed to merge partials";
      return false;
    }

    if (auto error = glz::read_json(user, buf)) {
      qWarning() << "Failed to read merged partials";
      return false;
    }

    return writeUser(user);
  }

  bool writeUser(const Partial<ConfigValue> &cfg) {
    std::string buf;

    if (auto error = glz::write_json(cfg, buf)) {
      qWarning() << "Failed to write json" << glz::format_error(error);
      return false;
    }

    {
      std::ofstream ofs(m_userPath);
      ofs << TOP_COMMENT << "\n\n" << glz::prettify_json(buf);
    }

    ConfigValue prev = std::move(m_user);
    m_user = loadUser({.resolveImports = true});
    emit configChanged(m_user, prev);

    return true;
  }

  void print(const ConfigValue &value) const {
    std::string buf;
    auto res = glz::write_json(value, buf);
    std::cout << glz::prettify_json(buf) << std::endl;
  }

  const ConfigValue &user() const { return m_user; }
  const ConfigValue &value() const { return m_user; }

private:
  void prunePartial(Partial<ConfigValue> &user) {
    if (user.providers) {
      auto &pvd = user.providers.value();

      for (auto it = pvd.begin(); it != pvd.end();) {
        auto &v = it->second;
        auto currentIt = it++;

        for (auto it2 = v.items.begin(); it2 != v.items.end();) {
          auto currentIt = it2++;
          const ProviderItemData &vi = currentIt->second;
          if (!vi.enabled.has_value() && vi.preferences.value_or({}).empty() && !vi.alias) {
            v.items.erase(currentIt);
          }
        }

        if (!v.enabled && v.preferences.value_or({}).empty() && v.items.empty()) { pvd.erase(currentIt); }
      }

      if (pvd.empty()) { user.providers.reset(); }
    }
  }

  QTimer m_watcherDebounce;
  QFileSystemWatcher m_watcher;
  std::filesystem::path m_userPath;
  ConfigValue m_user;
};
}; // namespace config

template <> struct glz::meta<config::ConfigValue> {
  static constexpr std::string rename_key(const std::string_view key) { return glz::to_snake_case(key); }
};

template <> struct glz::meta<config::Partial<config::ConfigValue>> {
  static constexpr std::string rename_key(const std::string_view key) { return glz::to_snake_case(key); }
};
