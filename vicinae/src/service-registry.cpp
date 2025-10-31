#include "service-registry.hpp"
#include "extension/manager/extension-manager.hpp"
#include "font-service.hpp"
#include "omni-database.hpp"
#include "services/app-service/app-service.hpp"
#include "services/power-manager/power-manager.hpp"
#include "services/shortcut/shortcut-service.hpp"
#include "services/calculator-service/calculator-service.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/config/config-service.hpp"
#include "services/emoji-service/emoji-service.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include "services/files-service/file-service.hpp"
#include "services/local-storage/local-storage-service.hpp"
#include "services/oauth/oauth-service.hpp"
#include "services/raycast/raycast-store.hpp"
#include "services/extension-store/vicinae-store.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "services/toast/toast-service.hpp"
#include "services/window-manager/window-manager.hpp"

RootItemManager *ServiceRegistry::rootItemManager() const { return m_rootItemManager.get(); }
ConfigService *ServiceRegistry::config() const { return m_config.get(); }
OmniDatabase *ServiceRegistry::omniDb() const { return m_omniDb.get(); }
CalculatorService *ServiceRegistry::calculatorService() const { return m_calculatorService.get(); }
WindowManager *ServiceRegistry::windowManager() const { return m_windowManager.get(); }
EmojiService *ServiceRegistry::emojiService() const { return m_emojiService.get(); }
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

void ServiceRegistry::setConfig(std::unique_ptr<ConfigService> cfg) { m_config = std::move(cfg); }
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
void ServiceRegistry::setEmojiService(std::unique_ptr<EmojiService> service) {
  m_emojiService = std::move(service);
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

ServiceRegistry *ServiceRegistry::instance() {
  static ServiceRegistry instance;
  return &instance;
}
