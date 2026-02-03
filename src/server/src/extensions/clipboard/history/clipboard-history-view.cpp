#include "clipboard-history-view.hpp"
#include "actions/root-search/root-search-actions.hpp"
#include "builtin_icon.hpp"
#include "clipboard-actions.hpp"
#include "common.hpp"
#include "extensions/clipboard/history/clipboard-history-model.hpp"
#include "navigation-controller.hpp"
#include "services/clipboard/clipboard-db.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/toast/toast-service.hpp"
#include "layout.hpp"
#include "theme.hpp"
#include "ui/detail/detail-widget.hpp"
#include "ui/empty-view/empty-view.hpp"
#include "ui/text-file-viewer/text-file-viewer.hpp"
#include "ui/action-pannel/push-action.hpp"
#include "ui/alert/alert.hpp"
#include "ui/form/text-area.hpp"
#include "ui/views/form-view.hpp"
#include "utils.hpp"
#include <qmimedata.h>
#include <qmimedatabase.h>
#include <qmimetype.h>
#include <qnamespace.h>
#include <qwidget.h>
#include <filesystem>
#include <system_error>

namespace fs = std::filesystem;

class TextContainer : public QWidget {
  QVBoxLayout *_layout;

public:
  void setWidget(QWidget *widget) { _layout->addWidget(widget); }

  TextContainer() {
    _layout = new QVBoxLayout;
    setLayout(_layout);
  }
};

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

class ClipboardHistoryDetail : public DetailWidget {
  QTemporaryFile m_tmpFile;

  std::vector<MetadataItem> createEntryMetadata(const ClipboardHistoryEntry &entry) const {
    auto mime = MetadataLabel{
        .text = entry.mimeType,
        .title = "Mime",
    };

    if (entry.encryption != ClipboardEncryptionType::None) {
      mime.icon = ImageURL::builtin("key").setFill(SemanticColor::Green);
    }

    auto size = MetadataLabel{
        .text = formatSize(entry.size),
        .title = "Size",
    };
    auto copiedAt = MetadataLabel{
        .text = QDateTime::fromSecsSinceEpoch(entry.updatedAt).toString(),
        .title = "Copied at",
    };
    auto checksum = MetadataLabel{
        .text = entry.md5sum,
        .title = "MD5",
    };

    return {mime, size, copiedAt, checksum};
  }

  QWidget *detailForFilePath(const fs::path &path) {
    auto mime = QMimeDatabase().mimeTypeForFile(path.c_str());

    if (mime.name().startsWith("image/")) {
      auto icon = new ImageWidget;
      icon->setContentsMargins(10, 10, 10, 10);
      icon->setUrl(ImageURL::local(path));
      return icon;
    }

    if (Utils::isTextMimeType(mime)) {
      auto viewer = new TextFileViewer;
      viewer->load(path);
      return VStack().add(viewer).buildWidget();
    }

    return detailForUnmatchedMime(mime);
  }

  QWidget *detailForUnmatchedMime(const QMimeType &mime) {
    auto icon = new ImageWidget;
    icon->setUrl(ImageURL::system(mime.genericIconName()));
    return icon;
  }

  QWidget *detailForMime(const QByteArray &data, const QString &mimeName) {
    QMimeType mime = QMimeDatabase().mimeTypeForName(Utils::normalizeMimeName(mimeName));

    if (mimeName == "text/uri-list") {
      QString text(data);
      auto paths = text.split("\r\n", Qt::SkipEmptyParts);

      if (paths.size() == 1) {
        QUrl url(paths.at(0));

        if (url.scheme() == "file") {
          std::error_code ec;
          fs::path path = url.path().toStdString();
          if (fs::is_regular_file(path, ec)) { return detailForFilePath(path); }
        }
      }
    }

    if (mimeName.startsWith("image/")) {
      if (!m_tmpFile.open()) {
        qWarning() << "Failed to open file";
        return detailForUnmatchedMime(mime);
      }

      m_tmpFile.write(data);
      m_tmpFile.close();
      auto icon = new ImageWidget;
      icon->setContentsMargins(10, 10, 10, 10);
      icon->setUrl(ImageURL::local(m_tmpFile.filesystemFileName()));
      return icon;
    }

    if (Utils::isTextMimeType(mimeName) || mimeName == "text/uri-list") {
      auto viewer = new TextFileViewer;
      viewer->load(data);
      return VStack().add(viewer).buildWidget();
    }

    return detailForUnmatchedMime(mime);
  }

