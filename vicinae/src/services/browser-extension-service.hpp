#pragma once
#include <QObject>
#include <algorithm>
#include <ranges>
#include "vicinae-ipc/ipc.hpp"

class BrowserExtensionService : public QObject {
  Q_OBJECT

signals:
  void tabFocusRequested(int id) const;
  void tabsChanged() const;

public:
  struct BrowserTab : public ipc::BrowserTabInfo {
    std::string browserId;
  };

  struct BrowserInfo {
    std::string id;
    std::string name;
    std::vector<ipc::BrowserTabInfo> tabs;
  };

  BrowserExtensionService() = default;

  std::vector<ipc::BrowserTabInfo> tabs() const {
    return m_browsers | std::views::transform([](auto &&b) { return b.tabs; }) | std::views::join |
           std::ranges::to<std::vector>();
  }

  auto findById(const std::string &id) {
    return std::ranges::find_if(m_browsers, [&](auto &&browser) { return browser.id == id; });
  }

  void setTabs(std::string id, std::vector<ipc::BrowserTabInfo> tabs) {
    if (auto it = findById(id); it != m_browsers.end()) {
      it->tabs = tabs;
      emit tabsChanged();
    }
  }

  void focusTab(int id) { emit tabFocusRequested(id); }

  void registerBrowser(const BrowserInfo &info) {
    if (auto it = findById(info.id); it != m_browsers.end()) {
      *it = info;
    } else {
      m_browsers.emplace_back(info);
    }
  }

  void unregisterBrowser(std::string id) {
    if (auto it = findById(id); it != m_browsers.end()) {
      m_browsers.erase(it);
      emit tabsChanged();
    }
  }

private:
  std::vector<ipc::BrowserTabInfo> m_tabs;
  std::vector<BrowserInfo> m_browsers;
};
