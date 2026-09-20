#pragma once
#include "builtins/dictation/dictation.hpp"
#include "builtins/dictation/dictation-command.hpp"
#include "builtins/dictation/dictation-history-view-host.hpp"
#include "builtins/dictation/vocabulary-form-view-host.hpp"
#include "builtins/dictation/vocabulary-view-host.hpp"
#include <qtmetamacros.h>
#include <string_view>
#include <utility>
#include "builtins/dictation/dictation-preferences.hpp"
#include "command/command-database.hpp"
#include "command/typed-command.hpp"
#include "command/single-view-command-context.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "theme/colors.hpp"
#include "ui/image/url.hpp"

namespace {

class AddVocabCommand : public BuiltinViewCommand<VocabularyFormViewHost> {
  Q_DECLARE_TR_FUNCTIONS(AddVocabCommand)

  QString id() const override { return "add-vocab"; }
  QString name() const override { return tr("Add Word to Vocabulary"); }
  ImageURL iconUrl() const override {
    return ImageURL(Dictation::VOCABULARY_ICON).setBadge(BuiltinIcon::Plus);
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

class VocabularyCommand : public BuiltinViewCommand<VocabularyViewHost> {
  Q_DECLARE_TR_FUNCTIONS(VocabularyCommand)

  QString id() const override { return "vocab"; }
  QString name() const override { return tr("Vocabulary"); }
  ImageURL iconUrl() const override { return Dictation::VOCABULARY_ICON; }
  std::vector<QString> keywords() const override { return {}; }
};

}; // namespace

class DictationExtension : public TypedCommandRepository<DictationPreferences> {
  Q_DECLARE_TR_FUNCTIONS(DictationExtension)

  QString id() const override { return Dictation::qs(Dictation::REPOSITORY_ID); }
  QString displayName() const override { return tr("Dictation"); }
  ImageURL iconUrl() const override { return Dictation::ICON; }

public:
  DictationExtension() {
    registerCommand<TranscribeCommand>();
    registerCommand<VocabularyCommand>();
    registerCommand<AddVocabCommand>();
    registerCommand<DictationHistoryCommand>();
  }
};
