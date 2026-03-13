#include "clipboard-history-model.hpp"
#include "common/context.hpp"
#include "extensions/clipboard/history/clipboard-history-actions.hpp"
#include "internal/keyboard/keybind.hpp"
#include "navigation-controller.hpp"
#include "settings-controller/settings-controller.hpp"
#include "service-registry.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/paste/paste-service.hpp"
#include "ui/action-pannel/action.hpp"
#include "utils/utils.hpp"
#include <QCoreApplication>
#include <QDateTime>

void ClipboardHistorySection::setEntries(const PaginatedResponse<ClipboardHistoryEntry> &page) {
  m_entries = page.data;
  notifyChanged();
}

QString ClipboardHistorySection::itemId(int i) const { return m_entries[i].id; }

QString ClipboardHistorySection::itemTitle(int i) const { return m_entries[i].textPreview; }

QString ClipboardHistorySection::itemSubtitle(int i) const {
  auto dt = QDateTime::fromSecsSinceEpoch(m_entries[i].updatedAt);
  return getRelativeTimeString(dt);
}

std::optional<ImageURL> ClipboardHistorySection::itemIcon(int i) const { return iconForEntry(m_entries[i]); }

ImageURL ClipboardHistorySection::iconForEntry(const ClipboardHistoryEntry &entry) const {
  switch (entry.kind) {
  case ClipboardOfferKind::Image:
    return ImageURL::builtin(BuiltinIcon::Image);
  case ClipboardOfferKind::Link:
    if (entry.urlHost)
      return ImageURL::favicon(*entry.urlHost).withFallback(ImageURL::builtin(BuiltinIcon::Link));
    return ImageURL::builtin(BuiltinIcon::Link);
  case ClipboardOfferKind::Text:
    return ImageURL::builtin(BuiltinIcon::Text);
  case ClipboardOfferKind::File:
    return ImageURL::builtin(BuiltinIcon::Folder);
  default:
    return ImageURL::builtin(BuiltinIcon::QuestionMarkCircle);
  }
}

std::unique_ptr<ActionPanelState> ClipboardHistorySection::actionPanel(int i) const {
  const auto &entry = m_entries[i];
  auto panel = std::make_unique<ListActionPanelState>();
  auto clipman = scope().services()->clipman();
  auto mainSection = panel->createSection();
  bool const isCopyable = entry.encryption == ClipboardEncryptionType::None || clipman->isEncryptionReady();

  if (!isCopyable) {
    mainSection->addAction(
        new StaticAction(QCoreApplication::translate("ClipboardHistorySection", "Open Settings"),
                         BuiltinIcon::Cog, [](ApplicationContext *ctx) {
                           ctx->settings->openTab(QStringLiteral("advanced"));
                           ctx->navigation->closeWindow();
                         }));
  }

  auto pasteService = scope().services()->pasteService();
  auto pin = new PinClipboardAction(entry.id, !entry.pinnedAt);
  auto editKeywords = new EditClipboardKeywordsAction(entry.id);
  auto remove = new RemoveSelectionAction(entry.id);
  auto removeAll = new RemoveAllSelectionsAction();

  editKeywords->setShortcut(Keybind::EditAction);
  remove->setStyle(AbstractAction::Style::Danger);
  remove->setShortcut(Keybind::RemoveAction);
  removeAll->setShortcut(Keybind::DangerousRemoveAction);
  pin->setShortcut(Keybind::PinAction);

  if (isCopyable) {
    auto copy = new CopyClipboardSelection(entry.id);
    copy->addShortcut(Keybind::CopyAction);

    if (pasteService->supportsPaste()) {
      auto paste = new PasteClipboardSelection(entry.id);
      paste->addShortcut(Keybind::PasteAction);
      if (m_defaultAction == DefaultAction::Copy) {
        mainSection->addAction(copy);
        mainSection->addAction(paste);
      } else {
        mainSection->addAction(paste);
        mainSection->addAction(copy);
      }
    } else {
      mainSection->addAction(copy);
    }
  }

  auto toolsSection = panel->createSection();
  auto dangerSection = panel->createSection();
  toolsSection->addAction(pin);
  if (isCopyable) {
    auto saveToFile = new SaveClipboardSelectionToFileAction(entry, m_saveDirectoryMode,
                                                             m_saveCustomDirectory, m_saveFileNameMode);
    saveToFile->setShortcut(Keybind::SaveAction);
    toolsSection->addAction(saveToFile);
  }
  toolsSection->addAction(editKeywords);
  dangerSection->addAction(remove);
  dangerSection->addAction(removeAll);

  return panel;
}
