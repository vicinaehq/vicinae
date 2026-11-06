#pragma once
#include <QDebug>
#include <QObject>
#include <QTimer>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <glaze/core/reflect.hpp>
#include <memory>
#include <optional>
#include <span>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>
#include "ui/image/url.hpp"
#include "generated/ipc-server.hpp"

/**
 * Connection to a single browser extension, through which the browser is asked to do things.
 */
class AbstractBrowserPeer {
public:
  template <typename T> using Future = ipc_gen::Result<T>::Future;

  virtual ~AbstractBrowserPeer() = default;

  virtual Future<void> focusTab(int tabId) = 0;
  virtual Future<void> closeTab(int tabId) = 0;
  virtual Future<std::vector<ipc_gen::BrowserBookmarkInfo>> bookmarks() = 0;
  virtual Future<void> removeBookmark(const std::string &bookmarkId) = 0;
  virtual Future<std::vector<ipc_gen::BrowserHistoryItemInfo>> searchHistory(const std::string &query,
                                                                             int maxResults) = 0;
  virtual Future<void> removeHistoryUrl(const std::string &url) = 0;
  virtual Future<void> clearHistory() = 0;
  // Ask the browser to show its permission page. Granting it requires user interaction in the browser.
  virtual Future<void> requestPermission(const std::string &permission) = 0;
};

class BrowserExtensionService : public QObject {
  Q_OBJECT

public:
  enum class PermissionAccess : std::uint8_t { Granted, NeedsPermission, NeedsUpdate, NoBrowser };
  template <typename T> using Future = ipc_gen::Result<T>::Future;

signals:
  void tabsChanged() const;
  void bookmarksChanged() const;
  void historyChanged() const;
  void browsersChanged() const;

public:
  static constexpr std::string_view BOOKMARKS_PERMISSION = "bookmarks";
  static constexpr std::chrono::seconds BOOKMARKS_CACHE_TTL{60};
  static constexpr std::string_view HISTORY_PERMISSION = "history";
  static constexpr int HISTORY_MAX_RESULTS = 200;
  static constexpr auto OUTDATED_EXTENSION_ERROR = "The browser extension needs to be updated";

  static QString urlHost(const std::string &url) {
    QUrl qurl(url.c_str());
    if (!qurl.isValid()) return QString::fromStdString(url);
    QString h = qurl.host();
    if (h.startsWith("www.")) h.remove(0, 4);
    return h;
  }

  static ImageURL urlIcon(const std::string &url, BuiltinIcon fallback) {
    if (QUrl qurl = QUrl(url.c_str());
        qurl.isValid() && std::ranges::contains(std::initializer_list{"https", "http"}, qurl.scheme())) {
      return ImageURL::favicon(qurl.host()).withFallback(fallback);
    }
    return fallback;
  }

  struct BrowserTab : ipc_gen::BrowserTabInfo {
    std::string browserId;

    std::string uniqueId() const { return std::format("{}-{}", browserId, id); }
    QString host() const { return urlHost(url); }
    ImageURL icon() const { return urlIcon(url, BuiltinIcon::AppWindowSidebarLeft); }
  };

  struct BrowserBookmark : ipc_gen::BrowserBookmarkInfo {
    std::string browserId;

    std::string uniqueId() const { return std::format("{}-{}", browserId, id); }
    QString host() const { return urlHost(url); }
    ImageURL icon() const { return urlIcon(url, BuiltinIcon::Bookmark); }

    bool isInFolder(std::span<const std::string> prefix) const {
      return folder.size() >= prefix.size() &&
             std::ranges::equal(prefix, folder | std::views::take(prefix.size()));
    }

    QString folderPath() const {
      return QString::fromStdString(folder | std::views::join_with(std::string_view{" / "}) |
                                    std::ranges::to<std::string>());
    }
  };

  struct BrowserHistoryItem : ipc_gen::BrowserHistoryItemInfo {
    std::string browserId;

    std::string uniqueId() const { return std::format("{}-{}", browserId, id); }
    QString host() const { return urlHost(url); }
    ImageURL icon() const { return urlIcon(url, BuiltinIcon::Clock); }
  };

