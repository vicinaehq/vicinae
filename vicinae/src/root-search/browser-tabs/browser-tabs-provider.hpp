#pragma once
#include "builtin_icon.hpp"
#include "common.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/browser-extension-service.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "ui/action-pannel/action.hpp"
#include <qjsonobject.h>
#include <qstringliteral.h>
#include <qwidget.h>
#include <ranges>

class BrowserTabRootItem : public RootItem {
  double baseScoreWeight() const override { return 1.1; }

  QString typeDisplayName() const override { return "Browser Tab"; }

  QString displayName() const override { return m_tab.title.c_str(); }

  QString subtitle() const override { return QUrl(m_tab.url.c_str()).host(); }

  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx,
                                                   const RootItemMetadata &metadata) const override {
    auto panel = std::make_unique<ListActionPanelState>();
    auto section = panel->createSection();

    auto focusTab =
        new StaticAction("Switch to tab", BuiltinIcon::Switch, [id = m_tab.id](ApplicationContext *ctx) {
          ctx->services->browserExtension()->focusTab(id);
          ctx->navigation->closeWindow({.clearRootSearch = true});
        });

    section->addAction(focusTab);

    return panel;
  }

  AccessoryList accessories() const override { return {{.text = "Browser Tab"}}; }

  EntrypointId uniqueId() const override { return EntrypointId("browser-tabs", std::to_string(m_tab.id)); }

  ImageURL iconUrl() const override {
    if (QUrl url = QUrl(m_tab.url.c_str());
        url.isValid() && std::ranges::contains(std::initializer_list{"https", "http"}, url.scheme())) {
      return ImageURL::favicon(url.host()).withFallback(BuiltinIcon::AppWindowSidebarLeft);
    }
    return BuiltinIcon::AppWindowSidebarLeft;
  }

  QWidget *settingsDetail(const QJsonObject &preferences) const override { return nullptr; }

  std::vector<QString> keywords() const override { return {}; }

  bool isActive() const override { return m_tab.active; }

public:
  BrowserTabRootItem(const ipc::BrowserTabInfo &tab) : m_tab(tab) {}

private:
  ipc::BrowserTabInfo m_tab;
};

class BrowserTabProvider : public RootProvider {
public:
  std::vector<std::shared_ptr<RootItem>> loadItems() const override {
    auto items = m_service.tabs() |
                 std::views::transform([](const ipc::BrowserTabInfo &tab) -> std::shared_ptr<RootItem> {
                   return std::make_shared<BrowserTabRootItem>(tab);
                 });

    return items | std::ranges::to<std::vector>();
  }

  Type type() const override { return GroupProvider; }

  ImageURL icon() const override { return BuiltinIcon::AppWindowSidebarLeft; }

  QString displayName() const override { return "Browser Tabs"; }

  bool isTransient() const override { return true; }

  QString uniqueId() const override { return "browser-tabs"; }

  PreferenceList preferences() const override { return {}; }

  void preferencesChanged(const QJsonObject &preferences) override {}

public:
  BrowserTabProvider(BrowserExtensionService &service) : m_service(service) {
    connect(&m_service, &BrowserExtensionService::tabsChanged, this, [this]() { emit itemsChanged(); });
  }

private:
  BrowserExtensionService &m_service;
};
