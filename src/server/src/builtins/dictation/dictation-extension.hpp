#pragma once
#include "builtins/dictation/dictation.hpp"
#include "builtins/dictation/dictation-command.hpp"
#include "builtins/dictation/dictation-history-view-host.hpp"
#include "builtins/dictation/styles-view-host.hpp"
#include "builtins/dictation/vocabulary-form-view-host.hpp"
#include "builtins/dictation/vocabulary-view-host.hpp"
#include <qtmetamacros.h>
#include <string_view>
#include <utility>
#include "builtins/dictation/dictation-preferences.hpp"
#include "command/command-database.hpp"
#include "command/typed-command.hpp"
#include "command/single-view-command-context.hpp"
#include "service-registry.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "services/dictation/dictation-service.hpp"
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

class DictationStylesCommand : public GuardedBuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(DictationStylesCommand)

  QString id() const override { return "styles"; }
  QString name() const override { return tr("Manage Dictation Styles"); }
  QString description() const override {
    return tr("Have AI rewrite what you dictate, with a different style for each app.");
  }
  ImageURL iconUrl() const override { return Dictation::STYLE_ICON; }
  std::vector<QString> keywords() const override { return {"rewrite", "tone", "prompt"}; }
  void execute(CommandController &controller) const override { Dictation::openStyles(controller.context()); }
};

class CreateDictationStyleCommand : public GuardedBuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(CreateDictationStyleCommand)

  QString id() const override { return "create-style"; }
  QString name() const override { return tr("Create Dictation Style"); }
  ImageURL iconUrl() const override { return ImageURL(Dictation::STYLE_ICON).setBadge(BuiltinIcon::Plus); }
  std::vector<QString> keywords() const override { return {"rewrite", "tone", "prompt"}; }
  void execute(CommandController &controller) const override {
    Dictation::openStyleForm(controller.context());
  }
};

}; // namespace

class DictationExtension : public TypedCommandRepository<DictationPreferences> {
  Q_DECLARE_TR_FUNCTIONS(DictationExtension)

  QString id() const override { return Dictation::qs(Dictation::REPOSITORY_ID); }
  QString displayName() const override { return tr("Dictation"); }
  ImageURL iconUrl() const override { return Dictation::ICON; }

  void preferencesChanged(const DictationPreferences &preferences) const override {
    TranscriptionSettings settings{.playSoundEffects = preferences.sound,
                                   .pauseMedia = preferences.pauseMedia};
    if (preferences.model != Dictation::NO_MODEL) {
      if (auto ref = AI::ModelRef::fromString(preferences.model)) settings.model = std::move(*ref);
    }
    if (preferences.language != Dictation::AUTO_LANGUAGE) settings.language = preferences.language;
    ServiceRegistry::instance()->dictation()->setSettings(std::move(settings));
  }

public:
  DictationExtension() {
    registerCommand<TranscribeCommand>();
    registerCommand<VocabularyCommand>();
    registerCommand<AddVocabCommand>();
    registerCommand<DictationHistoryCommand>();
    registerCommand<DictationStylesCommand>();
    registerCommand<CreateDictationStyleCommand>();
  }
};