  QWidget *detailForFailedDecryption() {
    auto empty = new EmptyViewWidget;
    empty->setIcon(ImageURL::builtin("key").setFill(SemanticColor::Red));
    empty->setTitle("Decryption failed");
    empty->setDescription(
        "Vicinae could not decrypt the data for this selection. This is most likely caused by a "
        "keychain software change. To fix this disable encryption in the clipboard extension settings.");

    return empty;
  }

  QWidget *detailForMissingEncryption() {
    auto empty = new EmptyViewWidget;
    empty->setIcon(ImageURL::builtin("key").setFill(SemanticColor::Orange));
    empty->setTitle("Data is encrypted");
    empty->setDescription(
        "Data for this selection was previously encrypted but the clipboard is not currently "
        "configured to use encryption. You should be able to fix this by enabling it in the clipboard "
        "extension settings.");

    return empty;
  }

  QWidget *detailForError(ClipboardService::OfferDecryptionError error) {
    switch (error) {
    case ClipboardService::OfferDecryptionError::DecryptionRequired:
      return detailForMissingEncryption();
    case ClipboardService::OfferDecryptionError::DecryptionFailed:
      return detailForFailedDecryption();
    }
    return nullptr;
  }

  QWidget *createEntryWidget(const ClipboardHistoryEntry &entry) {
    auto clipman = ServiceRegistry::instance()->clipman();
    auto data = clipman->getMainOfferData(entry.id);

    if (!data) { return detailForError(data.error()); }
    return detailForMime(data.value(), entry.mimeType);
  }

public:
  void setEntry(const ClipboardHistoryEntry &entry) {
    if (auto previous = content()) { previous->deleteLater(); }

    auto widget = createEntryWidget(entry);
    auto metadata = createEntryMetadata(entry);

    setContent(widget);
    setMetadata(metadata);
  }
};

class RemoveSelectionAction : public AbstractAction {
  QString _id;

  void execute(ApplicationContext *ctx) override {
    auto clipman = ctx->services->clipman();
    auto toast = ctx->services->toastService();

    if (clipman->removeSelection(_id)) {
      toast->setToast("Entry removed");
    } else {
      toast->setToast("Failed to remove entry", ToastStyle::Danger);
    }
  }

public:
  RemoveSelectionAction(const QString &id)
      : AbstractAction("Remove entry", ImageURL::builtin("trash")), _id(id) {
    setStyle(AbstractAction::Style::Danger);
  }
};

class PinClipboardAction : public AbstractAction {
  QString _id;
  bool _value;

  void execute(ApplicationContext *ctx) override {
    QString action = _value ? "pinned" : "unpinned";

    if (ctx->services->clipman()->setPinned(_id, _value)) {
      ctx->services->toastService()->success(QString("Selection %1").arg(action));
    } else {
      ctx->services->toastService()->failure("Failed to change pin status");
    }
  }

public:
  QString entryId() const { return _id; }

  PinClipboardAction(const QString &id, bool value)
      : AbstractAction(value ? "Pin" : "Unpin", ImageURL::builtin("pin")), _id(id), _value(value) {}
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

static const std::vector<Preference::DropdownData::Option> filterSelectorOptions = {
    {"All", "all"}, {"Text", "text"}, {"Images", "image"}, {"Links", "link"}, {"Files", "file"},
};

static const std::unordered_map<QString, ClipboardOfferKind> typeToOfferKind{
    {"image", ClipboardOfferKind::Image},
    {"link", ClipboardOfferKind::Link},
    {"text", ClipboardOfferKind::Text},
    {"file", ClipboardOfferKind::File},
};

QWidget *ClipboardHistoryView::wrapUI(QWidget *content) {
  return VStack().add(m_statusToolbar).add(content, 1).divided(1).buildWidget();
}

ClipboardHistoryView::ClipboardHistoryView() {
  auto clipman = ServiceRegistry::instance()->clipman();

  m_statusToolbar = new ClipboardStatusToolbar;

  if (!clipman->supportsMonitoring()) {
    m_statusToolbar->setClipboardStatus(ClipboardStatusToolbar::ClipboardStatus::Unavailable);
  } else {
    handleMonitoringChanged(clipman->monitoring());
  }

  m_filterInput->setMinimumWidth(200);
  m_filterInput->setFocusPolicy(Qt::NoFocus);
  m_filterInput->setOptions(filterSelectorOptions);

  connect(clipman, &ClipboardService::monitoringChanged, this,
          &ClipboardHistoryView::handleMonitoringChanged);
  connect(m_statusToolbar, &ClipboardStatusToolbar::statusIconClicked, this,
          &ClipboardHistoryView::handleStatusClipboard);
  connect(m_filterInput, &SelectorInput::selectionChanged, this, &ClipboardHistoryView::handleFilterChange);
}

void ClipboardHistoryView::initialize() {
  TypedListView::initialize();
  auto preferences = command()->preferenceValues();

  m_model = new ClipboardHistoryModel(this);
  m_controller = new ClipboardHistoryController(context()->services->clipman(), m_model, this);
  setLoading(true);
  setModel(m_model);
  m_defaultAction = parseDefaultAction(preferences.value("defaultAction").toString());
  setSearchPlaceholderText("Browse clipboard history...");
  m_statusToolbar->setLeftText("Loading...");
  textChanged("");
  m_filterInput->setValue(getSavedDropdownFilter().value_or("all"));
  handleFilterChange(*m_filterInput->value());

  connect(m_model, &ClipboardHistoryModel::dataChanged, this, [this]() { refreshCurrent(); });
  connect(m_controller, &ClipboardHistoryController::dataLoadingChanged, this, &BaseView::setLoading);
  connect(m_controller, &ClipboardHistoryController::dataRetrieved, this,
          [this](const PaginatedResponse<ClipboardHistoryEntry> &page) {
            m_statusToolbar->setLeftText(QString("%1 Items").arg(page.totalCount));
          });
}

std::unique_ptr<ActionPanelState> ClipboardHistoryView::createActionPanel(const ItemType &info) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto clipman = context()->services->clipman();
  auto mainSection = panel->createSection();
  bool isCopyable = info->encryption == ClipboardEncryptionType::None || clipman->isEncryptionReady();

