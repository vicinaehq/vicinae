#pragma once
#include <QObject>
#include <algorithm>
#include <cstdint>
#include <glaze/core/reflect.hpp>
#include <ranges>
#include "vicinae-ipc/ipc.hpp"

class BrowserExtensionService : public QObject {
  Q_OBJECT

public:
  enum class TabAction : std::uint8_t { Focus, Close };

signals:
  void tabActionRequested(std::string browserId, int id, TabAction action) const;
  void tabsChanged() const;
  void browsersChanged() const;

public:
  struct BrowserTab : ipc::BrowserTabInfo {
    std::string browserId;
  };

  struct BrowserInfo {
    std::string id;
    std::string name;
    std::string engine;
    std::vector<BrowserTab> tabs;
  };

  BrowserExtensionService() = default;

  std::vector<BrowserTab> tabs() const {
    return m_browsers | std::views::transform([](auto &&b) { return b.tabs; }) | std::views::join |
           std::ranges::to<std::vector>();
  }

  auto findById(const std::string &id) {
    return std::ranges::find_if(m_browsers, [&](auto &&browser) { return browser.id == id; });
  }

  void setTabs(std::string id, std::vector<ipc::BrowserTabInfo> tabs) {
    if (auto it = findById(id); it != m_browsers.end()) {
      it->tabs = tabs | std::views::transform([&](const ipc::BrowserTabInfo &info) {
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
      emit browsersChanged();
    }
  }

private:
  std::vector<BrowserInfo> m_browsers;
};
