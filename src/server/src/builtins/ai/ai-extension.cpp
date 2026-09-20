#include "ai-extension.hpp"
#include <algorithm>
#include "builtins/ai/quick-ai-view-host.hpp"
#include "command/command-controller.hpp"
#include "common/context.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/ai/ai-service.hpp"
#include "ui/settings/settings-controller.hpp"
#include "ui/views/intro-view-host.hpp"

namespace {

const auto SELECT_MODEL_ICON = ImageURL::builtin(BuiltinIcon::Stars)
                                   .setBackgroundTint(Omnicast::ACCENT_COLOR)
                                   .setBadge(BuiltinIcon::Cog);

} // namespace

void QuickAICommand::execute(const Controller &controller) const {
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

  const auto ref = AI::ModelRef::fromString(controller.preferences().model);
  const bool available = ref && std::ranges::any_of(models, [&](const AI::ProviderModel &model) {
                           return model.ref.provider == ref->provider && model.ref.id == ref->id;
                         });

  if (!available) {
    ctx->navigation->pushView(new IntroViewHost(
        tr("Choose a model"),
        tr("Pick the model Quick AI should use from its settings. You can change it at any time."),
        SELECT_MODEL_ICON, tr("Open Quick AI Settings"), [ctx, id = uniqueId()]() {
          ctx->settings->openExtensionPreferences(id);
          ctx->navigation->closeWindow();
        }));
    return;
  }

  const auto query = controller.launchProps().fallbackText.value_or(QString());
  ctx->navigation->pushView(new QuickAIViewHost(query, *ref));
}

void AiExtension::preferencesChanged(const AiPreferences &preferences) const {
  ServiceRegistry::instance()->ai()->setProviders(preferences.providers);
}
