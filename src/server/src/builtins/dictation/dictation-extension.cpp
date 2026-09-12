#include "dictation-extension.hpp"
#include "builtins/ai/ai-model-selector-utils.hpp"
#include "service-registry.hpp"
#include "services/ai/ai-service.hpp"

std::vector<Preference> DictationExtension::preferences() const {
  using namespace Dictation;

  auto sections =
      buildModelDropdownSections(ServiceRegistry::instance()->ai(), AI::Capability::Transcription);
  sections.insert(sections.begin(), Preference::DropdownData::Section{
                                        .options = {Preference::DropdownData::Option{
                                            .title = tr("None"),
                                            .value = qs(NO_MODEL),
                                            .icon = ImageURL::builtin(BuiltinIcon::MicrophoneDisabled),
                                        }},
                                    });

  auto model = Preference::makeDropdown(qs(MODEL_PREFERENCE), sections);
  model.setTitle(tr("Transcription model"));
  model.setDescription(tr("Model used to turn your voice into text. Local models run offline."));
  model.setDefaultValue(qs(NO_MODEL));
  model.setRequired(false);
  return {model};
}
