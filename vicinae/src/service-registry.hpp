#pragma once

#include <memory>
#include <qobject.h>

class AbstractWindowManager;
class AppService;
class OmniDatabase;
class OmniCommandDatabase;
class LocalStorageService;
class ExtensionManager;
class ClipboardService;
class FontService;
class RootItemManager;
class RootExtensionManager;
class ConfigService;
class BookmarkService;
class ToastService;
class EmojiService;
class CalculatorService;
class FileService;
class RaycastStoreService;
class ExtensionRegistry;
class OAuthService;

class ServiceRegistry : public QObject {
  std::unique_ptr<AbstractWindowManager> m_windowManager;
  std::unique_ptr<AppService> m_appDb;
  std::unique_ptr<OmniDatabase> m_omniDb;
  std::unique_ptr<OmniCommandDatabase> m_omniCommandDb;
  std::unique_ptr<LocalStorageService> m_localStorage;
  std::unique_ptr<ExtensionManager> m_extensionManager;
  std::unique_ptr<ClipboardService> m_clipman;
  std::unique_ptr<FontService> m_fontService;
  std::unique_ptr<RootItemManager> m_rootItemManager;
  std::unique_ptr<RootExtensionManager> m_rootExtMan;
  std::unique_ptr<ConfigService> m_config;
  std::unique_ptr<BookmarkService> m_bookmarkService;
  std::unique_ptr<ToastService> m_toastService;
  std::unique_ptr<EmojiService> m_emojiService;
  std::unique_ptr<CalculatorService> m_calculatorService;
  std::unique_ptr<FileService> m_fileService;
  std::unique_ptr<RaycastStoreService> m_raycastStoreService;
  std::unique_ptr<ExtensionRegistry> m_extensionRegistry;
  std::unique_ptr<OAuthService> m_oauthService;

public:
  static ServiceRegistry *instance();
  RootItemManager *rootItemManager() const;
  ConfigService *config() const;
  OmniDatabase *omniDb() const;
  CalculatorService *calculatorService() const;
  AbstractWindowManager *windowManager() const;
  EmojiService *emojiService() const;
  FontService *fontService() const;
  OmniCommandDatabase *commandDb() const;
  LocalStorageService *localStorage() const;
  ExtensionManager *extensionManager() const;
  ClipboardService *clipman() const;
  AppService *appDb() const;
  ToastService *toastService() const;
  BookmarkService *bookmarks() const;
  FileService *fileService() const;
  RaycastStoreService *raycastStore() const;
  ExtensionRegistry *extensionRegistry() const;
  OAuthService *oauthService() const;

  void setRootItemManager(std::unique_ptr<RootItemManager> manager);
  void setRaycastStore(std::unique_ptr<RaycastStoreService> service);
  void setOAuthService(std::unique_ptr<OAuthService> service);
  void setConfig(std::unique_ptr<ConfigService> cfg);
  void setBookmarkService(std::unique_ptr<BookmarkService> service);
  void setCalculatorService(std::unique_ptr<CalculatorService> service);
  void setExtensionRegistry(std::unique_ptr<ExtensionRegistry> service);
  void setFileService(std::unique_ptr<FileService> service);
  void setEmojiService(std::unique_ptr<EmojiService> service);
  void setToastService(std::unique_ptr<ToastService> service);
  void setRootExtMan(std::unique_ptr<RootExtensionManager> man);
  void setFontService(std::unique_ptr<FontService> font);
  void setOmniDb(std::unique_ptr<OmniDatabase> service);
  void setWindowManager(std::unique_ptr<AbstractWindowManager> service);
  void setCommandDb(std::unique_ptr<OmniCommandDatabase> commandDb);
  void setLocalStorage(std::unique_ptr<LocalStorageService> service);
  void setExtensionManager(std::unique_ptr<ExtensionManager> service);
  void setClipman(std::unique_ptr<ClipboardService> service);
  void setAppDb(std::unique_ptr<AppService> service);
};
