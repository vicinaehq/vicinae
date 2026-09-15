#pragma once
#include "builtins/dictation/dictation.hpp"
#include "builtins/dictation/dictation-command.hpp"
#include "builtins/dictation/dictation-history-view-host.hpp"
#include <qtmetamacros.h>
#include <string_view>
#include <utility>
#include "command/command-database.hpp"
#include "command/single-view-command-context.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "services/toast/toast-service.hpp"
#include "theme/colors.hpp"
#include "ui/image/url.hpp"

namespace {

class UnimplementedCommand : public BuiltinCallbackCommand {
  void execute(CommandController &controller) const override {
    controller.context()->services->toastService()->failure("Not implemented");
  }
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

class DictationHistoryCommand : public BuiltinViewCommand<DictationHistoryViewHost> {
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
    registerCommand<VocabularyCommand>();
    registerCommand<AddVocabCommand>();
    registerCommand<DictationHistoryCommand>();
  }
};
