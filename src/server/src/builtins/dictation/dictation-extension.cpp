#include "dictation-extension.hpp"
#include <algorithm>
#include <ranges>
#include <vector>
#include "builtins/ai/ai-model-selector-utils.hpp"
#include "service-registry.hpp"
#include "services/ai/ai-service.hpp"
#include "services/local-speech-model-registry/speech-language-catalogue.hpp"

namespace {

Preference::DropdownData::Option languageOption(const SpeechLanguage &lang) {
  return {.title = SpeechLanguageCatalogue::displayName(lang), .value = Dictation::qs(lang.code)};
}

} // namespace

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

  std::vector<Preference::DropdownData::Option> topOptions;
  topOptions.reserve(2);
  topOptions.emplace_back(
      Preference::DropdownData::Option{.title = tr("Auto-detect"), .value = qs(AUTO_LANGUAGE)});
  if (const auto *lang = SpeechLanguageCatalogue::systemLanguage()) {
    topOptions.emplace_back(languageOption(*lang));
  }

  auto all = SpeechLanguageCatalogue::entries() | std::views::transform(languageOption) |
             std::ranges::to<std::vector>();
  std::ranges::sort(
      all, [](const auto &a, const auto &b) { return QString::localeAwareCompare(a.title, b.title) < 0; });

  auto language = Preference::makeDropdown(qs(LANGUAGE_PREFERENCE),
                                           std::vector<Preference::DropdownData::Section>{
                                               {.options = std::move(topOptions)},
                                               {.title = tr("All languages"), .options = std::move(all)},
                                           });
  language.setTitle(tr("Language"));
  language.setDescription(tr("Language you dictate in. Auto-detect works with multilingual models, picking a "
                             "language is more accurate."));
  language.setDefaultValue(qs(AUTO_LANGUAGE));
  language.setRequired(false);

  return {model, language};
}
