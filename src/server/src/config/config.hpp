#pragma once
#include <expected>
#include <filesystem>
#include "common/entrypoint.hpp"
#include "vicinae.hpp"
#include <glaze/core/common.hpp>
#include <glaze/core/reflect.hpp>
#include <glaze/json.hpp>
#include <glaze/util/key_transformers.hpp>
#include <iostream>
#include <qdatetime.h>
#include <qfilesystemwatcher.h>
#include <qmargins.h>
#include <QTimer>
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
  std::map<std::string, ProviderItemData> entrypoints;
};

template <typename T> struct Partial;

struct SystemThemeConfig {
  std::string name;
  std::string iconTheme;
};

struct ThemeConfig {
  SystemThemeConfig light;
  SystemThemeConfig dark;
};

template <> struct Partial<SystemThemeConfig> {
  std::optional<std::string> name;
  std::optional<std::string> iconTheme;
};

template <> struct Partial<ThemeConfig> {
  std::optional<Partial<SystemThemeConfig>> light;
  std::optional<Partial<SystemThemeConfig>> dark;
};

template <> struct Partial<ProviderData> {
  std::optional<bool> enabled;
  std::optional<glz::generic::object_t> preferences;
  std::optional<std::map<std::string, ProviderItemData>> entrypoints;
};

struct LayerShellConfig {
  std::string keyboardInteractivity = "exclusive";
  std::string layer = "top";
  bool enabled = true;
};

template <> struct Partial<LayerShellConfig> {
  std::optional<std::string> layer;
  std::optional<std::string> keyboardInteractivity;
  std::optional<bool> enabled;
};

struct BlurConfig {
  bool enabled = true;
};

