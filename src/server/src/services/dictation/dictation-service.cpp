#include "dictation-service.hpp"
#include <algorithm>
#include "services/ai/ai-service.hpp"

DictationService::DictationService(const std::filesystem::path &dataDir, AI::Service &ai,
                                   MediaControlService &mediaControl, QObject *parent)
    : QObject(parent), m_ai(ai), m_mediaControl(mediaControl),
      m_history(dataDir / "dictation-history.json", this),
      m_vocabulary(dataDir / "dictation-vocabulary.json", this) {}

void DictationService::setSettings(TranscriptionSettings settings) {
  m_settings = std::move(settings);
  emit settingsChanged();
}

std::optional<AI::ProviderModel> DictationService::selectedModel() const {
  if (!m_settings.model) return std::nullopt;
  auto models = m_ai.listModels(AI::Capability::Transcription);
  auto it = std::ranges::find_if(models, [&](const AI::ProviderModel &model) {
    return model.ref.provider == m_settings.model->provider && model.ref.id == m_settings.model->id;
  });
  if (it == models.end()) return std::nullopt;
  return std::move(*it);
}

DictationService::Readiness DictationService::readiness() const {
  if (m_ai.listModels(AI::Capability::Transcription).empty()) return Readiness::NoModels;
  return selectedModel() ? Readiness::Ready : Readiness::NotSelected;
}

std::optional<TranscriptionSetup> DictationService::setup() const {
  const auto model = selectedModel();
  if (!model) return std::nullopt;
  return TranscriptionSetup{
      .model = model->ref,
      .options = {.language = m_settings.language},
      .playSoundEffects = m_settings.playSoundEffects,
      .pauseMedia = m_settings.pauseMedia,
  };
}
