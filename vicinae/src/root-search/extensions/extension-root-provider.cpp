#include "root-search/extensions/extension-root-provider.hpp"
#include "actions/extension/extension-actions.hpp"
#include "actions/fallback-actions.hpp"
#include "actions/root-search/root-search-actions.hpp"
#include "clipboard-actions.hpp"
#include "command-actions.hpp"
#include "common.hpp"
#include "extension/extension-command.hpp"
#include "navigation-controller.hpp"
#include "services/root-item-manager/root-item-manager.hpp"

QString CommandRootItem::displayName() const { return m_command->name(); }

QString CommandRootItem::subtitle() const {
  // Display overriden subtitle if set
  if (auto ext = std::dynamic_pointer_cast<ExtensionCommand>(m_command)) {
    if (ext->overriddenSubtitle() && !ext->overriddenSubtitle()->isEmpty()) {
      return *ext->overriddenSubtitle();
    }
  }
  return m_command->repositoryDisplayName();
}

ImageURL CommandRootItem::iconUrl() const { return m_command->iconUrl(); }
ArgumentList CommandRootItem::arguments() const { return m_command->arguments(); }
bool CommandRootItem::isSuitableForFallback() const { return m_command->isFallback(); }
double CommandRootItem::baseScoreWeight() const { return 1.1; }
QString CommandRootItem::typeDisplayName() const { return "Command"; }

std::unique_ptr<ActionPanelState> CommandRootItem::newActionPanel(ApplicationContext *ctx,
                                                                  const RootItemMetadata &metadata) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto open = new OpenBuiltinCommandAction(m_command, "Open command");
  auto mainSection = panel->createSection();
  auto itemSection = panel->createSection();
  auto extensionSection = panel->createSection();
  auto dangerSection = panel->createSection();
  auto copyDeeplink = new CopyToClipboardAction(Clipboard::Text(m_command->deeplink()), "Copy deeplink");

  mainSection->addAction(new DefaultActionWrapper(uniqueId(), open));
  itemSection->addAction(copyDeeplink);

  for (const auto action : RootSearchActionGenerator::generateActions(*this, metadata)) {
    itemSection->addAction(action);
  }

  if (m_command->type() == CommandType::CommandTypeExtension) {
    auto cmd = static_cast<ExtensionCommand *>(m_command.get());
    auto copyLocation =
        new CopyToClipboardAction(Clipboard::Text(cmd->path().c_str()), "Copy extension path");

    extensionSection->addAction(copyLocation);
    dangerSection->addAction(new UninstallExtensionAction(cmd->extensionId()));
  }

  return panel;
}

std::unique_ptr<ActionPanelState>
CommandRootItem::fallbackActionPanel(ApplicationContext *ctx, const RootItemMetadata &metadata) const {
  auto panel = std::make_unique<ActionPanelState>();
  auto main = panel->createSection();
  auto open = new OpenBuiltinCommandAction(m_command, "Open command", "");
  auto manage = new ManageFallbackActions;

  open->setForwardSearchText(true);
  manage->setShortcut(Keyboard::Shortcut::submit());

  main->addAction(open);
  main->addAction(manage);

  return panel;
}

EntrypointId CommandRootItem::uniqueId() const { return m_command->uniqueId(); }

AccessoryList CommandRootItem::accessories() const {
  if (m_command->isInternal()) { return {{.text = "Internal Command", .color = SemanticColor::Blue}}; }
  return {{.text = "Command", .color = SemanticColor::TextMuted}};
}

std::vector<std::shared_ptr<RootItem>> ExtensionRootProvider::loadItems() const {
  std::vector<std::shared_ptr<RootItem>> items;

  for (const auto &cmd : m_repo->commands()) {
    items.emplace_back(std::make_shared<CommandRootItem>(cmd));
  }

  return items;
}
