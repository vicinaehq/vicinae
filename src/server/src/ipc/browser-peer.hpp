#pragma once
#include <chrono>
#include <string>
#include <vector>
#include "generated/ipc-server.hpp"
#include "services/browser-extension-service.hpp"

class IpcService;

/**
 * Browser extension connected to the IPC server: every action is a call the extension replies to.
 * Owned by the browser extension service, which drops it before the connection goes away.
 */
class IpcBrowserPeer : public AbstractBrowserPeer {
public:
  explicit IpcBrowserPeer(IpcService &ipc) : m_ipc(ipc) {}

  Future<void> focusTab(int tabId) override;
  Future<void> closeTab(int tabId) override;
  Future<std::vector<ipc_gen::BrowserBookmarkInfo>> bookmarks() override;
  Future<void> removeBookmark(const std::string &bookmarkId) override;
  Future<std::vector<ipc_gen::BrowserHistoryItemInfo>> searchHistory(const std::string &query,
                                                                     int maxResults) override;
  Future<void> removeHistoryUrl(const std::string &url) override;
  Future<void> clearHistory() override;
  Future<void> requestPermission(const std::string &permission) override;

private:
  // large bookmark trees and histories can take a while to go through
  static constexpr std::chrono::seconds FETCH_TIMEOUT{15};
  static constexpr std::chrono::seconds CLEAR_HISTORY_TIMEOUT{60};

  IpcService &m_ipc;
};
