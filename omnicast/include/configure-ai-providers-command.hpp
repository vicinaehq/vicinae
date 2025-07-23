#include "action-panel/action-panel.hpp"
#include "base-view.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include "theme.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/default-list-item-widget.hpp"
#include "ui/omni-list.hpp"
#include "ui/toast.hpp"
#include <qnamespace.h>

using AIProvider = AI::Manager::ManagedProvider;

class ConfigureProviderAction : public AbstractAction {
  const AIProvider &m_info;

public:
  void execute() override {
    auto ui = ServiceRegistry::instance()->UI();
    if (auto view = m_info.provider->configView()) {
      auto navTitle = QString("Configure %1 AI provider").arg(m_info.provider->displayName());

      ui->pushView(view);
    } else {
      ui->setToast("This provider has no config view! This should not be the case.", ToastPriority::Danger);
    }
  }

  ConfigureProviderAction(const AIProvider &info)
      : AbstractAction("Configure", BuiltinOmniIconUrl("cog")), m_info(info) {}
};

class ToggleAIProviderAction : public AbstractAction {
  const AIProvider &m_info;

  QString getActionText() { return m_info.enabled ? "Disable provider" : "Enable provider"; }

public:
  void execute() override {
    auto ui = ServiceRegistry::instance()->UI();
    auto aiManager = ServiceRegistry::instance()->AI();
    bool success = aiManager->setProviderEnabled(m_info.provider->id(), !m_info.enabled);

    if (success) {
      ui->setToast("Provider status changed");
    } else {
      ui->setToast("Failed to change provider status");
    }
  }

  ToggleAIProviderAction(const AIProvider &info)
      : AbstractAction("Toggle status", BuiltinOmniIconUrl("stars")), m_info(info) {}
};

class AiProviderListItem : public AbstractDefaultListItem, public ListView::Actionnable {
  const AIProvider &m_info;

  AccessoryList generateAccessories() const {
    AccessoryList accessories;

    accessories.emplace_back(ListAccessory{
        .text = m_info.configured ? "Configured" : "Not configured",
        .color = m_info.configured ? SemanticColor::Blue : SemanticColor::Orange,
        .fillBackground = true,
        .icon = BuiltinOmniIconUrl("cog"),
    });
    accessories.emplace_back(ListAccessory{
        .text = m_info.enabled ? "Enabled" : "Disabled",
        .color = m_info.enabled ? SemanticColor::Green : SemanticColor::Red,
        .fillBackground = true,
        .icon = BuiltinOmniIconUrl(m_info.enabled ? "checkmark" : "x-mark-circle"),
    });

    return accessories;
  }

public:
  ActionPanelView *actionPanel() const override {
    auto panel = new ActionPanelStaticListView;
    auto configure = new ConfigureProviderAction(m_info);
    auto toggle = new ToggleAIProviderAction(m_info);

    configure->setPrimary(true);
    configure->setShortcut({.key = "return"});
    toggle->setShortcut({.key = "return", .modifiers = {"shift"}});

    panel->setTitle(m_info.provider->displayName());
    panel->addAction(configure);
    panel->addAction(toggle);

    return panel;
  }

  QString generateId() const override { return m_info.provider->id(); }
  ItemData data() const override {
    return {.iconUrl = BuiltinOmniIconUrl(m_info.provider->iconName()),
            .name = m_info.provider->displayName(),
            .accessories = generateAccessories()};
  }

  AiProviderListItem(const AIProvider &info) : m_info(info) {}
};

class ConfigureAIProvidersView : public ListView {
public:
  void initialize() override { onSearchChanged(""); }

  void onSearchChanged(const QString &s) override {
    auto aiManager = ServiceRegistry::instance()->AI();
    m_list->beginResetModel();

    auto &enabled = m_list->addSection("Enabled");

    for (const auto &provider : aiManager->providers()) {
      if (!(provider.configured && provider.enabled)) continue;
      if (!provider.provider->displayName().contains(s, Qt::CaseInsensitive)) { continue; }

      enabled.addItem(std::make_unique<AiProviderListItem>(provider));
    }

    auto &available = m_list->addSection("Available");

    for (const auto &provider : aiManager->providers()) {
      if (!provider.provider->displayName().contains(s, Qt::CaseInsensitive)) { continue; }
      if (!provider.enabled) { available.addItem(std::make_unique<AiProviderListItem>(provider)); }
    }

    m_list->endResetModel(OmniList::SelectFirst);
  }

  ConfigureAIProvidersView() { setSearchPlaceholderText("Search AI providers..."); }
};
