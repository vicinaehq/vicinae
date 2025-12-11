#pragma once
#include <expected>
#include <filesystem>
#include "common.hpp"
#include "glaze/glaze.hpp"
#include <fstream>
#include <glaze/core/common.hpp>
#include <glaze/core/reflect.hpp>
#include <glaze/json/prettify.hpp>
#include <glaze/json/ptr.hpp>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include <iostream>
#include <qdnslookup.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonparseerror.h>
#include <qjsonvalue.h>
#include <qlogging.h>
#include <string>

namespace config {

struct PreferenceData {};

struct ProviderItemData {
  std::optional<std::string> alias;
  std::optional<bool> enabled;
  std::optional<glz::raw_json> preferences;
};

struct ProviderData {
  std::optional<bool> enabled;
  std::optional<glz::raw_json> preferences;
  std::unordered_map<std::string, ProviderItemData> items;
};

struct WindowConfig {
  static const constexpr int DFLT_ROUNDING = 6;
  static const constexpr int DFLT_OPACITY = 6;
  static const constexpr bool DFLT_CSD = true;

  int rounding = DFLT_ROUNDING;
  float opacity = DFLT_ROUNDING;
  bool csd = DFLT_CSD;
};

struct FontConfig {
  std::optional<std::string> normal;
  float size = 10.5;
};

struct ThemeConfig {
  std::optional<std::string> iconTheme;
  std::string name = "vicinae-dark";
};

struct ConfigValue {
  static const constexpr bool DFLT_CLOSE_ON_FOCUS_LOSS = true;
  static const constexpr bool DFLT_CONSIDER_PRE_EDIT = false;
  static const constexpr bool DFLT_POP_TO_ROOT_ON_CLOSE = false;

  std::vector<std::string> imports;
  bool closeOnFocusLoss = DFLT_CLOSE_ON_FOCUS_LOSS;
  bool considerPreedit = DFLT_CONSIDER_PRE_EDIT;
  bool popToRootOnClose = DFLT_POP_TO_ROOT_ON_CLOSE;
  std::optional<std::string> faviconService;

  FontConfig font;
  ThemeConfig theme;

  std::vector<std::string> favorites;
  std::vector<std::string> fallbacks;
  std::optional<WindowConfig> window;
  std::unordered_map<std::string, std::string> keybinds;
  std::unordered_map<std::string, ProviderData> providers;
};

class Manager {

private:
  struct LoadingOptions {
    bool mergeImports;
  };

  ConfigValue load(const std::filesystem::path &path, LoadingOptions &&opts = {.mergeImports = false}) {
    ConfigValue cfg;
    auto res = glz::read_file_jsonc<glz::opts{.error_on_unknown_keys = false}>(cfg, path.c_str(), m_buf);
    if (res) { std::cerr << "Failed to load" << glz::format_error(res); }
    return cfg;
  }

  ConfigValue loadUser() { return load("config.json"); }

public:
  Manager() { m_user = loadUser(); }

  void mergeProviderWithUser(std::string_view id, ProviderData &&data) {
    mergeWithUser({.providers = {{std::string{id}, data}}});
  }

  void mergeEntrypointWithUser(const EntrypointId &id, ProviderItemData &&data) {
    ConfigValue patch;
    patch.providers[id.provider] = {.items = {{id.entrypoint, data}}};
    mergeWithUser(std::move(patch));
  }

  void mergeWithUser(ConfigValue &&value) {
    std::string buf;
    glz::write_json(glz::merge{m_user, value}, buf);
    ConfigValue cfg;
    if (auto error = glz::read_json(cfg, buf)) { qWarning() << "Failed to read" << glz::format_error(error); }
    glz::write_json(cfg, buf);
    std::ofstream ofs("config.json");
    ofs << glz::prettify_json(buf);
  }

  void print(const ConfigValue &value) const {
    std::string buf;
    auto res = glz::write_json(value, buf);
    std::cout << glz::prettify_json(buf) << std::endl;
  }

  const ConfigValue &user() const { return m_user; }

  ConfigValue m_user;
  std::string m_buf;
};

}; // namespace config

template <> struct glz::meta<config::WindowConfig> {
  template <class T>
  static constexpr bool skip_if(T &&value, std::string_view key, const glz::meta_context &) {
    using V = std::decay_t<T>;
    if constexpr (std::same_as<V, int>) {
      return key == "rounding" && value == config::WindowConfig::DFLT_ROUNDING;
    } else if constexpr (std::same_as<V, float>) {
      return key == "opacity" && value == config::WindowConfig::DFLT_OPACITY;
    } else if constexpr (std::same_as<V, bool>) {
      return key == "csd" && value == config::WindowConfig::DFLT_CSD;
    }
    return false;
  }
};

template <> struct glz::meta<config::ConfigValue> {
  template <class T>
  static constexpr bool skip_if(T &&value, std::string_view key, const glz::meta_context &) {
    using V = std::decay_t<T>;
    if constexpr (std::same_as<V, bool>) {
      return key == "closeOnFocusLoss" && value == config::ConfigValue::DFLT_CLOSE_ON_FOCUS_LOSS;
    } else if constexpr (std::same_as<V, bool>) {
      return key == "considerPreedit" && value == config::ConfigValue::DFLT_CONSIDER_PRE_EDIT;
    } else if constexpr (std::same_as<V, bool>) {
      return key == "popToRootOnClose" && value == config::ConfigValue::DFLT_POP_TO_ROOT_ON_CLOSE;
    }
    return false;
  }
};
