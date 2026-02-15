#pragma once
#include "actions/browser-tab-actions.hpp"
#include "builtin_icon.hpp"
#include "common.hpp"
#include "navigation-controller.hpp"
#include "services/browser-extension-service.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "ui/default-list-item-widget/default-list-item-widget.hpp"
#include <qjsonobject.h>
#include <qstringliteral.h>
#include <qwidget.h>
#include <ranges>

class BrowserTabRootItem : public RootItem {
  double baseScoreWeight() const override { return 1.1; }

  QString typeDisplayName() const override { return "Browser Tab"; }

  QString displayName() const override { return m_tab.title.c_str(); }

  QString subtitle() const override { return ""; }

  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx,
                                                   const RootItemMetadata &metadata) const override {
    return BrowserTabActionGenerator::generate(ctx, m_tab);
  }

  AccessoryList accessories() const override {
    ListAccessory accessory{.text = "Tab"};

    if (m_tab.audible) { accessory.icon = ImageURL::emoji(m_tab.muted ? "🔇" : "🔊"); }

    return {accessory};
  }

  EntrypointId uniqueId() const override { return EntrypointId("browser-tabs", m_tab.uniqueId()); }

  ImageURL iconUrl() const override { return m_tab.icon(); }

  QWidget *settingsDetail(const QJsonObject &preferences) const override { return nullptr; }

  std::vector<QString> keywords() const override { return {m_tab.url.c_str()}; }

  bool isActive() const override { return m_tab.active; }

public:
  BrowserTabRootItem(const BrowserExtensionService::BrowserTab &tab) : m_tab(tab) {}

private:
  BrowserExtensionService::BrowserTab m_tab;
};

class BrowserTabProvider : public RootProvider {
public:
  std::vector<std::shared_ptr<RootItem>> loadItems() const override {
    auto items = m_service.tabs() |
                 std::views::transform(
                     [](const BrowserExtensionService::BrowserTab &tab) -> std::shared_ptr<RootItem> {
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