  if (!isCopyable) { mainSection->addAction(new OpenItemPreferencesAction(EntrypointId{"clipboard", ""})); }

  auto wm = context()->services->windowManager();
  auto pin = new PinClipboardAction(info->id, !info->pinnedAt);
  auto editKeywords = new EditClipboardKeywordsAction(info->id);
  auto remove = new RemoveSelectionAction(info->id);
  auto removeAll = new RemoveAllSelectionsAction();

  editKeywords->setShortcut(Keybind::EditAction);
  remove->setStyle(AbstractAction::Style::Danger);
  remove->setShortcut(Keybind::RemoveAction);
  removeAll->setShortcut(Keybind::DangerousRemoveAction);
  pin->setShortcut(Keybind::PinAction);

  if (isCopyable) {
    auto copy = new CopyClipboardSelection(info->id);
    copy->addShortcut(Keybind::CopyAction);

    if (wm->canPaste()) {
      auto paste = new PasteClipboardSelection(info->id);
      paste->addShortcut(Keybind::PasteAction);
      if (m_defaultAction == ClipboardHistoryView::DefaultAction::Copy) {
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

QWidget *ClipboardHistoryView::generateDetail(const ItemType &item) const {
  auto detail = new ClipboardHistoryDetail;
  detail->setEntry(*item);
  return detail;
}

void ClipboardHistoryView::textChanged(const QString &value) {
  m_controller->setFilter(value);
  m_list->selectFirst();
}

void ClipboardHistoryView::handleMonitoringChanged(bool monitor) {
  if (monitor) {
    m_statusToolbar->setClipboardStatus(ClipboardStatusToolbar::ClipboardStatus::Monitoring);
    return;
  }

  m_statusToolbar->setClipboardStatus(ClipboardStatusToolbar::ClipboardStatus::Paused);
}

void ClipboardHistoryView::handleStatusClipboard() {
  QJsonObject patch;

  if (m_statusToolbar->clipboardStatus() == ClipboardStatusToolbar::Paused) {
    patch["monitoring"] = true;
  } else {
    patch["monitoring"] = false;
  }

  command()->setPreferenceValues(patch);
}

void ClipboardHistoryView::handleFilterChange(const SelectorInput::AbstractItem &item) {
  saveDropdownFilter(item.id());

  if (auto it = typeToOfferKind.find(item.id()); it != typeToOfferKind.end()) {
    m_controller->setKindFilter(it->second);
  } else {
    m_controller->setKindFilter({});
  }

  if (!searchText().isEmpty()) { clearSearchText(); }
}

ClipboardHistoryView::DefaultAction ClipboardHistoryView::parseDefaultAction(const QString &str) {
  if (str == "paste") return DefaultAction::Paste;
  return DefaultAction::Copy;
}

void ClipboardHistoryView::saveDropdownFilter(const QString &value) {
  command()->storage().setItem("filter", value);
}

std::optional<QString> ClipboardHistoryView::getSavedDropdownFilter() {
  auto value = command()->storage().getItem("filter");

  if (value.isNull()) return std::nullopt;

  return value.toString();
}
