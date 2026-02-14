#include "qml-clipboard-history-model.hpp"
#include "actions/root-search/root-search-actions.hpp"
#include "builtin_icon.hpp"
#include "clipboard-actions.hpp"
#include "lib/keyboard/keybind.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/toast/toast-service.hpp"
#include "services/window-manager/window-manager.hpp"
#include "ui/action-pannel/push-action.hpp"
#include "ui/alert/alert.hpp"
#include "ui/form/text-area.hpp"
#include "ui/views/form-view.hpp"
#include "utils/utils.hpp"
#include <QDateTime>

// -- Action classes (ported from clipboard-history-view.cpp) --

namespace {

class PasteClipboardSelection : public PasteToFocusedWindowAction {
  QString m_id;
  void execute(ApplicationContext *ctx) override {
    setConcealed();
    loadClipboardData(Clipboard::SelectionRecordHandle(m_id));
    PasteToFocusedWindowAction::execute(ctx);
  }

public:
  PasteClipboardSelection(const QString &id) : PasteToFocusedWindowAction(), m_id(id) {}
};

class CopyClipboardSelection : public AbstractAction {
  QString m_id;
  void execute(ApplicationContext *ctx) override {
    auto clipman = ctx->services->clipman();
    auto toast = ctx->services->toastService();
    if (clipman->copySelectionRecord(m_id, {.concealed = true})) {
      ctx->navigation->showHud("Selection copied to clipboard");
      return;
    }
    toast->failure("Failed to copy to clipboard");
  }

public:
  CopyClipboardSelection(const QString &id)
      : AbstractAction("Copy to clipboard", BuiltinIcon::CopyClipboard), m_id(id) {}
};

class RemoveSelectionAction : public AbstractAction {
  QString m_id;
  void execute(ApplicationContext *ctx) override {
    auto clipman = ctx->services->clipman();
    auto toast = ctx->services->toastService();
    if (clipman->removeSelection(m_id)) {
      toast->setToast("Entry removed");
    } else {
      toast->setToast("Failed to remove entry", ToastStyle::Danger);
    }
  }

public:
  RemoveSelectionAction(const QString &id)
      : AbstractAction("Remove entry", ImageURL::builtin("trash")), m_id(id) {
    setStyle(AbstractAction::Style::Danger);
  }
};

class PinClipboardAction : public AbstractAction {
  QString m_id;
  bool m_value;
  void execute(ApplicationContext *ctx) override {
    QString action = m_value ? "pinned" : "unpinned";
    if (ctx->services->clipman()->setPinned(m_id, m_value)) {
      ctx->services->toastService()->success(QString("Selection %1").arg(action));
    } else {
      ctx->services->toastService()->failure("Failed to change pin status");
    }
  }

public:
  PinClipboardAction(const QString &id, bool value)
      : AbstractAction(value ? "Pin" : "Unpin", ImageURL::builtin("pin")), m_id(id), m_value(value) {}
};

class EditClipboardSelectionKeywordsView : public ManagedFormView {
  TextArea *m_keywords = new TextArea;
  QString m_selectionId;

  void onSubmit() override {
    auto clipman = context()->services->clipman();
    auto toast = context()->services->toastService();
    if (clipman->setKeywords(m_selectionId, m_keywords->text())) {
      toast->setToast("Keywords edited", ToastStyle::Success);
      popSelf();
    } else {
      toast->setToast("Failed to edit keywords", ToastStyle::Danger);
    }
  }

