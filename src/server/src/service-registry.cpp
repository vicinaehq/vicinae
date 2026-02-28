#include "service-registry.hpp"
#ifdef Q_OS_LINUX
#include "services/input-server/linux-input-server.hpp"
#endif
#include "services/audio-control/audio-control-service.hpp"
#include "extension/manager/extension-manager.hpp"
#include "font-service.hpp"
#include "omni-database.hpp"
#include "services/app-service/app-service.hpp"
#include "services/background-effect/background-effect-manager.hpp"
#include "services/shortcut-inhibit/shortcut-inhibit-manager.hpp"
#include "services/browser-extension-service.hpp"
#include "services/power-manager/power-manager.hpp"
#include "services/script-command/script-command-service.hpp"
#include "services/shortcut/shortcut-service.hpp"
#include "services/calculator-service/calculator-service.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/glyph-service/glyph-service.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include "services/files-service/file-service.hpp"
#include "services/local-storage/local-storage-service.hpp"
#include "services/oauth/oauth-service.hpp"
#include "services/raycast/raycast-store.hpp"
#include "services/extension-store/vicinae-store.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "services/telemetry/telemetry-service.hpp"
#include "services/toast/toast-service.hpp"
#include "services/window-manager/window-manager.hpp"
#include "services/app-runtime/app-runtime.hpp"
#include "services/global-shortcuts/global-shortcut-service.hpp"
#include "services/snippet/snippet-service.hpp"
#include "services/paste/paste-service.hpp"
#include "services/file-chooser/file-chooser-service.hpp"
#include "services/news/news-service.hpp"
#include "services/ai/ai-service.hpp"
#include "config/config.hpp"

ServiceRegistry::~ServiceRegistry() = default;

RootItemManager *ServiceRegistry::rootItemManager() const { return m_rootItemManager.get(); }
config::Manager *ServiceRegistry::config() const { return m_config.get(); }
OmniDatabase *ServiceRegistry::omniDb() const { return m_omniDb.get(); }
CalculatorService *ServiceRegistry::calculatorService() const { return m_calculatorService.get(); }
WindowManager *ServiceRegistry::windowManager() const { return m_windowManager.get(); }
GlyphService *ServiceRegistry::glyphService() const { return m_glyphService.get(); }
FontService *ServiceRegistry::fontService() const { return m_fontService.get(); }
LocalStorageService *ServiceRegistry::localStorage() const { return m_localStorage.get(); }
ExtensionManager *ServiceRegistry::extensionManager() const { return m_extensionManager.get(); }
ClipboardService *ServiceRegistry::clipman() const { return m_clipman.get(); }
AppService *ServiceRegistry::appDb() const { return m_appDb.get(); }
ToastService *ServiceRegistry::toastService() const { return m_toastService.get(); }
ShortcutService *ServiceRegistry::shortcuts() const { return m_shortcutService.get(); }
FileService *ServiceRegistry::fileService() const { return m_fileService.get(); }
RaycastStoreService *ServiceRegistry::raycastStore() const { return m_raycastStoreService.get(); }
VicinaeStoreService *ServiceRegistry::vicinaeStore() const { return m_vicinaeStoreService.get(); }
ExtensionRegistry *ServiceRegistry::extensionRegistry() const { return m_extensionRegistry.get(); }
OAuthService *ServiceRegistry::oauthService() const { return m_oauthService.get(); }

PowerManager *ServiceRegistry::powerManager() const { return m_powerManager.get(); }

ScriptCommandService *ServiceRegistry::scriptDb() const { return m_scriptCommandService.get(); }

BrowserExtensionService *ServiceRegistry::browserExtension() const { return m_browserExtensionService.get(); }

#ifdef Q_OS_LINUX
LinuxInputServer *ServiceRegistry::inputServer() const { return m_inputServer.get(); }
#endif

SnippetService *ServiceRegistry::snippetService() const { return m_snippetService.get(); }

PasteService *ServiceRegistry::pasteService() const { return m_pasteService.get(); }

FileChooserService *ServiceRegistry::fileChooserService() const { return m_fileChooserService.get(); }

NewsService *ServiceRegistry::newsService() const { return m_newsService.get(); }

BackgroundEffectManager *ServiceRegistry::backgroundEffectManager() const {
  return m_backgroundEffectManager.get();
}

ShortcutInhibitManager *ServiceRegistry::shortcutInhibitManager() const {
  return m_shortcutInhibitManager.get();
}

TelemetryService *ServiceRegistry::telemetry() const { return m_telemetry.get(); }
AI::Service *ServiceRegistry::ai() const { return m_ai.get(); }

void ServiceRegistry::setAI(std::unique_ptr<AI::Service> service) { m_ai = std::move(service); }

AudioControlService *ServiceRegistry::audioControl() const { return m_audioControl.get(); }

AppRuntime *ServiceRegistry::appRuntime() const { return m_appRuntime.get(); }

void ServiceRegistry::setPowerManager(std::unique_ptr<PowerManager> powman) {
  m_powerManager = std::move(powman);
}

