#include "ai-commands-view-host.hpp"
#include <algorithm>
#include "builtins/ai/ai-commands.hpp"
#include "builtins/dictation/dictation.hpp"
#include "service-registry.hpp"
#include "services/ai/ai-service.hpp"
#include "theme/colors.hpp"
#include "ui/action-panel/action-panel-state.hpp"
#include "ui/action-panel/action.hpp"

void AiCommandsViewHost::onMount() {
  setSearchPlaceholderText(tr("Search AI commands..."));
  m_emptyTitle = tr("No AI commands yet");
  m_emptyDescription = tr("Create a command to rewrite the selected text with AI from any app.");
  m_emptyIcon = ImageUrl{AiCommands::defaultIconUrl()};

  auto *services = context()->services;
  connect(services->aiCommands(), &AiCommandStore::changed, this, [this]() { reload(); });
  connect(services->ai(), &AI::Service::modelsChanged, this, [this]() { reload(); });
  reload();
}

void AiCommandsViewHost::reload() {
  const auto commands = context()->services->aiCommands()->entries();
  setItems({commands.begin(), commands.end()});
  if (commands.empty()) setActions(emptyActionPanel());
}

std::optional<QString> AiCommandsViewHost::modelName(const ItemType &command) const {
  const auto ref = AI::ModelRef::fromString(command.model);
  if (!ref) return std::nullopt;
  const auto models = context()->services->ai()->listModels(AI::Capability::Completion);
  const auto it = std::ranges::find_if(models, [&](const AI::ProviderModel &model) {
    return model.ref.provider == ref->provider && model.ref.id == ref->id;
  });
  if (it == models.end()) return std::nullopt;
  return QString::fromStdString(it->name);
}

std::optional<ImageURL> AiCommandsViewHost::displayIcon(const ItemType &command) const {
  return AiCommands::iconUrl(command);
}

AccessoryList AiCommandsViewHost::displayAccessories(const ItemType &command) const {
  if (modelName(command)) return {};
  return {ListAccessory{.text = tr("Model unavailable"), .color = SemanticColor::Red}};
}

std::optional<ListItemDetail> AiCommandsViewHost::displayDetail(const ItemType &command) const {
  QVariantList meta;
  meta.append(QVariantMap{
      {QStringLiteral("label"), tr("Model")},
      {QStringLiteral("value"), modelName(command).value_or(tr("Unavailable"))},
  });
  meta.append(QVariantMap{
      {QStringLiteral("label"), tr("Output")},
      {QStringLiteral("value"), AiCommands::outputLabel(command.output)},
  });

  return ListItemDetail{.metadata = std::move(meta),
                        .markdown = Dictation::escapeMarkdown(QString::fromStdString(command.prompt))};
}

std::unique_ptr<ActionPanelState> AiCommandsViewHost::buildActionPanel(const ItemType &command) const {
  auto panel = std::make_unique<ListActionPanelState>();
  panel->setTitle(QString::fromStdString(command.name));

  auto *main = panel->createSection();
  main->addAction(AiCommands::runAction(command));

  auto *manage = panel->createSection();
  manage->addAction(AiCommands::editAction(command));
  manage->addAction(AiCommands::duplicateAction(command));
  manage->addAction(AiCommands::createAction());

  panel->createSection()->addAction(AiCommands::deleteAction(command));

  return panel;
}

std::unique_ptr<ActionPanelState> AiCommandsViewHost::emptyActionPanel() {
  auto panel = std::make_unique<ListActionPanelState>();
  panel->createSection()->addAction(AiCommands::createAction());
  return panel;
}
