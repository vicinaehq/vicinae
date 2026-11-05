#include "dictation-command.hpp"
#include <algorithm>
#include <QPointer>
#include <functional>
#include <optional>
#include <qlogging.h>
#include <string_view>
#include "builtins/dictation/dictation-session.hpp"
#include "builtins/dictation/transcribe-view-host.hpp"
#include "common/context.hpp"
#include "common/entrypoint.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/dictation/dictation-service.hpp"
#include "services/paste/paste-service.hpp"
#include "services/permissions/macos-permission-service.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/settings/settings-controller.hpp"
#include "utils/environment.hpp"
#include "ui/views/intro-view-host.hpp"

namespace {

const auto MICROPHONE_OFF_ICON =
    ImageURL::builtin(BuiltinIcon::MicrophoneDisabled).setBackgroundTint(Dictation::COLOR);

constexpr qint64 HOLD_THRESHOLD_MS = 300;

QPointer<DictationSession> active;

void startDictation(const ApplicationContext *ctx, const TranscriptionSetup &setup,
                    Dictation::DictationAction action, bool closeLauncher) {
  const bool hud = !Environment::isHudDisabled();

  if (hud && closeLauncher) ctx->navigation->closeWindow();

  if (active && active->isActive()) {
    active->accept();
    return;
  }

  // Without the HUD the launcher presents the dictation, and closes to give focus back before pasting.
  active = new DictationSession(ctx, setup, action,
                                hud ? PasteService::Target::CurrentFocus : PasteService::Target::FocusHandoff,
                                ctx->navigation.get());

  if (!active->start()) {
    const auto message = TranscribeCommand::tr("Could not start recording");
    if (hud) {
      ctx->navigation->showHud(message, ImageURL::builtin(BuiltinIcon::MicrophoneDisabled));
    } else {
      ctx->services->toastService()->failure(message);
    }
    return;
  }

  if (hud) {
    ctx->navigation->showDictationHud(active);
  } else {
    ctx->navigation->pushView(new TranscribeViewHost(active));
  }
}

void withMicrophoneAccess(const ApplicationContext *ctx, std::function<void()> start) {
  using vicinae::permissions::MicrophoneStatus;

  switch (vicinae::permissions::microphoneStatus()) {
  case MicrophoneStatus::Granted:
    start();
    return;
  case MicrophoneStatus::NotDetermined:
    ctx->navigation->closeWindow();
    vicinae::permissions::requestMicrophone([start = std::move(start)](bool granted) {
      if (granted) start();
    });
    return;
  case MicrophoneStatus::Denied:
    ctx->navigation->pushView(new IntroViewHost(
        TranscribeCommand::tr("Allow microphone access"),
        TranscribeCommand::tr("Dictation needs to hear you. Turn on the microphone for Vicinae in "
                              "System Settings, then try again."),
        MICROPHONE_OFF_ICON, TranscribeCommand::tr("Open System Settings"), [ctx]() {
          vicinae::permissions::openMicrophoneSettings();
          ctx->navigation->closeWindow();
        }));
    return;
  }
}

} // namespace

bool Dictation::hasActiveSession() { return active && active->isActive(); }

bool Dictation::isRecording() { return active && active->isRecording(); }

bool Dictation::acceptSession() {
  if (!isRecording()) return false;
  active->accept();
  return true;
}

bool Dictation::cancelSession() {
  if (!hasActiveSession()) return false;
  active->cancel();
  return true;
}

ImageURL TranscribeCommand::iconUrl() const { return Dictation::ICON; }

void TranscribeCommand::shortcutReleased() const {
  if (!active || !active->isRecording() || active->recordingMs() < HOLD_THRESHOLD_MS) return;
  active->accept();
}

void TranscribeCommand::execute(const Controller &controller) const {
  auto *ctx = controller.context();
  auto *dictation = ctx->services->dictation();
  const auto action = ctx->services->pasteService()->supportsPaste()
                          ? controller.repositoryPreferences().dictationAction
                          : Dictation::DictationAction::CopyToClipboard;

  switch (dictation->readiness()) {
  case DictationService::Readiness::Ready:
    withMicrophoneAccess(ctx, [ctx, setup = *dictation->setup(), action,
                               closeLauncher = controller.launchProps().source == LaunchSource::Launcher]() {
      startDictation(ctx, setup, action, closeLauncher);
    });
    return;
  case DictationService::Readiness::NoModels:
    ctx->navigation->pushView(
        new IntroViewHost(tr("Set up dictation"),
                          tr("Dictation needs a speech model before it can turn your voice into text. "
                             "Install one that runs on this machine, or add an AI provider that offers "
                             "transcription."),
                          Dictation::ICON, tr("Open AI Settings"), [ctx]() {
                            ctx->settings->openSubpage(QStringLiteral("ai"), QStringLiteral("providers"));
                            ctx->navigation->closeWindow();
                          }));
    return;
  case DictationService::Readiness::NotSelected: {
    auto *intro = new IntroViewHost(tr("Choose a transcription model"),
                                    tr("Pick the model dictation should use from the dictation settings. "
                                       "You can change it at any time."),
                                    Dictation::ICON, tr("Open Dictation Settings"), [ctx]() {
                                      ctx->settings->openExtensionPreferences(
                                          EntrypointId{std::string(Dictation::REPOSITORY_ID), "transcribe"});
                                      ctx->navigation->closeWindow();
                                    });
    ctx->navigation->pushView(intro);
    return;
  }
  }
}
