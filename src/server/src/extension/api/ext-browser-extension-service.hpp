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
      });
    }

    return tsapi::Result<std::vector<tsapi::BrowserTab>>::ok(transformed);
  }

  tsapi::Result<void>::Future focusTab(std::string browserId, int32_t tabId) override {
    m_browser.focusTab(browserId, tabId);
    return Void::ok();
  }

private:
  BrowserExtensionService &m_browser;
};
