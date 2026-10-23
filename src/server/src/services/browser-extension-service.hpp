#pragma once
#include <QObject>
#include <QTimer>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <glaze/core/reflect.hpp>
#include <optional>
#include <span>
#include <ranges>
#include <string_view>
#include "ui/image/url.hpp"
#include "generated/ipc-server.hpp"

class BrowserExtensionService : public QObject {
  Q_OBJECT

public:
  enum class TabAction : std::uint8_t { Focus, Close };

signals:
  void tabActionRequested(std::string browserId, int id, TabAction action) const;
  void bookmarksRequested(std::string browserId, int requestId) const;
  void permissionRequested(std::string browserId, std::string permission) const;
  void bookmarkRemovalRequested(std::string browserId, std::string bookmarkId) const;
  void tabsChanged() const;
  void bookmarksChanged() const;
  void browsersChanged() const;

public:
  static constexpr std::string_view BOOKMARKS_PERMISSION = "bookmarks";
  static constexpr std::chrono::seconds BOOKMARKS_CACHE_TTL{60};

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

  struct BrowserInfo {
    std::string id;
    std::string name;
    std::string engine;
    std::optional<std::string> version;
    std::optional<std::vector<std::string>> permissions;
    std::vector<BrowserTab> tabs;
    std::optional<std::vector<BrowserBookmark>> bookmarks;
    std::optional<int> pendingBookmarksRequest;

    bool supportsPermissions() const { return permissions.has_value(); }
    bool hasPermission(std::string_view permission) const {
      return permissions && std::ranges::contains(*permissions, permission);
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

  void focusTab(const std::string &browserId, int tabId) {
    emit tabActionRequested(browserId, tabId, TabAction::Focus);
  }

  void focusTab(const BrowserTab &tab) { focusTab(tab.browserId, tab.id); }

  std::expected<void, std::string> closeTab(const std::string &browserId, int tabId) {
    // remove it immediately, don't wait for the next tab change event so that root search
    // is updated immediately.

    auto browserIt = std::ranges::find_if(m_browsers, [&](auto &&info) { return info.id == browserId; });

    if (browserIt == m_browsers.end()) { return std::unexpected("No such browser"); }

    auto tabIt = std::ranges::find_if(browserIt->tabs, [&](auto &&tab) { return tab.id == tabId; });

    if (tabIt == browserIt->tabs.end()) { return std::unexpected("No such tab"); }

    browserIt->tabs.erase(tabIt);

    emit tabsChanged();
    emit tabActionRequested(browserId, tabId, TabAction::Close);

    return {};
  }

  auto closeTab(const BrowserTab &tab) { return closeTab(tab.browserId, tab.id); }

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

  void registerBrowser(const BrowserInfo &info) {
    if (auto it = findById(info.id); it != m_browsers.end()) {
      *it = info;
    } else {
      m_browsers.emplace_back(info);
    }
    emit browsersChanged();
  }

  void unregisterBrowser(std::string id) {
    if (auto it = findById(id); it != m_browsers.end()) {
      m_browsers.erase(it);
      emit tabsChanged();
      emit bookmarksChanged();
      emit browsersChanged();
    }
  }

  std::span<const BrowserInfo> browsers() const { return m_browsers; }

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

      const int requestId = m_nextBookmarksRequestId++;

      browser.pendingBookmarksRequest = requestId;
      emit bookmarksRequested(browser.id, requestId);
    }
  }

  bool isFetchingBookmarks() const {
    return std::ranges::any_of(m_browsers, [](auto &&b) { return b.pendingBookmarksRequest.has_value(); });
  }

  std::expected<void, std::string> setBookmarks(const std::string &browserId,
                                                ipc_gen::BookmarksResponse response) {
    auto it = findById(browserId);

    if (it == m_browsers.end()) return std::unexpected("No such browser");
    if (it->pendingBookmarksRequest != response.requestId)
      return std::unexpected("Unexpected bookmarks response");

    it->pendingBookmarksRequest.reset();

    if (response.error) {
      emit bookmarksChanged();
      return std::unexpected(*response.error);
    }

    it->bookmarks = response.bookmarks | std::views::transform([&](ipc_gen::BrowserBookmarkInfo &info) {
                      auto bookmark = BrowserBookmark(std::move(info));
                      bookmark.browserId = browserId;
                      return bookmark;
                    }) |
                    std::ranges::to<std::vector>();

    if (!m_bookmarksEviction.isActive()) m_bookmarksEviction.start();

    emit bookmarksChanged();

    return {};
  }

  std::expected<void, std::string> removeBookmark(const BrowserBookmark &bookmark) {
    auto it = findById(bookmark.browserId);

    if (it == m_browsers.end()) return std::unexpected("No such browser");

    // drop it from the cache right away so the list updates without waiting for a refetch
    if (it->bookmarks) {
      std::erase_if(*it->bookmarks, [&](auto &&b) { return b.id == bookmark.id; });
      emit bookmarksChanged();
    }

    emit bookmarkRemovalRequested(bookmark.browserId, bookmark.id);

    return {};
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

    emit browsersChanged();
  }

  /**
   * Ask the browser to show its permission page. Granting it requires user interaction in the browser.
   */
  void requestPermission(const std::string &browserId, std::string_view permission) {
    emit permissionRequested(browserId, std::string{permission});
  }

private:
  std::vector<BrowserInfo> m_browsers;
  QTimer m_bookmarksEviction;
  int m_nextBookmarksRequestId = 0;
};