  struct BrowserInfo {
    std::string id;
    std::string name;
    std::string engine;
    std::optional<std::string> version;
    std::optional<std::vector<std::string>> permissions;
    std::optional<std::vector<std::string>> optionalPermissions;
    // null when the extension is too old to be talked to
    std::shared_ptr<AbstractBrowserPeer> peer;
    std::vector<BrowserTab> tabs;
    std::optional<std::vector<BrowserBookmark>> bookmarks;
    std::optional<int> pendingBookmarksRequest;
    std::vector<BrowserHistoryItem> history;
    bool historyTruncated = false;
    std::optional<int> pendingHistoryRequest;

    bool isOutdated() const { return !peer; }
    bool canRequestPermission(std::string_view permission) const {
      return !isOutdated() && optionalPermissions && std::ranges::contains(*optionalPermissions, permission);
    }
    bool hasPermission(std::string_view permission) const {
      return !isOutdated() && permissions && std::ranges::contains(*permissions, permission);
    }
  };

  BrowserExtensionService() {
    m_bookmarksEviction.setSingleShot(true);
    m_bookmarksEviction.setInterval(BOOKMARKS_CACHE_TTL);
    connect(&m_bookmarksEviction, &QTimer::timeout, this, [this]() {
      for (auto &browser : m_browsers) {
        browser.bookmarks.reset();
      }
    });
  }

  std::vector<BrowserTab> tabs() const {
    auto tabs = m_browsers | std::views::transform([](auto &&b) { return b.tabs; }) | std::views::join |
                std::ranges::to<std::vector>();

    std::ranges::stable_sort(tabs, std::greater{}, &BrowserTab::lastAccessed);

    return tabs;
  }

  auto findById(const std::string &id) {
    return std::ranges::find_if(m_browsers, [&](auto &&browser) { return browser.id == id; });
  }

  void setTabs(std::string id, std::vector<ipc_gen::BrowserTabInfo> tabs) {
    if (auto it = findById(id); it != m_browsers.end()) {
      it->tabs = tabs | std::views::transform([&](const ipc_gen::BrowserTabInfo &info) {
                   auto tab = BrowserTab(info);
                   tab.browserId = id;
                   return tab;
                 }) |
                 std::ranges::to<std::vector>();
      emit tabsChanged();
    }
  }

  Future<void> focusTab(const std::string &browserId, int tabId) {
    auto it = findById(browserId);

    if (it == m_browsers.end()) return Void::fail("No such browser");
    if (it->isOutdated()) return Void::fail(OUTDATED_EXTENSION_ERROR);

    return it->peer->focusTab(tabId);
  }

  Future<void> focusTab(const BrowserTab &tab) { return focusTab(tab.browserId, tab.id); }

  Future<void> closeTab(const std::string &browserId, int tabId) {
    // remove it immediately, don't wait for the next tab change event so that root search
    // is updated immediately.

    auto browserIt = findById(browserId);

    if (browserIt == m_browsers.end()) return Void::fail("No such browser");
    if (browserIt->isOutdated()) return Void::fail(OUTDATED_EXTENSION_ERROR);

    auto tabIt = std::ranges::find_if(browserIt->tabs, [&](auto &&tab) { return tab.id == tabId; });

    if (tabIt == browserIt->tabs.end()) return Void::fail("No such tab");

    browserIt->tabs.erase(tabIt);

    emit tabsChanged();

    return browserIt->peer->closeTab(tabId);
  }

  Future<void> closeTab(const BrowserTab &tab) { return closeTab(tab.browserId, tab.id); }

  /**
   * Find the first active tab. If many browsers are connected, the active tab from the first connected
   * browser is returned.
   */
  const BrowserTab *findActiveTab() {
    for (const auto &browser : m_browsers) {
      if (auto it = std::ranges::find_if(browser.tabs, [](auto &&tab) { return tab.active; });
          it != browser.tabs.end()) {
        return &*it;
      }
    }
    return nullptr;
  }

  void registerBrowser(BrowserInfo info) {
    if (auto it = findById(info.id); it != m_browsers.end()) {
      // released after the swap: failing its pending calls must not observe a half-replaced entry
      auto previousPeer = std::move(it->peer);
      *it = std::move(info);
    } else {
      m_browsers.emplace_back(std::move(info));
    }
    emit browsersChanged();
  }

  void unregisterBrowser(const std::string &id) {
    if (auto it = findById(id); it != m_browsers.end()) {
      auto peer = std::move(it->peer);
      m_browsers.erase(it);
      emit tabsChanged();
      emit bookmarksChanged();
      emit historyChanged();
      emit browsersChanged();
    }
  }

  std::span<const BrowserInfo> browsers() const { return m_browsers; }

