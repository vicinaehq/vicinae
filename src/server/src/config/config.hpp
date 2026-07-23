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
  std::optional<std::string> shortcut;
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
  int width = 770;
  int height = 480;
};

template <> struct Partial<Size> {
  std::optional<int> width;
  std::optional<int> height;
};

struct WindowCSD {
  bool enabled = true;
#ifdef Q_OS_MACOS
  int rounding = 30;
#else
  int rounding = 10;
#endif
  int borderWidth = 3;
  int shadowSize = 12;
};

template <> struct Partial<WindowCSD> {
  std::optional<bool> enabled;
  std::optional<int> rounding;
  std::optional<int> borderWidth;
  std::optional<int> shadowSize;
};

struct WindowCompactMode {
  bool enabled;
};

template <> struct Partial<WindowCompactMode> {
  std::optional<bool> enabled;
};

struct WindowConfig {
  static constexpr float OPAQUE_OPACITY = 1.0F;
  static constexpr float TRANSLUCENT_OPACITY = 0.6F;
  static constexpr float ACRYLIC_OPACITY = 0.9F;
  static constexpr float GLASS_POPUP_OPACITY = 0.2F;
  static constexpr float SURFACE_OPACITY_LIFT = 0.65F;

  std::optional<float> opacity;
  std::optional<int> rounding;
  WindowCSD clientSideDecorations;
  Size size;
  std::string screen;
  BlurConfig blur;
  WindowCompactMode compactMode;
  LayerShellConfig layerShell;

  std::string material = "auto";

  // Corner radius is a window-level property, but historically lived under client_side_decorations.
  // Fall back to that value when the flat key is unset to keep older configs working.
  int effectiveRounding() const { return rounding.value_or(clientSideDecorations.rounding); }

  std::string resolvedMaterial(bool liquidGlassAvailable, bool windowMaterialAvailable) const {
    if (material != "auto") return material;
    if (!blur.enabled) return "none";
    if (liquidGlassAvailable) return "liquid_glass";
    return windowMaterialAvailable ? "blur" : "none";
  }

  float resolvedOpacity(bool liquidGlassAvailable, bool windowMaterialAvailable) const {
    if (opacity) return *opacity;
    const std::string material = resolvedMaterial(liquidGlassAvailable, windowMaterialAvailable);
    if (material == "liquid_glass") return TRANSLUCENT_OPACITY;
#ifdef Q_OS_WIN
    if (material == "blur") return ACRYLIC_OPACITY;
#endif
    return OPAQUE_OPACITY;
  }

  // Popups draw their own material layer; on liquid glass a fixed low tint keeps the
  // glass legible regardless of the configured window opacity.
  float resolvedPopupOpacity(bool liquidGlassAvailable, bool windowMaterialAvailable) const {
    if (resolvedMaterial(liquidGlassAvailable, windowMaterialAvailable) == "liquid_glass") {
      return GLASS_POPUP_OPACITY;
    }
    return resolvedOpacity(liquidGlassAvailable, windowMaterialAvailable);
  }

  // Fills that carry meaning (selection, hover, grid tiles) fade toward invisibility if they
  // share the background's alpha, so they get a floor above the window opacity. The quadratic
  // falloff concentrates the lift on very transparent surfaces and vanishes near opaque.
  static constexpr float liftedOpacity(float base) {
    return base + (1.0F - base) * (1.0F - base) * SURFACE_OPACITY_LIFT;
  }

  float resolvedSurfaceOpacity(bool liquidGlassAvailable, bool windowMaterialAvailable) const {
    return liftedOpacity(resolvedOpacity(liquidGlassAvailable, windowMaterialAvailable));
  }

  float resolvedPopupSurfaceOpacity(bool liquidGlassAvailable, bool windowMaterialAvailable) const {
    return liftedOpacity(resolvedPopupOpacity(liquidGlassAvailable, windowMaterialAvailable));
  }
};

template <> struct Partial<WindowConfig> {
  std::optional<int> rounding;
  std::optional<float> opacity;
  std::optional<Partial<WindowCSD>> clientSideDecorations;
  std::optional<Partial<Size>> size;
  std::optional<Partial<BlurConfig>> blur;
  std::optional<Partial<WindowCompactMode>> compactMode;
  std::optional<Partial<LayerShellConfig>> layerShell;
  std::optional<std::string> material;
};

struct FontConfig {
  std::string rendering = "qt";

