#pragma once
#include <qcoreapplication.h>
#include "builtins/ai/local-models-view-host.hpp"
#include "command/command-database.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "theme/colors.hpp"
#include "ui/image/url.hpp"

namespace {

#ifdef HAS_LOCAL_AI
class ManageLocalModelsCommand : public BuiltinViewCommand<LocalModelsViewHost> {
  Q_DECLARE_TR_FUNCTIONS(ManageLocalModelsCommand)

  QString id() const override { return "manage-local-models"; }
  QString name() const override { return tr("Manage Local Models"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::HardDrive)
        .setBackgroundTint(SemanticColor::Purple)
        .setBadge(BuiltinIcon::Download);
  }
  std::vector<QString> keywords() const override {
    return {"whisper", "parakeet", "llama", "speech", "transcription"};
  }
};
#endif

}; // namespace

class AiExtension : public BuiltinCommandRepository {
  Q_DECLARE_TR_FUNCTIONS(AiExtension)

  QString id() const override { return "ai"; }
  QString displayName() const override { return tr("AI"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Atom).setBackgroundTint(SemanticColor::Purple);
  }

public:
  AiExtension() {
#ifdef HAS_LOCAL_AI
    registerCommand<ManageLocalModelsCommand>();
#endif
  }
};