  const BrowserInfo *browser(std::string_view id) const {
    auto it = std::ranges::find_if(m_browsers, [&](auto &&browser) { return browser.id == id; });
    return it == m_browsers.end() ? nullptr : &*it;
  }

  PermissionAccess permissionAccess(std::string_view permission) const {
    if (m_browsers.empty()) return PermissionAccess::NoBrowser;
    if (std::ranges::any_of(m_browsers, [&](auto &&b) { return b.hasPermission(permission); }))
      return PermissionAccess::Granted;
    if (std::ranges::any_of(m_browsers, [&](auto &&b) { return b.canRequestPermission(permission); }))
      return PermissionAccess::NeedsPermission;
    return PermissionAccess::NeedsUpdate;
  }

  std::vector<BrowserBookmark> bookmarks() const {
    return m_browsers | std::views::filter([](auto &&b) { return b.bookmarks.has_value(); }) |
           std::views::transform([](auto &&b) { return *b.bookmarks; }) | std::views::join |
           std::ranges::to<std::vector>();
  }

  /**
   * Request bookmarks from every browser that granted access and has no cached copy.
   * bookmarksChanged is emitted as responses come in.
   */
  void fetchBookmarks() {
    for (auto &browser : m_browsers) {
      if (!browser.hasPermission(BOOKMARKS_PERMISSION)) continue;
      if (browser.bookmarks || browser.pendingBookmarksRequest) continue;

      const int requestId = m_nextRequestId++;

      browser.pendingBookmarksRequest = requestId;
      browser.peer->bookmarks().then(this, [this, browserId = browser.id, requestId](BookmarksResult result) {
        setBookmarks(browserId, requestId, std::move(result));
      });
    }
  }

  bool isFetchingBookmarks() const {
    return std::ranges::any_of(m_browsers, [](auto &&b) { return b.pendingBookmarksRequest.has_value(); });
  }

  Future<void> removeBookmark(const BrowserBookmark &bookmark) {
    auto it = findById(bookmark.browserId);

    if (it == m_browsers.end()) return Void::fail("No such browser");
    if (it->isOutdated()) return Void::fail(OUTDATED_EXTENSION_ERROR);

    // drop it from the cache right away so the list updates without waiting for a refetch
    if (it->bookmarks) {
      std::erase_if(*it->bookmarks, [&](auto &&b) { return b.id == bookmark.id; });
      emit bookmarksChanged();
    }

    return it->peer->removeBookmark(bookmark.id)
        .then(this, [this, browserId = bookmark.browserId](Void::Type result) {
          if (!result) refetchBookmarks(browserId);
          return result;
        });
  }

  void setPermissions(const std::string &browserId, std::vector<std::string> permissions) {
    auto it = findById(browserId);

    if (it == m_browsers.end()) return;

    it->permissions = std::move(permissions);

    if (!it->hasPermission(BOOKMARKS_PERMISSION)) {
      it->bookmarks.reset();
      it->pendingBookmarksRequest.reset();
      emit bookmarksChanged();
    }

    if (!it->hasPermission(HISTORY_PERMISSION)) {
      it->history.clear();
      it->historyTruncated = false;
      it->pendingHistoryRequest.reset();
      emit historyChanged();
    }

    emit browsersChanged();
  }

  std::vector<BrowserHistoryItem> history() const {
    auto items = m_browsers | std::views::transform([](auto &&b) { return b.history; }) | std::views::join |
                 std::ranges::to<std::vector>();

    std::ranges::stable_sort(items, std::greater{}, &BrowserHistoryItem::lastVisitTime);

    return items;
  }

  void searchHistory(const std::string &query) {
    m_historyQuery = query;

    for (auto &browser : m_browsers) {
      if (!browser.hasPermission(HISTORY_PERMISSION)) continue;

      const int requestId = m_nextRequestId++;

      browser.pendingHistoryRequest = requestId;
      browser.peer->searchHistory(query, HISTORY_MAX_RESULTS)
          .then(this, [this, browserId = browser.id, requestId](HistoryResult result) {
            setHistory(browserId, requestId, std::move(result));
          });
    }
  }

  bool isHistoryTruncated() const {
    return std::ranges::any_of(m_browsers, [](auto &&b) { return b.historyTruncated; });
  }

  bool isFetchingHistory() const {
    return std::ranges::any_of(m_browsers, [](auto &&b) { return b.pendingHistoryRequest.has_value(); });
  }

