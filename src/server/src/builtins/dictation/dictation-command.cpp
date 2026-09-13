#include "dictation-command.hpp"
#include <algorithm>
#include <QPointer>
#include <optional>
#include <string_view>
#include <QJsonObject>
#include "builtins/dictation/dictation-extension.hpp"
#include "builtins/dictation/dictation-models-view-host.hpp"
#include "builtins/dictation/dictation-session.hpp"
#include "builtins/dictation/transcribe-view-host.hpp"
#include "common/context.hpp"
#include "common/entrypoint.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/ai/ai-service.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "ui/settings/settings-controller.hpp"
#include "utils/environment.hpp"
#include "ui/views/intro-view-host.hpp"

namespace {

const auto NO_MODEL_ICON =
    ImageURL::builtin(BuiltinIcon::MicrophoneDisabled).setBackgroundTint(Dictation::COLOR);
const auto SELECT_MODEL_ICON =
    ImageURL::builtin(BuiltinIcon::Microphone).setBackgroundTint(Dictation::COLOR).setBadge(BuiltinIcon::Cog);

constexpr qint64 HOLD_THRESHOLD_MS = 300;

QPointer<DictationSession> active;

enum class Readiness { NoModels, NotSelected, Ready };

struct Status {
  Readiness readiness;
  std::optional<AI::ProviderModel> model;
  AI::TranscriptionOptions options;
};

std::optional<std::string> configuredLanguage(const QJsonObject &values) {
  const auto language = values.value(Dictation::qs(Dictation::LANGUAGE_PREFERENCE)).toString();
  if (language.isEmpty() || language == Dictation::qs(Dictation::AUTO_LANGUAGE)) return std::nullopt;
  return language.toStdString();
}

Status status(const ApplicationContext *ctx) {
  auto models = ctx->services->ai()->listModels(AI::Capability::Transcription);
  if (models.empty()) return {.readiness = Readiness::NoModels};

  const auto values =
      ctx->services->rootItemManager()->getProviderPreferenceValues(Dictation::qs(Dictation::REPOSITORY_ID));
  const auto selected = values.value(Dictation::qs(Dictation::MODEL_PREFERENCE)).toString();
  if (selected.isEmpty() || selected == Dictation::qs(Dictation::NO_MODEL))
    return {.readiness = Readiness::NotSelected};

  const auto ref = AI::ModelRef::fromString(selected.toStdString());
  if (!ref) return {.readiness = Readiness::NotSelected};

  auto it = std::ranges::find_if(models, [&](const AI::ProviderModel &model) {
    return model.ref.provider == ref->provider && model.ref.id == ref->id;
  });
  if (it == models.end()) return {.readiness = Readiness::NotSelected};

  return {.readiness = Readiness::Ready,
          .model = std::move(*it),
          .options = {.language = configuredLanguage(values)}};
}

void startDictation(const ApplicationContext *ctx, const AI::ModelRef &model,
                    const AI::TranscriptionOptions &options) {
  if (Environment::isHudDisabled()) {
    ctx->navigation->pushView(new TranscribeViewHost(model, options));
    return;
  }

  ctx->navigation->closeWindow();

  if (active && active->isActive()) {
    active->accept();
    return;
  }

  active = new DictationSession(ctx, model, options, ctx->navigation.get());
  active->start();
}

} // namespace

ImageURL TranscribeCommand::iconUrl() const { return Dictation::ICON; }

void TranscribeCommand::shortcutReleased() const {
  if (!active || !active->isRecording() || active->recordingMs() < HOLD_THRESHOLD_MS) return;
  active->accept();
}

void TranscribeCommand::execute(CommandController &controller) const {
  auto *ctx = controller.context();
  const auto status = ::status(ctx);

  switch (status.readiness) {
  case Readiness::Ready:
    startDictation(ctx, status.model->ref, status.options);
    return;
  case Readiness::NoModels:
    ctx->navigation->pushView(
        new IntroViewHost(tr("Set up dictation"),
                          tr("Dictation needs a speech model before it can turn your voice into text. "
                             "Install one that runs on this machine, or add an AI provider that offers "
                             "transcription."),
                          NO_MODEL_ICON, tr("Manage Dictation Models"),
                          [ctx]() { ctx->navigation->replaceView(new DictationModelsViewHost); }));
    return;
  case Readiness::NotSelected: {
    auto *intro = new IntroViewHost(tr("Choose a transcription model"),
                                    tr("Pick the model dictation should use from the dictation settings. "
                                       "You can change it at any time."),
                                    SELECT_MODEL_ICON, tr("Open Dictation Settings"), [ctx]() {
                                      ctx->settings->openExtensionPreferences(
                                          EntrypointId{std::string(Dictation::REPOSITORY_ID), "transcribe"});
                                      ctx->navigation->closeWindow();
                                    });
    intro->addSecondaryAction(tr("Manage Dictation Models"), ImageURL::builtin(BuiltinIcon::Download),
                              [ctx]() { ctx->navigation->replaceView(new DictationModelsViewHost); });
    ctx->navigation->pushView(intro);
    return;
  }
  }
}
