#pragma once
#include "builtins/dictation/dictation-models-view-host.hpp"
#include "builtins/dictation/transcribe-command.hpp"
#include <string_view>
#include "command/command-database.hpp"
#include "command/single-view-command-context.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "services/toast/toast-service.hpp"
#include "theme/colors.hpp"
#include "ui/image/url.hpp"

namespace Dictation {

constexpr auto REPOSITORY_ID = std::string_view("dictation");
constexpr auto MODEL_PREFERENCE = std::string_view("model");
constexpr auto NO_MODEL = std::string_view("none");
constexpr auto COLOR = SemanticColor::Blue;

inline const auto ICON = ImageURL::builtin(BuiltinIcon::Microphone).setBackgroundTint(COLOR);

inline QString qs(std::string_view view) {
  return QString::fromUtf8(view.data(), static_cast<qsizetype>(view.size()));
}

} // namespace Dictation

namespace {

class UnimplementedCommand : public BuiltinCallbackCommand {
  void execute(CommandController &controller) const override {
    controller.context()->services->toastService()->failure("Not implemented");
  }
};

class ManageModelsCommand : public BuiltinViewCommand<DictationModelsViewHost> {
  Q_DECLARE_TR_FUNCTIONS(ManageModelsCommand)

  QString id() const override { return "models"; }
  QString name() const override { return tr("Manage Dictation Models"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Microphone)
        .setBackgroundTint(Dictation::COLOR)
        .setBadge(BuiltinIcon::Download);
  }
  std::vector<QString> keywords() const override { return {"whisper", "parakeet", "speech"}; }
};

class AddVocabCommand : public UnimplementedCommand {
  Q_DECLARE_TR_FUNCTIONS(AddVocabCommand)

  QString id() const override { return "add-vocab"; }
  QString name() const override { return tr("Add Word to Vocabulary"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::BookAntique)
        .setBackgroundTint(Dictation::COLOR)
        .setBadge(BuiltinIcon::Plus);
  }
  std::vector<QString> keywords() const override { return {}; }
};

class DictationHistoryCommand : public UnimplementedCommand {
  Q_DECLARE_TR_FUNCTIONS(DictationHistoryCommand)

  QString id() const override { return "history"; }
  QString name() const override { return tr("Dictation History"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Microphone)
        .setBackgroundTint(Dictation::COLOR)
        .setBadge(BuiltinIcon::MagnifyingGlass);
  }
  std::vector<QString> keywords() const override { return {}; }
};

class VocabularyCommand : public UnimplementedCommand {
  Q_DECLARE_TR_FUNCTIONS(VocabularyCommand)

  QString id() const override { return "vocab"; }
  QString name() const override { return tr("Vocabulary"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::BookAntique).setBackgroundTint(Dictation::COLOR);
  }
  std::vector<QString> keywords() const override { return {}; }
};

}; // namespace

class DictationExtension : public BuiltinCommandRepository {
  Q_DECLARE_TR_FUNCTIONS(DictationExtension)

  QString id() const override { return Dictation::qs(Dictation::REPOSITORY_ID); }
  QString displayName() const override { return tr("Dictation"); }
  ImageURL iconUrl() const override { return Dictation::ICON; }
  std::vector<Preference> preferences() const override;

public:
  DictationExtension() {
    registerCommand<TranscribeCommand>();
    registerCommand<ManageModelsCommand>();
    registerCommand<VocabularyCommand>();
    registerCommand<AddVocabCommand>();
    registerCommand<DictationHistoryCommand>();
  }
};