  void clearHistoryResults() {
    m_historyQuery.reset();

    for (auto &browser : m_browsers) {
      browser.history.clear();
      browser.historyTruncated = false;
      browser.pendingHistoryRequest.reset();
    }
  }

  Future<void> removeHistoryUrl(const BrowserHistoryItem &item) {
    auto it = findById(item.browserId);

    if (it == m_browsers.end()) return Void::fail("No such browser");
    if (it->isOutdated()) return Void::fail(OUTDATED_EXTENSION_ERROR);
    if (!it->hasPermission(HISTORY_PERMISSION)) return Void::fail("History access not granted");

    std::erase_if(it->history, [&](auto &&entry) { return entry.url == item.url; });
    emit historyChanged();

    return it->peer->removeHistoryUrl(item.url).then(this, [this](Void::Type result) {
      if (!result) refreshHistory();
      return result;
    });
  }

  Future<void> clearHistory(const std::string &browserId) {
    auto it = findById(browserId);

    if (it == m_browsers.end()) return Void::fail("No such browser");
    if (it->isOutdated()) return Void::fail(OUTDATED_EXTENSION_ERROR);
    if (!it->hasPermission(HISTORY_PERMISSION)) return Void::fail("History access not granted");

    it->history.clear();
    it->historyTruncated = false;
    emit historyChanged();

    return it->peer->clearHistory().then(this, [this](Void::Type result) {
      if (!result) refreshHistory();
      return result;
    });
  }

  Future<void> requestPermission(const std::string &browserId, std::string_view permission) {
    auto it = findById(browserId);

    if (it == m_browsers.end()) return Void::fail("No such browser");
    if (it->isOutdated()) return Void::fail(OUTDATED_EXTENSION_ERROR);

    return it->peer->requestPermission(std::string{permission});
  }

private:
  using Void = ipc_gen::Result<void>;
  using BookmarksResult = ipc_gen::Result<std::vector<ipc_gen::BrowserBookmarkInfo>>::Type;
  using HistoryResult = ipc_gen::Result<std::vector<ipc_gen::BrowserHistoryItemInfo>>::Type;

  void setBookmarks(const std::string &browserId, int requestId, BookmarksResult result) {
    auto it = findById(browserId);

    if (it == m_browsers.end() || it->pendingBookmarksRequest != requestId) return;

    it->pendingBookmarksRequest.reset();

    if (!result) {
      qWarning() << "Failed to load bookmarks from browser" << it->name.c_str() << ":"
                 << result.error().c_str();
      emit bookmarksChanged();
      return;
    }

    it->bookmarks = *result | std::views::transform([&](ipc_gen::BrowserBookmarkInfo &info) {
      auto bookmark = BrowserBookmark(std::move(info));
      bookmark.browserId = browserId;
      return bookmark;
    }) | std::ranges::to<std::vector>();

    if (!m_bookmarksEviction.isActive()) m_bookmarksEviction.start();

    emit bookmarksChanged();
  }

  void refetchBookmarks(const std::string &browserId) {
    if (auto it = findById(browserId); it != m_browsers.end()) {
      it->bookmarks.reset();
      it->pendingBookmarksRequest.reset();
    }

    fetchBookmarks();
    emit bookmarksChanged();
  }

  void setHistory(const std::string &browserId, int requestId, HistoryResult result) {
    auto it = findById(browserId);

    if (it == m_browsers.end() || it->pendingHistoryRequest != requestId) return;

    it->pendingHistoryRequest.reset();

    if (!result) {
      qWarning() << "Failed to load history from browser" << it->name.c_str() << ":"
                 << result.error().c_str();
      emit historyChanged();
      return;
    }

    it->history = *result | std::views::transform([&](ipc_gen::BrowserHistoryItemInfo &info) {
      auto item = BrowserHistoryItem(std::move(info));
      item.browserId = browserId;
      return item;
    }) | std::ranges::to<std::vector>();
    it->historyTruncated = it->history.size() >= HISTORY_MAX_RESULTS;

    emit historyChanged();
  }

  // restore what the browser actually has after an optimistic update failed
  void refreshHistory() {
    if (m_historyQuery) searchHistory(*m_historyQuery);
  }

  std::vector<BrowserInfo> m_browsers;
  QTimer m_bookmarksEviction;
  std::optional<std::string> m_historyQuery;
  int m_nextRequestId = 0;
};
