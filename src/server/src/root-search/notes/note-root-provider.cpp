#include "root-search/notes/note-root-provider.hpp"
#include "actions/note/note-actions.hpp"
#include "actions/root-search/root-search-actions.hpp"
#include "builtin_icon.hpp"
#include "service-registry.hpp"
#include "services/paste/paste-service.hpp"
#include "theme/colors.hpp"
#include "ui/image/url.hpp"

RootNoteItem::RootNoteItem(note::Note note) : m_note(std::move(note)) {}

QString RootNoteItem::title() const { return QString::fromStdString(m_note.title); }

double RootNoteItem::baseScoreWeight() const { return 1.4; }

AccessoryList RootNoteItem::accessories() const {
  AccessoryList accessories;

  accessories.reserve(2);

  if (m_note.pinned()) {
    accessories.emplace_back(ListAccessory{
        .tooltip = tr("Pinned"),
        .icon = ImageURL::builtin(BuiltinIcon::Pin).setFill(SemanticColor::TextMuted),
    });
  }

  accessories.emplace_back(ListAccessory{.text = tr("Note"), .color = SemanticColor::TextMuted});

  return accessories;
}

ImageURL RootNoteItem::iconUrl() const {
  return ImageURL(BuiltinIcon::BlankDocument).setBackgroundTint(SemanticColor::Yellow);
}

EntrypointId RootNoteItem::uniqueId() const { return EntrypointId{"notes", m_note.id}; }

QString RootNoteItem::typeDisplayName() const { return tr("Note"); }

std::unique_ptr<ActionPanelState> RootNoteItem::newActionPanel(ApplicationContext *ctx,
                                                               const RootItemMetadata &metadata) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto *main = panel->createSection();
  auto *manage = panel->createSection();
  auto *itemSection = panel->createSection();
  auto *danger = panel->createSection();

  panel->setTitle(title());

  if (ctx->services->pasteService()->supportsPaste()) {
    main->addAction(new note_actions::PasteNoteAction(m_note));
  }

  main->addAction(new note_actions::CopyNoteAction(m_note));

  manage->addAction(new note_actions::EditNoteAction(m_note));
  manage->addAction(new note_actions::TogglePinNoteAction(m_note));

  for (const auto action :
       RootSearchActionGenerator::generateActions(*this, *ctx->services->rootItemManager())) {
    itemSection->addAction(action);
  }

  danger->addAction(new note_actions::RemoveNoteAction(m_note));

  return panel;
}

std::vector<std::shared_ptr<RootItem>> NoteRootProvider::loadItems() const {
  const auto &notes = m_service.database()->notes();
  std::vector<std::shared_ptr<RootItem>> items;

  items.reserve(notes.size());

  for (const auto &note : notes) {
    items.emplace_back(std::make_shared<RootNoteItem>(note));
  }

  return items;
}

QString NoteRootProvider::displayName() const {
  return QCoreApplication::translate("NoteRootProvider", "Notes");
}

QString NoteRootProvider::uniqueId() const { return "notes"; }

ImageURL NoteRootProvider::icon() const {
  return ImageURL::builtin(BuiltinIcon::BlankDocument).setBackgroundTint(SemanticColor::Yellow);
}

RootProvider::Type NoteRootProvider::type() const { return RootProvider::Type::GroupProvider; }

NoteRootProvider::NoteRootProvider(NoteService &service) : m_service(service) {
  connect(&service, &NoteService::notesChanged, this, [this]() { emit itemsChanged(); });
}
