#pragma once

#include <QCoreApplication>
#include "command/command-controller.hpp"
#include "command/command-database.hpp"
#include "command/single-view-command-context.hpp"
#include "command/typed-command.hpp"
#include "notes-view-host.hpp"
#include "vicinae.hpp"

class CreateNoteCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(CreateNoteCommand)
  QString id() const override { return QStringLiteral("create"); }
  QString name() const override { return tr("Create Note"); }
  QString description() const override { return tr("Write a Markdown note in a dedicated window."); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Feather)
        .setBackgroundTint(SemanticColor::Yellow)
        .setBadge(BuiltinIcon::Plus);
  }
  std::vector<QString> keywords() const override { return {"new", "markdown", "write"}; }
  void execute(CommandController &controller) const override {
    auto *ctx = controller.context();
    ctx->navigation->closeWindow();
    ctx->notes->newNote();
  }
};

class SearchNotesCommand : public BuiltinViewCommand<NotesViewHost> {
  Q_DECLARE_TR_FUNCTIONS(SearchNotesCommand)
  QString id() const override { return QStringLiteral("search"); }
  QString name() const override { return tr("Search Notes"); }
  QString description() const override { return tr("Find, edit, and manage your notes."); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Feather)
        .setBackgroundTint(SemanticColor::Yellow)
        .setBadge(BuiltinIcon::MagnifyingGlass);
  }
  std::vector<QString> keywords() const override { return {"markdown", "browse", "find"}; }
};

class NotesExtension : public TypedCommandRepository<Notes::Preferences> {
  Q_DECLARE_TR_FUNCTIONS(NotesExtension)
  QString id() const override { return QStringLiteral("notes"); }
  QString displayName() const override { return tr("Notes"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Feather).setBackgroundTint(SemanticColor::Yellow);
  }

public:
  std::vector<Preference> preferences() const override {
    return describePreferences<Notes::Preferences>(
        {.defaultDirectory = (Omnicast::dataDir() / "notes").string()});
  }
  void initialized(const Notes::Preferences &preferences) const override { preferencesChanged(preferences); }
  void preferencesChanged(const Notes::Preferences &preferences) const override {
    ServiceRegistry::instance()->notes()->configure(preferences);
  }

  NotesExtension() {
    registerCommand<CreateNoteCommand>();
    registerCommand<SearchNotesCommand>();
  }
};
