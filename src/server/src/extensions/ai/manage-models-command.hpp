#include "qml/manage-models-view-host.hpp"
#include "../../ui/image/url.hpp"
#include "services/ai/ai-provider.hpp"
#include "single-view-command-context.hpp"
#include "services/ai/ai-service.hpp"
#include "services/paste/paste-service.hpp"
#include "services/toast/toast-service.hpp"
#include <qlogging.h>

class ManageModelsCommand : public BuiltinViewCommand<ManageModelsViewHost> {
  QString id() const override { return "manage-models"; }
  QString name() const override { return "Manage Models"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("computer-chip").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
};

class TranscribeCommand : public BuiltinCallbackCommand {
  QString id() const override { return "transcribe"; }
  QString name() const override { return "Transcribe"; }
  ImageURL iconUrl() const override {
    return ImageURL::emoji("🎤").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  void execute(CommandController *controller) const override {
    auto ai = controller->context()->services->ai();
    auto ctx = controller->context();
    auto toast = ctx->services->toastService();

    toast->dynamic("Transcribing...");

    ai->transcribe("/tmp/hello.mp4").then([ctx, toast](const AI::TranscriptionResult &result) {
      if (!result) {
        toast->failure("Transcription failed");
        qWarning() << "Failed to transcribe" << result.error();
        return;
      }

      toast->success("Transcription succeeded");
      ctx->services->pasteService()->pasteContent(Clipboard::Text(result->text.c_str()));
      ctx->navigation->closeWindow();
    });
  }
};
