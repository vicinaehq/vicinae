#include "ai-commands.hpp"
#include <array>
#include <QCoreApplication>
#include "builtins/ai/ai-command-form-view-host.hpp"
#include "builtins/ai/ai-command-run.hpp"
#include "common/context.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/ai/ai-service.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/action-panel/action.hpp"
#include "ui/alert/alert.hpp"
#include "ui/settings/settings-controller.hpp"
#include "ui/views/intro-view-host.hpp"

QString AiCommands::outputLabel(AiCommandStore::Output output) {
  switch (output) {
  case AiCommandStore::Output::Paste:
    return QCoreApplication::translate("AiCommands", "Paste in Active App");
  case AiCommandStore::Output::Copy:
    return QCoreApplication::translate("AiCommands", "Copy to Clipboard");
  case AiCommandStore::Output::Chat:
    return QCoreApplication::translate("AiCommands", "Open in AI Chat");
  }
  return {};
}

ImageURL AiCommands::iconUrl(const AiCommandStore::Command &command) {
  if (command.icon.empty()) return defaultIconUrl();
  ImageURL url(QString::fromStdString(command.icon));
  if (url.type() == ImageURLType::Builtin) url.setBackgroundTint(COLOR);
  return url;
}

PlaceholderString AiCommands::parsePrompt(const AiCommandStore::Command &command) {
  static const auto RESERVED = std::to_array<QString>(
      {QString::fromLatin1(SELECTION_PLACEHOLDER), QString::fromLatin1(CLIPBOARD_PLACEHOLDER)});
  return PlaceholderString::parse(QString::fromStdString(command.prompt), RESERVED);
}

ArgumentList AiCommands::arguments(const PlaceholderString &prompt) {
  ArgumentList args;
  args.reserve(prompt.arguments().size());
  for (const auto &arg : prompt.arguments()) {
    args.emplace_back(CommandArgument{
        .name = arg.name,
        .type = CommandArgument::Text,
        .placeholder = arg.name,
        .required = arg.defaultValue.isEmpty(),
    });
  }
  return args;
}

std::vector<QString> AiCommands::completionValues(const ApplicationContext *ctx) {
  const auto values = ctx->navigation->completionValues();
  std::vector<QString> result;
  result.reserve(values.size());
  for (const auto &[name, value] : values) {
    result.emplace_back(value);
  }
  return result;
}

void AiCommands::openCreateForm(const ApplicationContext *ctx) {
  if (!ctx->services->ai()->listModels(AI::Capability::Completion).empty()) {
    ctx->navigation->pushView(new AiCommandFormViewHost);
    return;
  }
  ctx->navigation->pushView(new IntroViewHost(
      QCoreApplication::translate("AiCommands", "Set up an AI provider"),
      QCoreApplication::translate(
          "AiCommands", "AI commands need a language model to rewrite your text. Add an AI provider in "
                        "the settings."),
      defaultIconUrl(), QCoreApplication::translate("AiCommands", "Open AI Settings"), [ctx]() {
        ctx->settings->openSubpage(QStringLiteral("ai"), QStringLiteral("providers"));
        ctx->navigation->closeWindow();
      }));
}

AbstractAction *AiCommands::runAction(const AiCommandStore::Command &command) {
  return new StaticAction(
      QCoreApplication::translate("AiCommands", "Run Command"), iconUrl(command),
      [command](ApplicationContext *ctx) { ctx->aiCommands->run(command, completionValues(ctx)); });
}

AbstractAction *AiCommands::editAction(const AiCommandStore::Command &command) {
  auto *action = new StaticAction(
      QCoreApplication::translate("AiCommands", "Edit Command"), ImageURL::builtin(BuiltinIcon::Pencil),
      [command](ApplicationContext *ctx) {
        ctx->navigation->pushView(new AiCommandFormViewHost(command, AiCommandFormViewHost::Mode::Edit));
      });
  action->setShortcut(Keybind::EditAction);
  return action;
}

AbstractAction *AiCommands::duplicateAction(const AiCommandStore::Command &command) {
  auto *action = new StaticAction(
      QCoreApplication::translate("AiCommands", "Duplicate Command"),
      ImageURL::builtin(BuiltinIcon::Duplicate), [command](ApplicationContext *ctx) {
        ctx->navigation->pushView(new AiCommandFormViewHost(command, AiCommandFormViewHost::Mode::Duplicate));
      });
  action->setShortcut(Keybind::DuplicateAction);
  return action;
}

AbstractAction *AiCommands::createAction() {
  auto *action = new StaticAction(QCoreApplication::translate("AiCommands", "Create AI Command"),
                                  ImageURL::builtin(BuiltinIcon::Plus),
                                  [](ApplicationContext *ctx) { openCreateForm(ctx); });
  action->setShortcut(Keybind::NewAction);
  return action;
}

AbstractAction *AiCommands::deleteAction(const AiCommandStore::Command &command) {
  auto *action = new StaticAction(
      QCoreApplication::translate("AiCommands", "Delete Command"), ImageURL::builtin(BuiltinIcon::Trash),
      [id = command.id, name = QString::fromStdString(command.name)](ApplicationContext *ctx) {
        auto *alert = new CallbackAlertWidget();
        alert->setTitle(QCoreApplication::translate("AiCommands", "Delete \"%1\"?").arg(name));
        alert->setMessage(
            QCoreApplication::translate("AiCommands", "This AI command and its hotkey will be removed."));
        alert->setConfirmText(QCoreApplication::translate("AiCommands", "Delete"), SemanticColor::Red);
        alert->setConfirmCallback([ctx, id]() {
          if (!ctx->services->aiCommands()->remove(id)) {
            ctx->services->toastService()->failure(
                QCoreApplication::translate("AiCommands", "Failed to delete command"));
          }
        });
        ctx->navigation->setDialog(alert);
      });
  action->setStyle(AbstractAction::Style::Danger);
  action->setShortcut(Keybind::RemoveAction);
  return action;
}
