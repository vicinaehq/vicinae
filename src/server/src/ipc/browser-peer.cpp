#include "ipc/browser-peer.hpp"
#include "ipc/ipc-command-server.hpp"

AbstractBrowserPeer::Future<void> IpcBrowserPeer::focusTab(int tabId) {
  return m_ipc.callbrowserFocusTab(tabId);
}

AbstractBrowserPeer::Future<void> IpcBrowserPeer::closeTab(int tabId) {
  return m_ipc.callbrowserCloseTab(tabId);
}

AbstractBrowserPeer::Future<std::vector<ipc_gen::BrowserBookmarkInfo>> IpcBrowserPeer::bookmarks() {
  return m_ipc.callbrowserGetBookmarks(FETCH_TIMEOUT);
}

AbstractBrowserPeer::Future<void> IpcBrowserPeer::removeBookmark(const std::string &bookmarkId) {
  return m_ipc.callbrowserRemoveBookmark(bookmarkId);
}

AbstractBrowserPeer::Future<std::vector<ipc_gen::BrowserHistoryItemInfo>>
IpcBrowserPeer::searchHistory(const std::string &query, int maxResults) {
  return m_ipc.callbrowserSearchHistory(query, maxResults, FETCH_TIMEOUT);
}

AbstractBrowserPeer::Future<void> IpcBrowserPeer::removeHistoryUrl(const std::string &url) {
  return m_ipc.callbrowserRemoveHistoryUrl(url);
}

AbstractBrowserPeer::Future<void> IpcBrowserPeer::clearHistory() {
  return m_ipc.callbrowserClearHistory(CLEAR_HISTORY_TIMEOUT);
}

AbstractBrowserPeer::Future<void> IpcBrowserPeer::requestPermission(const std::string &permission) {
  return m_ipc.callbrowserRequestPermission(permission);
}