void ServiceRegistry::setWindowManager(std::unique_ptr<WindowManager> manager) {
  m_windowManager = std::move(manager);
}

void ServiceRegistry::ServiceRegistry::setRootItemManager(std::unique_ptr<RootItemManager> manager) {
  m_rootItemManager = std::move(manager);
}
void ServiceRegistry::ServiceRegistry::setRaycastStore(std::unique_ptr<RaycastStoreService> service) {
  m_raycastStoreService = std::move(service);
}
void ServiceRegistry::ServiceRegistry::setVicinaeStore(std::unique_ptr<VicinaeStoreService> service) {
  m_vicinaeStoreService = std::move(service);
}
void ServiceRegistry::ServiceRegistry::setOAuthService(std::unique_ptr<OAuthService> service) {
  m_oauthService = std::move(service);
}

void ServiceRegistry::setConfig(std::unique_ptr<config::Manager> cfg) { m_config = std::move(cfg); }
void ServiceRegistry::setShortcutService(std::unique_ptr<ShortcutService> service) {
  m_shortcutService = std::move(service);
}
void ServiceRegistry::ServiceRegistry::setCalculatorService(std::unique_ptr<CalculatorService> service) {
  m_calculatorService = std::move(service);
}
void ServiceRegistry::ServiceRegistry::setExtensionRegistry(std::unique_ptr<ExtensionRegistry> service) {
  m_extensionRegistry = std::move(service);
}
void ServiceRegistry::ServiceRegistry::setFileService(std::unique_ptr<FileService> service) {
  m_fileService = std::move(service);
}
void ServiceRegistry::setGlyphService(std::unique_ptr<GlyphService> service) {
  m_glyphService = std::move(service);
}
void ServiceRegistry::setToastService(std::unique_ptr<ToastService> service) {
  m_toastService = std::move(service);
}
void ServiceRegistry::setFontService(std::unique_ptr<FontService> font) { m_fontService = std::move(font); }
void ServiceRegistry::setOmniDb(std::unique_ptr<OmniDatabase> service) { m_omniDb = std::move(service); }

void ServiceRegistry::setLocalStorage(std::unique_ptr<LocalStorageService> service) {
  m_localStorage = std::move(service);
}
void ServiceRegistry::setExtensionManager(std::unique_ptr<ExtensionManager> service) {
  m_extensionManager = std::move(service);
}
void ServiceRegistry::setClipman(std::unique_ptr<ClipboardService> service) {
  m_clipman = std::move(service);
}
void ServiceRegistry::setAppDb(std::unique_ptr<AppService> service) { m_appDb = std::move(service); }

void ServiceRegistry::setScriptDb(std::unique_ptr<ScriptCommandService> service) {
  m_scriptCommandService = std::move(service);
}

void ServiceRegistry::setBrowserExtension(std::unique_ptr<BrowserExtensionService> service) {
  m_browserExtensionService = std::move(service);
}

#ifdef Q_OS_LINUX
void ServiceRegistry::setInputServer(std::unique_ptr<LinuxInputServer> server) {
  m_inputServer = std::move(server);
}
#endif

void ServiceRegistry::setSnippetServerBackend(std::unique_ptr<AbstractSnippetServer> backend) {
  m_snippetServerBackend = std::move(backend);
}

void ServiceRegistry::setSnippetService(std::unique_ptr<SnippetService> service) {
  m_snippetService = std::move(service);
}

void ServiceRegistry::setPasteService(std::unique_ptr<PasteService> service) {
  m_pasteService = std::move(service);
}

void ServiceRegistry::setFileChooserService(std::unique_ptr<FileChooserService> service) {
  m_fileChooserService = std::move(service);
}

void ServiceRegistry::setNewsService(std::unique_ptr<NewsService> service) {
  m_newsService = std::move(service);
}

void ServiceRegistry::setBackgroundEffectManager(std::unique_ptr<BackgroundEffectManager> service) {
  m_backgroundEffectManager = std::move(service);
}

void ServiceRegistry::setShortcutInhibitManager(std::unique_ptr<ShortcutInhibitManager> service) {
  m_shortcutInhibitManager = std::move(service);
}

void ServiceRegistry::setTelemetry(std::unique_ptr<TelemetryService> telemetry) {
  m_telemetry = std::move(telemetry);
}

void ServiceRegistry::setAudioControl(std::unique_ptr<AudioControlService> service) {
  m_audioControl = std::move(service);
}

void ServiceRegistry::setAppRuntime(std::unique_ptr<AppRuntime> service) {
  m_appRuntime = std::move(service);
}

GlobalShortcutService *ServiceRegistry::globalShortcuts() const { return m_globalShortcuts.get(); }

void ServiceRegistry::setGlobalShortcuts(std::unique_ptr<GlobalShortcutService> service) {
  m_globalShortcuts = std::move(service);
}

ServiceRegistry *ServiceRegistry::instance() {
  static ServiceRegistry instance;
  return &instance;
}
