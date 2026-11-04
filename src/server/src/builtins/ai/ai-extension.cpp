#include "ai-extension.hpp"
#include <algorithm>
#include "builtins/ai/quick-ai-view-host.hpp"
#include "command/command-controller.hpp"
#include "common/context.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/ai/ai-service.hpp"
#include "ui/settings/settings-controller.hpp"
#include "ui/windows/chat-window.hpp"
#include "ui/views/intro-view-host.hpp"

void QuickAICommand::execute(CommandController &controller) const {
  auto *ctx = controller.context();
  const auto models = ctx->services->ai()->listModels(AI::Capability::Completion);

  if (models.empty()) {
    ctx->navigation->pushView(new IntroViewHost(
        tr("Set up an AI provider"),
        tr("Quick AI needs a language model before it can answer. Add an AI provider in the settings."),
        iconUrl(), tr("Open AI Settings"), [ctx]() {
          ctx->settings->openSubpage(QStringLiteral("ai"), QStringLiteral("providers"));
          ctx->navigation->closeWindow();
        }));
    return;
  }

  const auto query = controller.launchProps().fallbackText.value_or(QString());
  ctx->navigation->pushView(new QuickAIViewHost(query));
}

void AiExtension::preferencesChanged(const AiPreferences &preferences) const {
  ServiceRegistry::instance()->ai()->setProviders(preferences.providers);
}

void AIChatCommand::execute(CommandController &controller) const {
  auto *ctx = controller.context();
  ctx->navigation->closeWindow();
  ctx->chat->show();
}

void OpenLastChatCommand::execute(CommandController &controller) const {
  controller.context()->navigation->pushView(QuickAIViewHost::forLastConversation());
}
