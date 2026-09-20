#include "ai-extension.hpp"
#include <algorithm>
#include "builtins/ai/ai-model-selector-utils.hpp"
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

std::vector<Preference> QuickAICommand::preferences() const {
  using namespace QuickAI;

  auto sections = buildModelDropdownSections(ServiceRegistry::instance()->ai(), AI::Capability::Completion);
  sections.insert(sections.begin(),
                  Preference::DropdownData::Section{.options = {Preference::DropdownData::Option{
                                                        .title = tr("None"),
                                                        .value = qs(NO_MODEL),
                                                    }}});

  auto model = Preference::makeDropdown(qs(MODEL_PREFERENCE), std::move(sections));
  model.setTitle(tr("Model"));
  model.setDescription(tr("Model used to answer your questions. Any provider set up in the AI settings can "
                          "be used."));
  model.setDefaultValue(qs(NO_MODEL));
  model.setRequired(false);

  return {model};
}

void QuickAICommand::execute(CommandController &controller) const {
  using namespace QuickAI;

  auto *ctx = controller.context();
  const auto models = ctx->services->ai()->listModels(AI::Capability::Completion);

  if (models.empty()) {
    ctx->navigation->pushView(new IntroViewHost(
        tr("Set up an AI provider"),
        tr("Quick AI needs a language model before it can answer. Add an AI provider in the settings."),
        iconUrl(), tr("Open AI Settings"), [ctx]() {
          ctx->settings->openTab("ai-providers");
          ctx->navigation->closeWindow();
        }));
    return;
  }

  const auto selected = controller.preferenceValues().value(qs(MODEL_PREFERENCE)).toString();
  const auto ref = AI::ModelRef::fromString(selected.toStdString());
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
