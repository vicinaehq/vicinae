#include "root-search/ai-commands/ai-command-root-provider.hpp"
#include <utility>
#include "actions/root-search-actions.hpp"
#include "builtins/ai/ai-commands.hpp"
#include "ui/action-panel/action-panel-state.hpp"

RootAiCommandItem::RootAiCommandItem(AiCommandStore::Command command) : m_command(std::move(command)) {}

EntrypointId RootAiCommandItem::uniqueId() const {
  return EntrypointId{AiCommands::PROVIDER_ID, m_command.id};
}

QString RootAiCommandItem::title() const { return QString::fromStdString(m_command.name); }

ImageURL RootAiCommandItem::iconUrl() const { return AiCommands::iconUrl(m_command); }

QString RootAiCommandItem::typeDisplayName() const { return tr("AI Command"); }

AccessoryList RootAiCommandItem::accessories() const {
  return {{.text = tr("AI Command"), .color = SemanticColor::TextMuted}};
}

ArgumentList RootAiCommandItem::arguments() const {
  return AiCommands::arguments(AiCommands::parsePrompt(m_command));
}

std::unique_ptr<ActionPanelState> RootAiCommandItem::newActionPanel(ApplicationContext *ctx,
                                                                    const RootItemMetadata &) const {
  auto panel = std::make_unique<ListActionPanelState>();
  panel->setTitle(title());

  auto *main = panel->createSection();
  main->addAction(AiCommands::runAction(m_command));

  auto *manage = panel->createSection();
  manage->addAction(AiCommands::editAction(m_command));
  manage->addAction(AiCommands::duplicateAction(m_command));

  auto *item = panel->createSection();
  for (auto *action : RootSearchActionGenerator::generateActions(*this, *ctx->services->rootItemManager())) {
    item->addAction(action);
  }

  auto *danger = panel->createSection();
  auto *remove = AiCommands::deleteAction(m_command);
  remove->setShortcut(Keybind::DangerousRemoveAction);
  danger->addAction(remove);

  return panel;
}

AiCommandRootProvider::AiCommandRootProvider(AiCommandStore &store) : m_store(store) {
  connect(&store, &AiCommandStore::changed, this, [this]() { emit itemsChanged(); });
}

QString AiCommandRootProvider::displayName() const {
  return QCoreApplication::translate("AiCommandRootProvider", "AI Commands");
}

QString AiCommandRootProvider::uniqueId() const { return AiCommands::PROVIDER_ID; }

ImageURL AiCommandRootProvider::icon() const { return AiCommands::defaultIconUrl(); }

RootProvider::Type AiCommandRootProvider::type() const { return RootProvider::Type::GroupProvider; }

std::vector<std::shared_ptr<RootItem>> AiCommandRootProvider::loadItems() const {
  const auto commands = m_store.entries();
  std::vector<std::shared_ptr<RootItem>> items;
  items.reserve(commands.size());
  for (const auto &command : commands) {
    items.emplace_back(std::make_shared<RootAiCommandItem>(command));
  }
  return items;
}
