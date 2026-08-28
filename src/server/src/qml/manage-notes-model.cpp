#include "manage-notes-model.hpp"
#include "actions/note/note-actions.hpp"
#include "builtin_icon.hpp"
#include "service-registry.hpp"
#include "services/paste/paste-service.hpp"
#include "theme/colors.hpp"

QString ManageNotesSection::displayTitle(const note::Note &item) const {
  return QString::fromStdString(item.title);
}

std::optional<ImageURL> ManageNotesSection::displayIcon(const note::Note &) const {
  return ImageURL(BuiltinIcon::BlankDocument);
}

AccessoryList ManageNotesSection::displayAccessories(const note::Note &item) const {
  if (!item.pinned()) return {};

  return {{.tooltip = tr("Pinned"),
           .icon = ImageURL::builtin(BuiltinIcon::Pin).setFill(SemanticColor::TextMuted)}};
}

std::unique_ptr<ActionPanelState> ManageNotesSection::buildActionPanel(const note::Note &item) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto *main = panel->createSection();
  auto *manage = panel->createSection();
  auto *danger = panel->createSection();

  if (scope().services()->pasteService()->supportsPaste()) {
    main->addAction(new note_actions::PasteNoteAction(item));
  }

  main->addAction(new note_actions::CopyNoteAction(item));

  manage->addAction(new note_actions::EditNoteAction(item));
  manage->addAction(new note_actions::TogglePinNoteAction(item));

  danger->addAction(new note_actions::RemoveNoteAction(item));

  return panel;
}