template <> struct Partial<BlurConfig> {
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

struct WindowCSD {
  bool enabled = true;
  int rounding = 10;
  int borderWidth = 3;
};

template <> struct Partial<WindowCSD> {
  std::optional<bool> enabled;
  std::optional<int> rounding;
  std::optional<int> borderWidth;
};

struct WindowCompactMode {
  bool enabled;
};

template <> struct Partial<WindowCompactMode> {
  std::optional<bool> enabled;
};

struct WindowConfig {
  float opacity;
  WindowCSD clientSideDecorations;
  Size size;
  std::string screen;
  bool dimAround = true;
  BlurConfig blur;
  WindowCompactMode compactMode;
  LayerShellConfig layerShell;
};

template <> struct Partial<WindowConfig> {
  std::optional<int> rounding;
  std::optional<float> opacity;
  std::optional<Partial<WindowCSD>> clientSideDecorations;
  std::optional<Partial<Size>> size;
  std::optional<bool> dimAround;
  std::optional<Partial<BlurConfig>> blur;
  std::optional<Partial<WindowCompactMode>> compactMode;
  std::optional<Partial<LayerShellConfig>> layerShell;
};

struct FontConfig {
  struct {
    std::string family = "auto";
    float size = 10.5;
  } normal;
};

template <> struct Partial<FontConfig> {
  struct {
    std::optional<std::string> family;
    std::optional<float> size;
  } normal;
};

struct Margin {
  int left;
  int top;
  int right;
  int bottom;

  operator QMargins() const { return QMargins{left, top, right, bottom}; }
};

struct Header {
  int height = 60;
  Margin margins = {15, 5, 15, 5};
};

template <> struct Partial<Header> {
  std::optional<Margin> margins;
  std::optional<int> height;
};

struct Footer {
  int height = 40;
};

template <> struct Partial<Footer> {
  std::optional<int> height;
};

using KeybindMap = std::map<std::string, std::string>;

using ProviderMap = std::map<std::string, ProviderData>;

static constexpr const char *SCHEMA = "https://vicinae.com/schemas/config.json";

struct ConfigValue {
  std::string schema = SCHEMA;
  std::vector<std::string> imports;
  bool searchFilesInRoot = false;
  bool closeOnFocusLoss = false;
  bool considerPreedit = false;
  bool popToRootOnClose = false;
  std::string escapeKeyBehavior;
  std::string faviconService = "twenty";
  std::string keybinding = "default";
  int pixmapCacheMb = 50;

  FontConfig font;
  ThemeConfig theme;

  WindowConfig launcherWindow;
  Header header;
  Footer footer;

  // we use maps to keep serialized output predictable
  KeybindMap keybinds;

  std::vector<std::string> favorites;
  std::vector<std::string> fallbacks;

  ProviderMap providers;

  std::optional<glz::generic::object_t> providerPreferences(std::string_view id) const {
    if (auto it = providers.find(std::string{id}); it != providers.end()) { return it->second.preferences; }
    return std::nullopt;
  }

  std::optional<glz::generic::object_t> preferences(const EntrypointId &id) const {
    if (auto it = providers.find(id.provider); it != providers.end()) {
      if (auto it2 = it->second.entrypoints.find(id.entrypoint); it2 != it->second.entrypoints.end()) {
        if (auto preferences = it2->second.preferences) { return preferences; }
      }
    }

    return std::nullopt;
  }

  const SystemThemeConfig &systemTheme() const;
};

using PartialValue = Partial<ConfigValue>;

template <> struct Partial<ConfigValue> {
  std::string schema = SCHEMA;
  std::optional<std::vector<std::string>> imports;
  std::optional<bool> closeOnFocusLoss;
  std::optional<bool> considerPreedit;
  std::optional<bool> popToRootOnClose;
  std::optional<std::string> escapeKeyBehavior;
  std::optional<std::string> faviconService;
  std::optional<std::string> keybinding;
  std::optional<int> pixmapCacheMb;
  std::optional<bool> searchFilesInRoot;

  std::optional<Partial<FontConfig>> font;
  std::optional<Partial<ThemeConfig>> theme;

  std::optional<Partial<WindowConfig>> launcherWindow;
  std::optional<Partial<Header>> header;
  std::optional<Partial<Footer>> footer;

  std::optional<KeybindMap> keybinds;

  std::optional<std::vector<std::string>> favorites;
  std::optional<std::vector<std::string>> fallbacks;

  std::optional<std::map<std::string, Partial<ProviderData>>> providers;
};

class Manager : public QObject {
  Q_OBJECT

signals:
  void configChanged(const ConfigValue &next, const ConfigValue &prev) const;
  void configLoadingError(std::string_view message) const;

private:
  struct LoadingOptions {
    bool resolveImports;
  };

  using ConfigResult = std::expected<ConfigValue, std::string>;
  using PartialConfigResult = std::expected<Partial<ConfigValue>, std::string>;

public:
  Manager(std::filesystem::path path = Omnicast::configDir() / "settings.json");

  ConfigValue defaultConfig() const;
  const char *defaultConfigData() const;

  bool mergeProviderWithUser(std::string_view id, const Partial<ProviderData> &data);

  /**
   * Update the current user configuration, instead of merging.
   */
  bool updateUser(const std::function<void(Partial<ConfigValue> &value)> &updater);

  bool mergeEntrypointWithUser(const EntrypointId &id, const ProviderItemData &data);

  bool mergeWithUser(const Partial<ConfigValue> &patch);

  bool mergeThemeConfig(const config::Partial<config::SystemThemeConfig> &cfg);

  const SystemThemeConfig &theme() const;

  std::filesystem::path path() const { return m_userPath; }

  static void print(const ConfigValue &value) {
    std::string buf;
    auto res = glz::write_json(value, buf);
    std::cout << glz::prettify_json(buf) << std::endl;
  }

  const ConfigValue &value() const { return m_user; }

private:
  static void prunePartial(Partial<ConfigValue> &user);

  PartialConfigResult load(const std::filesystem::path &path,
                           const LoadingOptions &opts = {.resolveImports = true});

  void reloadConfig();
  ConfigResult loadUser(const LoadingOptions &opts);
  bool writeUser(const Partial<ConfigValue> &cfg);
  void initConfig();

  QFileSystemWatcher m_watcher;
  std::filesystem::path m_userPath;
  ConfigValue m_user;

  std::string m_defaultData;
  ConfigValue m_defaultConfig;

  QTimer m_fsDebounce;
};
}; // namespace config
