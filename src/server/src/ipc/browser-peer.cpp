#include "ipc/browser-peer.hpp"
#include "ipc/ipc-command-server.hpp"

AbstractBrowserPeer::Future<void> IpcBrowserPeer::focusTab(int tabId) {
  return m_ipc.callbrowserFocusTab(tabId);
}

AbstractBrowserPeer::Future<void> IpcBrowserPeer::closeTab(int tabId) {
  return m_ipc.callbrowserCloseTab(tabId);
}

AbstractBrowserPeer::Future<void> IpcBrowserPeer::setPinned(int tabId, bool pinned) {
  return m_ipc.callbrowserSetPinned({.tabId = tabId, .pinned = pinned});
}

AbstractBrowserPeer::Future<void> IpcBrowserPeer::setMuted(int tabId, bool muted) {
  return m_ipc.callbrowserSetMuted({.tabId = tabId, .muted = muted});
}

AbstractBrowserPeer::Future<void> IpcBrowserPeer::bookmarkTab(const ipc_gen::BookmarkTabRequest &req) {
  return m_ipc.callbrowserBookmarkTab(req);
}

AbstractBrowserPeer::Future<void> IpcBrowserPeer::duplicateTab(int tabId) {
  return m_ipc.callbrowserDuplicateTab({.tabId = tabId});
}

AbstractBrowserPeer::Future<std::vector<ipc_gen::BrowserBookmarkInfo>> IpcBrowserPeer::bookmarks() {
  return m_ipc.callbrowserGetBookmarks(FETCH_TIMEOUT);
}

AbstractBrowserPeer::Future<std::vector<ipc_gen::BrowserBookmarkFolderInfo>> IpcBrowserPeer::bookmarkFolders() {
  return m_ipc.callbrowserGetBookmarkFolders(FETCH_TIMEOUT);
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
