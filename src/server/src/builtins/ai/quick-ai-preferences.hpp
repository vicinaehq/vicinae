#pragma once
#include <algorithm>
#include <optional>
#include <string>
#include <vector>
#include <QCoreApplication>
#include "builtins/ai/ai-model-selector-utils.hpp"
#include "builtins/ai/ai.hpp"
#include "command/preference-schema.hpp"
#include "service-registry.hpp"
#include "services/ai/ai-service.hpp"

namespace QuickAI {

/**
 * The model Quick AI is set to use, if it can currently be used.
 */
inline std::optional<AI::ModelRef> availableModel(AI::Service &ai, const std::string &preference) {
  const auto ref = AI::ModelRef::fromString(preference);
  if (!ref) return std::nullopt;
  const auto models = ai.listModels(AI::Capability::Completion);
  const bool available = std::ranges::any_of(models, [&](const AI::ProviderModel &model) {
    return model.ref.provider == ref->provider && model.ref.id == ref->id;
  });
  return available ? std::optional(*ref) : std::nullopt;
}

} // namespace QuickAI

struct QuickAIPreferences {
  std::string model = std::string(QuickAI::NO_MODEL);
};

template <> struct PreferenceSchema<QuickAIPreferences> {
  PreferenceMeta model{
      .title = tr("Model"),
      .description =
          tr("Model used to answer your questions. Any provider set up in the AI settings can be used."),
      .required = false,
      .sections =
          [] {
            auto sections =
                buildModelDropdownSections(ServiceRegistry::instance()->ai(), AI::Capability::Completion);
            sections.insert(sections.begin(),
                            Preference::DropdownData::Section{.options = {Preference::DropdownData::Option{
                                                                  .title = tr("None"),
                                                                  .value = QuickAI::qs(QuickAI::NO_MODEL),
                                                              }}});
            return sections;
          },
  };
  Q_DECLARE_TR_FUNCTIONS(QuickAIPreferences)
};