  struct FontSpec {
    std::string family = "auto";
#ifdef Q_OS_MACOS
    float size = 13;
#else
    float size = 10.5;
#endif
  } normal;
};

template <> struct Partial<FontConfig> {
  std::optional<std::string> rendering;

  struct FontSpec {
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

struct TelemetryConfig {
  bool systemInfo = true;
};

template <> struct Partial<TelemetryConfig> {
  std::optional<bool> systemInfo;
};

using KeybindMap = std::map<std::string, std::string>;

using ProviderMap = std::map<std::string, ProviderData>;

static constexpr const char *SCHEMA = "https://vicinae.com/schemas/config.json";

struct InputServer {
  bool enabled = true;
};

template <> struct Partial<InputServer> {
  std::optional<bool> enabled;
};

struct GlobalShortcuts {
#if defined(Q_OS_MACOS) || defined(Q_OS_WIN)
  std::optional<std::string> toggle = "alt+space";
#else
  std::optional<std::string> toggle = "super+control+space";
#endif
};

template <> struct Partial<GlobalShortcuts> {
  std::optional<std::string> toggle;
};

struct ConfigValue {
  std::string schema = SCHEMA;
  std::vector<std::string> imports;
  bool searchFilesInRoot = false;
  bool closeOnFocusLoss = false;
  bool considerPreedit = false;
  bool popToRootOnClose = false;
  // seconds; with popToRootOnClose, reopening within this window keeps the query/view stack
  int popToRootOnCloseDelay = 0;
  bool popOnBackspace = true;
  bool activateOnSingleClick = false;
  bool wrapNavigation = false;
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
  bool encryptSensitiveData = false;
#else
  bool encryptSensitiveData = true;
#endif
  std::string escapeKeyBehavior;
  std::string faviconService = "twenty";
  std::string keybinding = "default";
  int pixmapCacheMb = 50;

  InputServer inputServer;
  GlobalShortcuts globalShortcuts;

  FontConfig font;
  ThemeConfig theme;
  TelemetryConfig telemetry;

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
  std::optional<int> popToRootOnCloseDelay;
  std::optional<bool> popOnBackspace;
  std::optional<bool> activateOnSingleClick;
  std::optional<bool> wrapNavigation;
  std::optional<bool> encryptSensitiveData;
  std::optional<std::string> escapeKeyBehavior;
  std::optional<std::string> faviconService;
  std::optional<std::string> keybinding;
  std::optional<int> pixmapCacheMb;
  std::optional<bool> searchFilesInRoot;
  std::optional<Partial<InputServer>> inputServer;
  std::optional<Partial<GlobalShortcuts>> globalShortcuts;

  std::optional<Partial<FontConfig>> font;
  std::optional<Partial<ThemeConfig>> theme;
  std::optional<Partial<TelemetryConfig>> telemetry;

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
    std::unordered_set<std::filesystem::path> &visited;
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

  static void print(const ConfigValue &value);

  const ConfigValue &value() const { return m_user; }

private:
  static void prunePartial(Partial<ConfigValue> &user);

  PartialConfigResult load(const std::filesystem::path &path, const LoadingOptions &opts);

  static std::filesystem::path resolvePath(const std::filesystem::path &path,
                                           const std::filesystem::path &cwd);

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

  std::vector<std::string> m_envOverrides;
};
}; // namespace config

#define SNAKE_CASIFY(T)                                                                                      \
  template <> struct glz::meta<T> : glz::snake_case {};                                                      \
  template <> struct glz::meta<config::Partial<T>> : glz::snake_case {};

SNAKE_CASIFY(config::LayerShellConfig);
SNAKE_CASIFY(config::WindowConfig);
SNAKE_CASIFY(config::SystemThemeConfig);
SNAKE_CASIFY(config::ThemeConfig);
SNAKE_CASIFY(config::TelemetryConfig);
SNAKE_CASIFY(config::WindowCSD);
SNAKE_CASIFY(config::GlobalShortcuts);

#undef SNAKE_CASIFY

struct ConfigTransformer : glz::snake_case {
  static constexpr std::string rename_key(const std::string_view key) {
    if (key == "schema") return "$schema";
    return glz::to_snake_case(key);
  }
};

template <> struct glz::meta<config::ConfigValue> : ConfigTransformer {};
template <> struct glz::meta<config::Partial<config::ConfigValue>> : ConfigTransformer {};
