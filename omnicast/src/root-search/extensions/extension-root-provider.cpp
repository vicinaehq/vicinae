#include "root-search/extensions/extension-root-provider.hpp"
#include "action-panel/action-panel.hpp"
#include "actions/fallback-actions.hpp"
#include "actions/root-search/root-search-actions.hpp"
#include "command-actions.hpp"
#include "service-registry.hpp"
#include "base-view.hpp"

QString CommandRootItem::displayName() const { return m_command->name(); }
QString CommandRootItem::subtitle() const { return m_command->repositoryName(); }
OmniIconUrl CommandRootItem::iconUrl() const { return m_command->iconUrl(); }
ArgumentList CommandRootItem::arguments() const { return m_command->arguments(); }
QString CommandRootItem::providerId() const { return "command"; }
bool CommandRootItem::isSuitableForFallback() const { return m_command->isFallback(); }
double CommandRootItem::baseScoreWeight() const { return 1.1; }

ActionPanelView *CommandRootItem::actionPanel() const {
  auto panel = new ActionPanelStaticListView;
  auto open = new OpenBuiltinCommandAction(m_command, "Open command");
  auto resetRanking = new ResetItemRanking(uniqueId());
  auto markAsFavorite = new MarkItemAsFavorite(uniqueId());

  panel->setTitle(m_command->name());
  panel->addAction(new DefaultActionWrapper(uniqueId(), open));
  panel->addSection();
  panel->addAction(resetRanking);
  panel->addAction(markAsFavorite);
  panel->addSection();
  panel->addAction(new DisableApplication(uniqueId()));

  return panel;
}

ActionPanelView *CommandRootItem::fallbackActionPanel() const {
  auto panel = new ActionPanelStaticListView;
  auto ui = ServiceRegistry::instance()->UI();

  panel->addAction(new OpenBuiltinCommandAction(m_command, "Open command", ui->topView()->searchText()));
  panel->addAction(new ManageFallbackActions);

  return panel;
}

QString CommandRootItem::uniqueId() const { return QString("extension.%1").arg(m_command->uniqueId()); }

AccessoryList CommandRootItem::accessories() const {
  return {{.text = "Command", .color = ColorTint::TextSecondary}};
}

std::vector<std::shared_ptr<RootItem>> ExtensionRootProvider::loadItems() const {
  return m_repo->commands() | std::views::transform([](const auto &command) {
           return std::static_pointer_cast<RootItem>(std::make_shared<CommandRootItem>(command));
         }) |
         std::ranges::to<std::vector>();
}
