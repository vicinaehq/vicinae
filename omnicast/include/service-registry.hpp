#pragma once
#include "ai/ai-service.hpp"
#include "app-service.hpp"
#include "app/app-database.hpp"
#include "calculator-database.hpp"
#include "clipboard/clipboard-service.hpp"
#include "extension_manager.hpp"
#include "font-service.hpp"
#include "local-storage-service.hpp"
#include "omni-command-db.hpp"
#include "omni-database.hpp"
#include "quicklist-database.hpp"
#include "ranking-service.hpp"
#include "wm/window-manager.hpp"
#include <memory>
#include <qobject.h>

class ServiceRegistry : public QObject {
  std::unique_ptr<QuicklistDatabase> m_quickinkDb;
  std::unique_ptr<AbstractWindowManager> m_windowManager;
  std::unique_ptr<CalculatorDatabase> m_calculatorDb;
  std::unique_ptr<AppService> m_appDb;
  std::unique_ptr<OmniDatabase> m_omniDb;
  std::unique_ptr<OmniCommandDatabase> m_omniCommandDb;
  std::unique_ptr<LocalStorageService> m_localStorage;
  std::unique_ptr<ExtensionManager> m_extensionManager;
  std::unique_ptr<ClipboardService> m_clipman;
  std::unique_ptr<AI::Manager> m_aiManager;
  std::unique_ptr<FontService> m_fontService;
  std::unique_ptr<RankingService> m_rankingService;

public:
  static ServiceRegistry *instance() {
    static ServiceRegistry instance;
    return &instance;
  }

  auto AI() const { return m_aiManager.get(); }
  auto omniDb() const { return m_omniDb.get(); }
  auto quicklinks() const { return m_quickinkDb.get(); }
  auto windowManager() const { return m_windowManager.get(); }
  auto fontService() const { return m_fontService.get(); }
  auto calculatorDb() const { return m_calculatorDb.get(); }
  auto commandDb() const { return m_omniCommandDb.get(); }
  auto localStorage() const { return m_localStorage.get(); }
  auto extensionManager() const { return m_extensionManager.get(); }
  auto clipman() const { return m_clipman.get(); }
  auto appDb() const { return m_appDb.get(); }
  auto rankingService() const { return m_rankingService.get(); }

  void setAI(std::unique_ptr<AI::Manager> manager) { m_aiManager = std::move(manager); }
  void setRankingService(std::unique_ptr<RankingService> service) { m_rankingService = std::move(service); }
  void setFontService(std::unique_ptr<FontService> font) { m_fontService = std::move(font); }
  void setOmniDb(std::unique_ptr<OmniDatabase> service) { m_omniDb = std::move(service); }
  void setQuicklinks(std::unique_ptr<QuicklistDatabase> service) { m_quickinkDb = std::move(service); }
  void setWindowManager(std::unique_ptr<AbstractWindowManager> service) {
    m_windowManager = std::move(service);
  }
  void setCalculatorDb(std::unique_ptr<CalculatorDatabase> service) { m_calculatorDb = std::move(service); }
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
