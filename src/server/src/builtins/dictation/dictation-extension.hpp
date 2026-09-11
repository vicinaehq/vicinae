#pragma once
#include "services/builtin-icon/builtin-icon.hpp"
#include "command/command-database.hpp"
#include "command/single-view-command-context.hpp"
#include "builtins/dictation/dictation-models-view-host.hpp"
#include "builtins/dictation/transcribe-view-host.hpp"
#include "theme/colors.hpp"
#include "ui/image/url.hpp"
#include "services/toast/toast-service.hpp"

namespace {
const auto COLOR = SemanticColor::Blue;
const auto DICTATION_ICON = ImageURL::builtin(BuiltinIcon::Microphone).setBackgroundTint(COLOR);

class UnimplementedCommand : public BuiltinCallbackCommand {
  void execute(CommandController &controller) const override {
    controller.context()->services->toastService()->failure("Not implemented");
  }
};

class TranscribeCommand : public BuiltinViewCommand<TranscribeViewHost> {
  Q_DECLARE_TR_FUNCTIONS(TranscribeCommand)

  QString id() const override { return "transcribe"; }
  QString name() const override { return tr("Transcribe"); }
  ImageURL iconUrl() const override { return DICTATION_ICON; }
  std::vector<QString> keywords() const override { return {"dictate"}; }
};

class ManageModelsCommand : public BuiltinViewCommand<DictationModelsViewHost> {
  Q_DECLARE_TR_FUNCTIONS(ManageModelsCommand)

  QString id() const override { return "models"; }
  QString name() const override { return tr("Manage Dictation Models"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Microphone)
        .setBackgroundTint(COLOR)
        .setBadge(BuiltinIcon::Download);
  }
  std::vector<QString> keywords() const override { return {"whisper", "parakeet", "speech"}; }
};

class AddVocabCommand : public UnimplementedCommand {
  Q_DECLARE_TR_FUNCTIONS(AddVocabCommand)

  QString id() const override { return "add-vocab"; }
  QString name() const override { return tr("Add Word to Vocabulary"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::BookAntique).setBackgroundTint(COLOR).setBadge(BuiltinIcon::Plus);
  }
  std::vector<QString> keywords() const override { return {}; }
};

class DictationHistoryCommand : public UnimplementedCommand {
  Q_DECLARE_TR_FUNCTIONS(DictationHistoryCommand)

  QString id() const override { return "history"; }
  QString name() const override { return tr("Dictation History"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Microphone)
        .setBackgroundTint(COLOR)
        .setBadge(BuiltinIcon::MagnifyingGlass);
  }
  std::vector<QString> keywords() const override { return {}; }
};

class VocabularyCommand : public UnimplementedCommand {
  Q_DECLARE_TR_FUNCTIONS(VocabularyCommand)

  QString id() const override { return "vocab"; }
  QString name() const override { return tr("Vocabulary"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::BookAntique).setBackgroundTint(COLOR);
  }
  std::vector<QString> keywords() const override { return {}; }
};

}; // namespace

class DictationExtension : public BuiltinCommandRepository {
  Q_DECLARE_TR_FUNCTIONS(DictationExtension)

  QString id() const override { return "dictation"; }
  QString displayName() const override { return tr("Dictation"); }
  ImageURL iconUrl() const override { return DICTATION_ICON; }

public:
  DictationExtension() {
    registerCommand<TranscribeCommand>();
    registerCommand<ManageModelsCommand>();
    registerCommand<VocabularyCommand>();
    registerCommand<AddVocabCommand>();
    registerCommand<DictationHistoryCommand>();
  }
};
