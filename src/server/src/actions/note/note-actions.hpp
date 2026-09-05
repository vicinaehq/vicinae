#pragma once
#include <QCoreApplication>
#include "builtin_icon.hpp"
#include "keyboard/keybind.hpp"
#include "navigation-controller.hpp"
#include "qml/note-form-view-host.hpp"
#include "service-registry.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/note/note-db.hpp"
#include "services/note/note-service.hpp"
#include "services/paste/paste-service.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/image/url.hpp"

/**
 * Actions shared by the "Manage Notes" list and the notes root search provider.
 */
namespace note_actions {

class CopyNoteAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(CopyNoteAction)

  note::Note m_note;

public:
  void execute(ApplicationContext *ctx) override {
    if (!ctx->services->clipman()->copyText(QString::fromStdString(m_note.body), {.transient = true})) {
      ctx->services->toastService()->failure(tr("Failed to copy note"));
      return;
    }

    ctx->services->noteService()->registerUse(m_note.id);
    ctx->navigation->showHud(tr("Copied to clipboard"), ImageURL::builtin(BuiltinIcon::CopyClipboard));
  }

  explicit CopyNoteAction(note::Note note)
      : AbstractAction(tr("Copy note"), ImageURL::builtin(BuiltinIcon::CopyClipboard)),
        m_note(std::move(note)) {
    setShortcut(Keybind::CopyAction);
  }
};

class PasteNoteAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(PasteNoteAction)

  note::Note m_note;

public:
  void execute(ApplicationContext *ctx) override {
    ctx->services->pasteService()->pasteContent(Clipboard::Text{QString::fromStdString(m_note.body)},
                                                {.transient = true});
    ctx->services->noteService()->registerUse(m_note.id);
    ctx->navigation->closeWindow();
  }

  explicit PasteNoteAction(note::Note note)
      : AbstractAction(tr("Paste to active window"), ImageURL::builtin(BuiltinIcon::CopyClipboard)),
        m_note(std::move(note)) {
    setShortcut(Keybind::PasteAction);
  }
};

class EditNoteAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(EditNoteAction)

  note::Note m_note;

public:
  void execute(ApplicationContext *ctx) override {
    ctx->navigation->pushView(new NoteFormViewHost(m_note, NoteFormViewHost::Mode::Edit));
  }

  explicit EditNoteAction(note::Note note)
      : AbstractAction(tr("Edit note"), ImageURL::builtin(BuiltinIcon::Pencil)), m_note(std::move(note)) {
    setShortcut(Keybind::EditAction);
  }
};

class TogglePinNoteAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(TogglePinNoteAction)

  note::Note m_note;

public:
  void execute(ApplicationContext *ctx) override {
    const bool pin = !m_note.pinned();

    auto *toast = ctx->services->toastService();

    if (const auto res = ctx->services->noteService()->setPinned(m_note.id, pin); !res) {
      toast->failure(res.error().c_str());
      return;
    }

    toast->setToast(pin ? tr("Pinned note") : tr("Unpinned note"));
  }

  explicit TogglePinNoteAction(note::Note note)
      : AbstractAction(note.pinned() ? tr("Unpin note") : tr("Pin note"),
                       ImageURL::builtin(note.pinned() ? BuiltinIcon::PinDisabled : BuiltinIcon::Pin)),
        m_note(std::move(note)) {
    setShortcut(Keybind::PinAction);
  }
};

class RemoveNoteAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(RemoveNoteAction)

  note::Note m_note;

public:
  void execute(ApplicationContext *ctx) override {
    auto *toast = ctx->services->toastService();

    if (const auto res = ctx->services->noteService()->removeNote(m_note.id); !res) {
      toast->failure(res.error().c_str());
      return;
    }

    toast->setToast(tr("Removed note"));
  }

  explicit RemoveNoteAction(note::Note note)
      : AbstractAction(tr("Remove note"), ImageURL::builtin(BuiltinIcon::Trash)), m_note(std::move(note)) {
    setStyle(AbstractAction::Style::Danger);
    setShortcut(Keybind::DangerousRemoveAction);
  }
};

} // namespace note_actions
