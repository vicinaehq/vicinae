#pragma once
#include <memory>
#include <qobject.h>

class AbstractWindowManager;
class AppService;
class OmniDatabase;
class LocalStorageService;
class ExtensionManager;
class ClipboardService;
class FontService;
class RootItemManager;
class ConfigService;
class ShortcutService;
class ToastService;
class GlyphService;
class CalculatorService;
class FileService;
class RaycastStoreService;
class VicinaeStoreService;
class ExtensionRegistry;
class OAuthService;
class WindowManager;
class WallpaperManager;
class PowerManager;
class ScriptCommandService;
class AbstractSnippetServer;
#ifdef Q_OS_LINUX
class LinuxInputServer;
#endif
class SnippetService;
class BrowserExtensionService;
class WindowMaterialManager;
class ShortcutInhibitManager;
class FileChooserService;
class NewsService;
class PasteService;
class TelemetryService;
class UpdateService;
class AudioControlService;
class AppRuntime;
class GlobalShortcutService;
class AbstractLayoutSwitchService;

namespace config {
class Manager;
};

class ServiceRegistry : public QObject {

public:
  ~ServiceRegistry() override;
  static ServiceRegistry *instance();
  RootItemManager *rootItemManager() const;
  config::Manager *config() const;
  OmniDatabase *omniDb() const;
  CalculatorService *calculatorService() const;
  WindowManager *windowManager() const;
  WallpaperManager *wallpaperManager() const;
  GlyphService *glyphService() const;
  FontService *fontService() const;
  LocalStorageService *localStorage() const;
  ExtensionManager *extensionManager() const;
  ClipboardService *clipman() const;
  AppService *appDb() const;
  ToastService *toastService() const;
  ShortcutService *shortcuts() const;
  FileService *fileService() const;
  RaycastStoreService *raycastStore() const;
  VicinaeStoreService *vicinaeStore() const;
  ExtensionRegistry *extensionRegistry() const;
  OAuthService *oauthService() const;
  PowerManager *powerManager() const;
  ScriptCommandService *scriptDb() const;
  BrowserExtensionService *browserExtension() const;
#ifdef Q_OS_LINUX
  LinuxInputServer *inputServer() const;
#endif
  SnippetService *snippetService() const;
  PasteService *pasteService() const;
  FileChooserService *fileChooserService() const;
  NewsService *newsService() const;
  WindowMaterialManager *windowMaterialManager() const;
  ShortcutInhibitManager *shortcutInhibitManager() const;
  TelemetryService *telemetry() const;
  UpdateService *updateService() const;
  AudioControlService *audioControl() const;
  AppRuntime *appRuntime() const;
  GlobalShortcutService *globalShortcuts() const;
  AbstractLayoutSwitchService *layoutSwitch() const;

