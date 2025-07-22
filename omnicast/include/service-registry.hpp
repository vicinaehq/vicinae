#pragma once
#include "ai/ai-service.hpp"
#include "services/app-service/app-service.hpp"
#include "services/bookmark/bookmark-service.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/config/config-service.hpp"
#include "services/calculator-service/calculator-service.hpp"
#include "extension/manager/extension-manager.hpp"
#include "font-service.hpp"
#include "services/emoji-service/emoji-service.hpp"
#include "services/files-service/file-service.hpp"
#include "services/local-storage/local-storage-service.hpp"
#include "omni-command-db.hpp"
#include "omni-database.hpp"
#include "quicklist-database.hpp"
#include "root-extension-manager.hpp"
#include "services/raycast/raycast-store.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/ui-controller.hpp"
#include "wm/window-manager.hpp"
#include <memory>
#include <qobject.h>

class ServiceRegistry : public QObject {

  std::unique_ptr<QuicklistDatabase> m_quickinkDb;
  std::unique_ptr<AbstractWindowManager> m_windowManager;
  std::unique_ptr<AppService> m_appDb;
  std::unique_ptr<OmniDatabase> m_omniDb;
  std::unique_ptr<OmniCommandDatabase> m_omniCommandDb;
  std::unique_ptr<LocalStorageService> m_localStorage;
  std::unique_ptr<ExtensionManager> m_extensionManager;
  std::unique_ptr<ClipboardService> m_clipman;
  std::unique_ptr<AI::Manager> m_aiManager;
  std::unique_ptr<FontService> m_fontService;
  std::unique_ptr<RootItemManager> m_rootItemManager;
  std::unique_ptr<RootExtensionManager> m_rootExtMan;
  std::unique_ptr<ConfigService> m_config;
  std::unique_ptr<BookmarkService> m_bookmarkService;
  std::unique_ptr<UIController> m_uiController;
  std::unique_ptr<ToastService> m_toastService;
  std::unique_ptr<EmojiService> m_emojiService;
  std::unique_ptr<CalculatorService> m_calculatorService;
  std::unique_ptr<FileService> m_fileService;
  std::unique_ptr<RaycastStoreService> m_raycastStoreService;

public:
  static ServiceRegistry *instance() {
    static ServiceRegistry instance;
    return &instance;
  }

  auto rootItemManager() const { return m_rootItemManager.get(); }
  auto config() const { return m_config.get(); }
  auto AI() const { return m_aiManager.get(); }
  auto omniDb() const { return m_omniDb.get(); }
  auto calculatorService() const { return m_calculatorService.get(); }
  auto quicklinks() const { return m_quickinkDb.get(); }
  auto windowManager() const { return m_windowManager.get(); }
  auto emojiService() const { return m_emojiService.get(); }
  auto fontService() const { return m_fontService.get(); }
  auto commandDb() const { return m_omniCommandDb.get(); }
  auto localStorage() const { return m_localStorage.get(); }
  auto extensionManager() const { return m_extensionManager.get(); }
  auto clipman() const { return m_clipman.get(); }
  auto appDb() const { return m_appDb.get(); }
  auto toastService() const { return m_toastService.get(); }
  auto bookmarks() const { return m_bookmarkService.get(); }
  auto UI() const { return m_uiController.get(); }
  auto fileService() const { return m_fileService.get(); }
  auto raycastStore() const { return m_raycastStoreService.get(); }

  auto setUI(std::unique_ptr<UIController> controller) { m_uiController = std::move(controller); }
  auto setRootItemManager(std::unique_ptr<RootItemManager> manager) {
    m_rootItemManager = std::move(manager);
  }
  auto setRaycastStore(std::unique_ptr<RaycastStoreService> service) {
    m_raycastStoreService = std::move(service);
  }
  void setConfig(std::unique_ptr<ConfigService> cfg) { m_config = std::move(cfg); }
  void setBookmarkService(std::unique_ptr<BookmarkService> service) {
    m_bookmarkService = std::move(service);
  }
  auto setCalculatorService(std::unique_ptr<CalculatorService> service) {
    m_calculatorService = std::move(service);
  }
  auto setFileService(std::unique_ptr<FileService> service) { m_fileService = std::move(service); }
  void setEmojiService(std::unique_ptr<EmojiService> service) { m_emojiService = std::move(service); }
  void setToastService(std::unique_ptr<ToastService> service) { m_toastService = std::move(service); }
  void setRootExtMan(std::unique_ptr<RootExtensionManager> man) { m_rootExtMan = std::move(man); }
  void setAI(std::unique_ptr<AI::Manager> manager) { m_aiManager = std::move(manager); }
  void setFontService(std::unique_ptr<FontService> font) { m_fontService = std::move(font); }
  void setOmniDb(std::unique_ptr<OmniDatabase> service) { m_omniDb = std::move(service); }
  void setQuicklinks(std::unique_ptr<QuicklistDatabase> service) { m_quickinkDb = std::move(service); }
  void setWindowManager(std::unique_ptr<AbstractWindowManager> service) {
    m_windowManager = std::move(service);
  }
  void setCommandDb(std::unique_ptr<OmniCommandDatabase> commandDb) {
    m_omniCommandDb = std::move(commandDb);
  }
  void setLocalStorage(std::unique_ptr<LocalStorageService> service) { m_localStorage = std::move(service); }
  void setExtensionManager(std::unique_ptr<ExtensionManager> service) {
    m_extensionManager = std::move(service);
  }
  void setClipman(std::unique_ptr<ClipboardService> service) { m_clipman = std::move(service); }
  void setAppDb(std::unique_ptr<AppService> service) { m_appDb = std::move(service); }
};
