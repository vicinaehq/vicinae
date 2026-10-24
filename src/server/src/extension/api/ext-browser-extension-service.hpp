#pragma once
#include "generated/tsapi.hpp"
#include "services/browser-extension-service.hpp"

class ExtBrowserExtensionService : public tsapi::AbstractBrowserExtension {
  using Void = tsapi::Result<void>;

public:
  ExtBrowserExtensionService(tsapi::RpcTransport &transport, BrowserExtensionService &browser)
      : AbstractBrowserExtension(transport), m_browser(browser) {}

  tsapi::Result<std::vector<tsapi::BrowserTab>>::Future getTabs() override {
    auto tabs = m_browser.tabs();
    std::vector<tsapi::BrowserTab> transformed;

    transformed.reserve(tabs.size());

    for (auto &tab : tabs) {
      transformed.emplace_back(tsapi::BrowserTab{
          .id = tab.id,
          .title = std::move(tab.title),
          .url = std::move(tab.url),
          .active = tab.active,
          .browserId = std::move(tab.browserId),
          .lastAccessed = tab.lastAccessed,
          .pinned = tab.pinned,
          .audible = tab.audible,
          .muted = tab.muted,
      });
    }

    return tsapi::Result<std::vector<tsapi::BrowserTab>>::ok(transformed);
  }

  tsapi::Result<void>::Future focusTab(std::string browserId, int32_t tabId) override {
    m_browser.focusTab(browserId, tabId);
    return Void::ok();
  }

  tsapi::Result<void>::Future closeTab(std::string browserId, int32_t tabId) override {
    auto result = m_browser.closeTab(browserId, tabId);
    return result ? Void::ok() : Void::fail(result.error());
  }

  tsapi::Result<void>::Future setPinned(std::string browserId, int32_t tabId, bool pinned) override {
    auto result = m_browser.setPinned(browserId, tabId, pinned);
    return result ? Void::ok() : Void::fail(result.error());
  }

  tsapi::Result<void>::Future setMuted(std::string browserId, int32_t tabId, bool muted) override {
    auto result = m_browser.setMuted(browserId, tabId, muted);
    return result ? Void::ok() : Void::fail(result.error());
  }

  tsapi::Result<void>::Future bookmarkTab(std::string browserId, int32_t tabId) override {
    auto result = m_browser.bookmarkTab(browserId, tabId);
    return result ? Void::ok() : Void::fail(result.error());
  }

  tsapi::Result<void>::Future duplicateTab(std::string browserId, int32_t tabId) override {
    auto result = m_browser.duplicateTab(browserId, tabId);
    return result ? Void::ok() : Void::fail(result.error());
  }

private:
  BrowserExtensionService &m_browser;
};
