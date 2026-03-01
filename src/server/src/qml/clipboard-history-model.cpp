#include "clipboard-history-model.hpp"
#include "actions/root-search/root-search-actions.hpp"
#include "extensions/clipboard/history/clipboard-history-actions.hpp"
#include "lib/keyboard/keybind.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/window-manager/window-manager.hpp"
#include "utils/utils.hpp"
#include <QDateTime>
#include <utility>

ClipboardHistoryModel::ClipboardHistoryModel(QObject *parent) : CommandListModel(parent) {}

void ClipboardHistoryModel::setEntries(const PaginatedResponse<ClipboardHistoryEntry> &page) {
  m_entries = page.data;
  std::vector<SectionInfo> sections;
  if (!m_entries.empty()) {
    sections.push_back({.name = QString(), .count = static_cast<int>(m_entries.size())});
  }
  bool incremental = !selectFirstOnReset();
  setSections(sections);
  setSelectFirstOnReset(false);
  if (incremental) refreshActionPanel();
}

QHash<int, QByteArray> ClipboardHistoryModel::roleNames() const {
  auto roles = CommandListModel::roleNames();
  roles[IsPinned] = "isPinned";
  return roles;
}

QVariant ClipboardHistoryModel::data(const QModelIndex &index, int role) const {
  if (role == IsPinned) {
    int s, i;
    if (!dataItemAt(index.row(), s, i)) return false;
    return m_entries[i].pinnedAt != 0;
  }
  return CommandListModel::data(index, role);
}

QString ClipboardHistoryModel::itemId(int, int i) const { return m_entries[i].id; }

QString ClipboardHistoryModel::itemTitle(int, int i) const { return m_entries[i].textPreview; }

QString ClipboardHistoryModel::itemSubtitle(int, int i) const {
  auto dt = QDateTime::fromSecsSinceEpoch(m_entries[i].updatedAt);
  return getRelativeTimeString(dt);
}

QString ClipboardHistoryModel::itemIconSource(int, int i) const {
  return imageSourceFor(iconForEntry(m_entries[i]));
}

ImageURL ClipboardHistoryModel::iconForEntry(const ClipboardHistoryEntry &entry) const {
  switch (entry.kind) {
  case ClipboardOfferKind::Image:
    return ImageURL::builtin("image");
  case ClipboardOfferKind::Link:
    if (entry.urlHost) return ImageURL::favicon(*entry.urlHost).withFallback(ImageURL::builtin("link"));
    return ImageURL::builtin("link");
  case ClipboardOfferKind::Text:
    return ImageURL::builtin("text");
  case ClipboardOfferKind::File:
    return ImageURL::builtin("folder");
  default:
    return ImageURL::builtin("question-mark-circle");
  }
}

void ClipboardHistoryModel::onItemSelected(int, int i) {
  if (i >= 0 && std::cmp_less(i, m_entries.size())) { emit entrySelected(m_entries[i]); }
}

std::unique_ptr<ActionPanelState> ClipboardHistoryModel::createActionPanel(int, int i) const {
  const auto &entry = m_entries[i];
  auto panel = std::make_unique<ListActionPanelState>();
  auto clipman = scope().services()->clipman();
  auto mainSection = panel->createSection();
  bool const isCopyable = entry.encryption == ClipboardEncryptionType::None || clipman->isEncryptionReady();

  if (!isCopyable) { mainSection->addAction(new OpenItemPreferencesAction(EntrypointId{"clipboard", ""})); }

  auto wm = scope().services()->windowManager();
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

    if (wm->canPaste()) {
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
  toolsSection->addAction(editKeywords);
  dangerSection->addAction(remove);
  dangerSection->addAction(removeAll);

  return panel;
}
