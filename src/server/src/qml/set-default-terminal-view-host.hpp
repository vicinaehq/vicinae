#pragma once
#include <memory>
#include <ranges>
#include "builtin_icon.hpp"
#include "common/context.hpp"
#include "mono-list-view-host.hpp"
#include "services/app-service/abstract-app-db.hpp"
#include "theme/colors.hpp"
#include "ui/action-pannel/action-panel-state.hpp"
#include "services/app-service/app-service.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/list-accessory/list-accessory.hpp"
#include "xdgpp/xdg-terminal-exec/xdg-terminals-list.hpp"
#include "services/toast/toast-service.hpp"

class SetDefaultTerminalViewHost : public MonoListViewHost<std::shared_ptr<AbstractApplication>> {
signals:
  void defaultEmulatorChanged() const;

public:
  void onMount() override {
    setSearchPlaceholderText("Select a terminal emulator...");
    m_onChanged = [this]() { reloadItems(); };
    reloadItems();
  }

  QString sectionName() const override { return "Available terminal emulators"; }

  QString displayTitle(const ItemType &e) const override { return e->displayName(); }

  QString displaySubtitle(const ItemType &e) const override { return e->description(); }

  std::optional<ImageURL> displayIcon(const ItemType &e) const override { return e->iconUrl(); }

  AccessoryList displayAccessories(const ItemType &e) const override {
    if (auto &t = m_defaultTerminal; t && *t == e->id()) {
      return {
          ListAccessory{.icon = ImageURL::builtin(BuiltinIcon::CheckCircle).setFill(SemanticColor::Green)}};
    }

    return {};
  }

  std::unique_ptr<ActionPanelState> buildActionPanel(const ItemType &e) const override {
    auto panel = std::make_unique<ListActionPanelState>();
    auto main = panel->createSection();
    auto setDefault = new StaticAction(
        "Set as default terminal", ImageURL::symbol("$"), [this, appId = e->id()](ApplicationContext *ctx) {
          if (xdgpp::setDefaultTerminal(appId.toStdString(), {})) {
            m_onChanged();
            ctx->navigation->showHud("Default terminal changed",
                                     ImageURL::symbol("$").setFill(SemanticColor::Green));
            ctx->navigation->popToRoot();
          } else {
            ctx->services->toastService()->failure("Failed to set default terminal");
          }
        });

    main->addAction(setDefault);

    return panel;
  }

private:
  void reloadItems() {
    auto appDb = context()->services->appDb();

    if (auto app = appDb->terminalEmulator()) { m_defaultTerminal = app->id(); }

    auto terminals =
        appDb->list() |
        std::views::filter([](auto &&app) { return app->displayable() && app->isTerminalEmulator(); }) |
        std::ranges::to<std::vector>();
    const auto isDefault = [&](auto &&app) { return m_defaultTerminal && *m_defaultTerminal == app->id(); };

    std::ranges::sort(terminals, [&](auto &&a, auto &&b) { return isDefault(a) > isDefault(b); });

    setItems(std::move(terminals));
    notifyChanged();
  }

  std::optional<QString> m_defaultTerminal;
  std::function<void()> m_onChanged;
};