  void setPowerManager(std::unique_ptr<PowerManager> manager);
  void setWindowManager(std::unique_ptr<WindowManager> manager);
  void setWallpaperManager(std::unique_ptr<WallpaperManager> manager);
  void setRootItemManager(std::unique_ptr<RootItemManager> manager);
  void setRaycastStore(std::unique_ptr<RaycastStoreService> service);
  void setScriptDb(std::unique_ptr<ScriptCommandService> service);
  void setVicinaeStore(std::unique_ptr<VicinaeStoreService> service);
  void setOAuthService(std::unique_ptr<OAuthService> service);
  void setConfig(std::unique_ptr<config::Manager> cfg);
  void setShortcutService(std::unique_ptr<ShortcutService> service);
  void setCalculatorService(std::unique_ptr<CalculatorService> service);
  void setExtensionRegistry(std::unique_ptr<ExtensionRegistry> service);
  void setFileService(std::unique_ptr<FileService> service);
  void setGlyphService(std::unique_ptr<GlyphService> service);
  void setToastService(std::unique_ptr<ToastService> service);
  void setFontService(std::unique_ptr<FontService> font);
  void setOmniDb(std::unique_ptr<OmniDatabase> service);
  void setWindowManager(std::unique_ptr<AbstractWindowManager> service);
  void setLocalStorage(std::unique_ptr<LocalStorageService> service);
  void setExtensionManager(std::unique_ptr<ExtensionManager> service);
  void setClipman(std::unique_ptr<ClipboardService> service);
  void setAppDb(std::unique_ptr<AppService> service);
  void setBrowserExtension(std::unique_ptr<BrowserExtensionService> service);
#ifdef Q_OS_LINUX
  void setInputServer(std::unique_ptr<LinuxInputServer> server);
#endif
  void setSnippetServerBackend(std::unique_ptr<AbstractSnippetServer> backend);
  void setSnippetService(std::unique_ptr<SnippetService> service);
  void setPasteService(std::unique_ptr<PasteService> service);
  void setFileChooserService(std::unique_ptr<FileChooserService> service);
  void setNewsService(std::unique_ptr<NewsService> service);
  void setWindowMaterialManager(std::unique_ptr<WindowMaterialManager> manager);
  void setShortcutInhibitManager(std::unique_ptr<ShortcutInhibitManager> manager);
  void setTelemetry(std::unique_ptr<TelemetryService> telemetry);
  void setUpdateService(std::unique_ptr<UpdateService> service);
  void setAudioControl(std::unique_ptr<AudioControlService> service);
  void setAppRuntime(std::unique_ptr<AppRuntime> service);
  void setGlobalShortcuts(std::unique_ptr<GlobalShortcutService> service);
  void setLayoutSwitch(std::unique_ptr<AbstractLayoutSwitchService> service);

private:
  std::unique_ptr<WindowManager> m_windowManager;
  std::unique_ptr<WallpaperManager> m_wallpaperManager;
  std::unique_ptr<AppService> m_appDb;
  std::unique_ptr<OmniDatabase> m_omniDb;
  std::unique_ptr<LocalStorageService> m_localStorage;
  std::unique_ptr<ExtensionManager> m_extensionManager;
  std::unique_ptr<ClipboardService> m_clipman;
  std::unique_ptr<FontService> m_fontService;
  std::unique_ptr<RootItemManager> m_rootItemManager;
  std::unique_ptr<config::Manager> m_config;
  std::unique_ptr<ShortcutService> m_shortcutService;
  std::unique_ptr<ToastService> m_toastService;
  std::unique_ptr<GlyphService> m_glyphService;
  std::unique_ptr<CalculatorService> m_calculatorService;
  std::unique_ptr<FileService> m_fileService;
  std::unique_ptr<RaycastStoreService> m_raycastStoreService;
  std::unique_ptr<VicinaeStoreService> m_vicinaeStoreService;
  std::unique_ptr<ExtensionRegistry> m_extensionRegistry;
  std::unique_ptr<OAuthService> m_oauthService;
  std::unique_ptr<PowerManager> m_powerManager;
  std::unique_ptr<ScriptCommandService> m_scriptCommandService;
  std::unique_ptr<BrowserExtensionService> m_browserExtensionService;
#ifdef Q_OS_LINUX
  std::unique_ptr<LinuxInputServer> m_inputServer;
#endif
  std::unique_ptr<AbstractSnippetServer> m_snippetServerBackend;
  std::unique_ptr<SnippetService> m_snippetService;
  std::unique_ptr<PasteService> m_pasteService;
  std::unique_ptr<FileChooserService> m_fileChooserService;
  std::unique_ptr<NewsService> m_newsService;
  std::unique_ptr<WindowMaterialManager> m_windowMaterialManager;
  std::unique_ptr<ShortcutInhibitManager> m_shortcutInhibitManager;
  std::unique_ptr<TelemetryService> m_telemetry;
  std::unique_ptr<UpdateService> m_updateService;
  std::unique_ptr<AudioControlService> m_audioControl;
  std::unique_ptr<AppRuntime> m_appRuntime;
  std::unique_ptr<GlobalShortcutService> m_globalShortcuts;
  std::unique_ptr<AbstractLayoutSwitchService> m_layoutSwitch;
};
