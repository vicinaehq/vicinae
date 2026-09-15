#include "dictation-extension.hpp"
#include <algorithm>
#include <ranges>
#include <vector>
#include "builtins/ai/ai-model-selector-utils.hpp"
#include "service-registry.hpp"
#include "services/ai/speech-language-catalogue.hpp"
#include "services/media-control/media-control-service.hpp"
#include "services/paste/paste-service.hpp"

namespace {

Preference::DropdownData::Option languageOption(const SpeechLanguage &lang) {
  return {.title = SpeechLanguageCatalogue::displayName(lang), .value = Dictation::qs(lang.code)};
}

} // namespace

std::vector<Preference> DictationExtension::preferences() const {
  using namespace Dictation;

  auto registry = ServiceRegistry::instance();
  std::vector<Preference> preferences;

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
  model.setDescription(tr("Model used to turn your voice into text. You can configure cloud options, or "
                          "Vicinae can download and run local transcription models for you."));
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
  language.setDescription(
      tr("Language you dictate in. Some models can auto-detect it, others need it set explicitly."));
  language.setDefaultValue(qs(AUTO_LANGUAGE));
  language.setRequired(false);

  auto soundEffects = Preference::makeCheckbox("sound");

  soundEffects.setTitle(tr("Sound Effects"));
  soundEffects.setDescription(tr("Whether to play a sound effect when starting or stopping dictation."));
  soundEffects.setDefaultValue(true);

  preferences.emplace_back(model);
  preferences.emplace_back(language);
  preferences.emplace_back(soundEffects);

  if (registry->mediaControl()->available()) {
    auto pauseMedia = Preference::makeCheckbox("pauseMedia");

    pauseMedia.setTitle(tr("Pause media"));
    pauseMedia.setDescription("Pause all media players when recording and resume them after");
    pauseMedia.setDefaultValue(true);
    preferences.emplace_back(pauseMedia);
  }

  std::vector<Preference::DropdownData::Option> defaultActionOptions;
  QString dflt = "copy";

  if (registry->pasteService()->supportsPaste()) {
    defaultActionOptions.emplace_back(
        Preference::DropdownData::Option{tr("Paste to active window"), Dictation::ACTION_PASTE});
    dflt = "paste";
  }
  defaultActionOptions.emplace_back(
      Preference::DropdownData::Option{tr("Copy to clipboard"), Dictation::ACTION_COPY});

  auto defaultAction = Preference::makeDropdown("dictationAction", defaultActionOptions);

  defaultAction.setDefaultValue(dflt);
  defaultAction.setTitle(tr("Dictation Action"));
  defaultAction.setDescription(tr("What to do after dictation"));

  preferences.emplace_back(defaultAction);

  auto history = Preference::makeCheckbox(Dictation::qs(Dictation::HISTORY_PREFERENCE));
  history.setTitle(tr("Save dictation history"));
  history.setDescription(
      tr("Keep what you dictated so you can find and reuse it later. Stored as plain text."));
  history.setDefaultValue(true);

  preferences.emplace_back(history);

  return preferences;
}
