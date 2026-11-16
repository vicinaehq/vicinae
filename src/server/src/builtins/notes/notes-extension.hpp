#pragma once

#include <QCoreApplication>
#include "command/command-controller.hpp"
#include "command/command-database.hpp"
#include "command/single-view-command-context.hpp"
#include "command/typed-command.hpp"
#include "notes-view-host.hpp"

class CreateNoteCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(CreateNoteCommand)
  QString id() const override { return QStringLiteral("create"); }
  QString name() const override { return tr("Create Note"); }
  QString description() const override { return tr("Write a Markdown note in a dedicated window."); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Pencil).setBackgroundTint(SemanticColor::Yellow);
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
    return ImageURL::builtin(BuiltinIcon::BookAntique).setBackgroundTint(SemanticColor::Yellow);
  }
  std::vector<QString> keywords() const override { return {"markdown", "browse", "find"}; }
};

class NotesExtension : public BuiltinCommandRepository {
  Q_DECLARE_TR_FUNCTIONS(NotesExtension)
  QString id() const override { return QStringLiteral("notes"); }
  QString displayName() const override { return tr("Notes"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::BookAntique).setBackgroundTint(SemanticColor::Yellow);
  }

public:
  NotesExtension() {
    registerCommand<CreateNoteCommand>();
    registerCommand<SearchNotesCommand>();
  }
};
