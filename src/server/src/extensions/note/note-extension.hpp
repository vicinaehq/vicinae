#pragma once
#include <QCoreApplication>
#include "command-database.hpp"
#include "qml/manage-notes-view-host.hpp"
#include "qml/note-form-view-host.hpp"
#include "single-view-command-context.hpp"
#include "ui/image/url.hpp"

namespace {
class CreateNoteCommand : public BuiltinViewCommand<NoteFormViewHost> {
  QString id() const override { return "create"; }
  QString name() const override { return QCoreApplication::translate("CreateNoteCommand", "Create Note"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::NewDocument).setBackgroundTint(SemanticColor::Yellow);
  }
};

class ManageNotesCommand : public BuiltinViewCommand<ManageNotesViewHost> {
  QString id() const override { return "manage"; }
  QString name() const override { return QCoreApplication::translate("ManageNotesCommand", "Manage Notes"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::BlankDocument).setBackgroundTint(SemanticColor::Yellow);
  }
};
} // namespace

// The `notes` provider id belongs to NoteRootProvider, which surfaces the notes themselves.
class NoteExtension : public BuiltinCommandRepository {
  QString id() const override { return "manage-notes"; }
  QString displayName() const override {
    return QCoreApplication::translate("NoteExtension", "Manage Notes");
  }
  QString description() const override {
    return QCoreApplication::translate("NoteExtension", "Local notes you can search, pin and paste");
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::BlankDocument).setBackgroundTint(SemanticColor::Yellow);
  }

public:
  NoteExtension() {
    registerCommand<CreateNoteCommand>();
    registerCommand<ManageNotesCommand>();
  }
};