  void initializeForm() override {
    auto clipman = context()->services->clipman();
    m_keywords->setText(clipman->retrieveKeywords(m_selectionId).value_or(""));
    m_keywords->textEdit()->selectAll();
  }

public:
  EditClipboardSelectionKeywordsView(const QString &id) : m_selectionId(id) {
    auto inputField = new FormField();
    inputField->setWidget(m_keywords);
    inputField->setName("Keywords");
    inputField->setInfo("Additional keywords that will be used to index this selection.");
    form()->addField(inputField);
  }
};

class EditClipboardKeywordsAction : public PushAction<EditClipboardSelectionKeywordsView, QString> {
  QString title() const override { return "Edit keywords"; }
  std::optional<ImageURL> icon() const override { return ImageURL::builtin("text"); }

public:
  EditClipboardKeywordsAction(const QString &id) : PushAction(id) {}
};

class RemoveAllSelectionsAction : public AbstractAction {
  void execute(ApplicationContext *ctx) override {
    auto alert = new CallbackAlertWidget();
    alert->setTitle("Are you sure?");
    alert->setMessage("All your clipboard history will be lost forever");
    alert->setConfirmText("Delete all", SemanticColor::Red);
    alert->setConfirmCallback([ctx]() {
      auto toast = ctx->services->toastService();
      auto clipman = ctx->services->clipman();
      if (clipman->removeAllSelections()) {
        toast->success("All selections were removed");
      } else {
        toast->failure("Failed to remove all selections");
      }
    });
    ctx->navigation->setDialog(alert);
  }

public:
  QString title() const override { return "Remove all"; }
  std::optional<ImageURL> icon() const override { return ImageURL::builtin("trash"); }
  RemoveAllSelectionsAction() { setStyle(AbstractAction::Style::Danger); }
};

} // namespace

// -- Model implementation --

QmlClipboardHistoryModel::QmlClipboardHistoryModel(QObject *parent) : QmlCommandListModel(parent) {}

void QmlClipboardHistoryModel::setEntries(const PaginatedResponse<ClipboardHistoryEntry> &page) {
  m_entries = page.data;
  std::vector<SectionInfo> sections;
  if (!m_entries.empty()) {
    sections.push_back({.name = QString(), .count = static_cast<int>(m_entries.size())});
  }
  setSections(sections);
}

QHash<int, QByteArray> QmlClipboardHistoryModel::roleNames() const {
  auto roles = QmlCommandListModel::roleNames();
  roles[IsPinned] = "isPinned";
  return roles;
}

QVariant QmlClipboardHistoryModel::data(const QModelIndex &index, int role) const {
  if (role == IsPinned) {
    int s, i;
    if (!dataItemAt(index.row(), s, i)) return false;
    return m_entries[i].pinnedAt != 0;
  }
  return QmlCommandListModel::data(index, role);
}

QString QmlClipboardHistoryModel::itemTitle(int, int i) const { return m_entries[i].textPreview; }

QString QmlClipboardHistoryModel::itemSubtitle(int, int i) const {
  auto dt = QDateTime::fromSecsSinceEpoch(m_entries[i].updatedAt);
  return getRelativeTimeString(dt);
}

QString QmlClipboardHistoryModel::itemIconSource(int, int i) const {
  return imageSourceFor(iconForEntry(m_entries[i]));
}

ImageURL QmlClipboardHistoryModel::iconForEntry(const ClipboardHistoryEntry &entry) const {
  switch (entry.kind) {
  case ClipboardOfferKind::Image:
    return ImageURL::builtin("image");
  case ClipboardOfferKind::Link:
    if (entry.urlHost)
      return ImageURL::favicon(*entry.urlHost).withFallback(ImageURL::builtin("link"));
    return ImageURL::builtin("link");
  case ClipboardOfferKind::Text:
    return ImageURL::builtin("text");
  case ClipboardOfferKind::File:
    return ImageURL::builtin("folder");
  default:
    return ImageURL::builtin("question-mark-circle");
  }
}

void QmlClipboardHistoryModel::onItemSelected(int, int i) {
  if (i >= 0 && i < static_cast<int>(m_entries.size())) {
    emit entrySelected(m_entries[i]);
  }
}

std::unique_ptr<ActionPanelState> QmlClipboardHistoryModel::createActionPanel(int, int i) const {
  const auto &entry = m_entries[i];
  auto panel = std::make_unique<ListActionPanelState>();
  auto clipman = ctx()->services->clipman();
  auto mainSection = panel->createSection();
  bool isCopyable = entry.encryption == ClipboardEncryptionType::None || clipman->isEncryptionReady();

  if (!isCopyable) {
    mainSection->addAction(new OpenItemPreferencesAction(EntrypointId{"clipboard", ""}));
  }

  auto wm = ctx()->services->windowManager();
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
