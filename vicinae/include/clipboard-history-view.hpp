#pragma once
#include "navigation-controller.hpp"
#include "services/clipboard/clipboard-db.hpp"
#include "ui/views/base-view.hpp"
#include "clipboard-actions.hpp"
#include "settings/command-metadata-settings-detail.hpp"
#include "theme.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "ui/form/selector-input.hpp"
#include "ui/selectable-omni-list-widget/selectable-omni-list-widget.hpp"
#include "utils/layout.hpp"
#include "ui/empty-view/empty-view.hpp"
#include "common.hpp"
#include "extend/metadata-model.hpp"
#include "manage-quicklinks-command.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "../src/ui/image/url.hpp"
#include "service-registry.hpp"
#include "ui/icon-button/icon-button.hpp"
#include "ui/image/image.hpp"
#include "ui/omni-list/omni-list.hpp"
#include "ui/split-detail/split-detail.hpp"
#include "ui/toast/toast.hpp"
#include <QtCore>
#include <cstdio>
#include <libqalculate/Number.h>
#include <libqalculate/includes.h>
#include "services/toast/toast-service.hpp"
#include <memory>
#include <qboxlayout.h>
#include <qevent.h>
#include <qfuturewatcher.h>
#include <qlabel.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qproperty.h>
#include <qstackedwidget.h>
#include <qstringview.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <sys/socket.h>
#include "text-file-viewer.hpp"
#include "ui/typography/typography.hpp"
#include "utils/utils.hpp"

class LogWidget : public QWidget {
  ~LogWidget() { qDebug() << "widget down"; }
};

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
    auto clipman = ctx->services->clipman();
    auto toast = ctx->services->toastService();

    if (auto selection = clipman->retrieveSelectionById(m_id)) {
      loadClipboardData(*selection);
      PasteToFocusedWindowAction::execute(ctx);
      return;
    }

    toast->setToast(QString("No selection with ID %1").arg(m_id));
  }

public:
  PasteClipboardSelection(const QString &id) : PasteToFocusedWindowAction(), m_id(id) {}
};

class CopyClipboardSelection : public AbstractAction {
  QString m_id;

  void execute(ApplicationContext *ctx) override {
    auto clipman = ctx->services->clipman();
    auto toast = ctx->services->toastService();

    if (auto selection = clipman->retrieveSelectionById(m_id)) {
      clipman->copySelection(*selection, {.concealed = true});
      ctx->navigation->showHud("Selection copied to clipboard");
      return;
    }

    toast->setToast("Failed to copy to clipboard", ToastPriority::Danger);
  }

public:
  CopyClipboardSelection(const QString &id)
      : AbstractAction("Copy to clipboard", ImageURL::builtin("copy-clipboard")), m_id(id) {}
};

class ClipboardHistoryItemWidget : public SelectableOmniListWidget {
  TypographyWidget *m_title = new TypographyWidget;
  TypographyWidget *m_description = new TypographyWidget;
  ImageWidget *m_icon = new ImageWidget;
  ImageWidget *m_pinIcon = new ImageWidget;

  ImageURL getLinkIcon(const std::optional<QString> &urlHost) const {
    auto dflt = ImageURL::builtin("link");

    if (urlHost) return ImageURL::favicon(*urlHost).withFallback(dflt);

    return dflt;
  }

  ImageURL iconForMime(const ClipboardHistoryEntry &entry) const {
    switch (entry.kind) {
    case ClipboardOfferKind::Image:
      return ImageURL::builtin("image");
    case ClipboardOfferKind::Link:
      return getLinkIcon(entry.urlHost);
    case ClipboardOfferKind::Text:
      return ImageURL::builtin("text");
    default:
      break;
    }
    return ImageURL::builtin("question-mark-circle");
  }

  void setupUI() {
    m_pinIcon->setUrl(ImageURL::builtin("pin").setFill(SemanticColor::Red));
    m_pinIcon->setFixedSize(16, 16);
    m_description->setColor(SemanticColor::TextSecondary);
    m_description->setSize(TextSize::TextSmaller);

    auto layout = HStack().margins(5).spacing(10).add(m_icon).add(
        VStack().add(m_title).add(HStack().add(m_pinIcon).add(m_description).spacing(5)));

    setLayout(layout.buildLayout());
  }

public:
  void setEntry(const ClipboardHistoryEntry &entry) {
    auto createdAt = QDateTime::fromSecsSinceEpoch(entry.updatedAt);
    m_title->setText(entry.textPreview);
    m_pinIcon->setVisible(entry.pinnedAt);
    // TODO: add char count / size
    m_description->setText(QString("%1").arg(getRelativeTimeString(createdAt)));
    m_icon->setFixedSize(25, 25);
    m_icon->setUrl(iconForMime(entry));
  }

  ClipboardHistoryItemWidget() { setupUI(); }
};

class ClipboardHistoryDetail : public DetailWithMetadataWidget {
  QTemporaryFile m_tmpFile;

  std::vector<MetadataItem> createEntryMetadata(const ClipboardHistoryEntry &entry) const {
    auto mime = MetadataLabel{
        .text = entry.mimeType,
        .title = "Mime",
    };
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
        .title = "Checksum (MD5)",
    };

    return {mime, size, copiedAt, checksum};
  }

  QWidget *createEntryWidget(const ClipboardHistoryEntry &entry) {
    auto data = ServiceRegistry::instance()->clipman()->decryptMainSelectionOffer(entry.id);

    if (entry.mimeType.startsWith("text/")) {
      auto container = new TextContainer;
      auto viewer = new TextFileViewer();

      container->setWidget(viewer);
      viewer->load(data);

      return container;
    }

    if (entry.mimeType.startsWith("image/")) {
      if (!m_tmpFile.open()) { qWarning() << "Failed to open file"; }

      m_tmpFile.write(data);
      m_tmpFile.close();

      auto icon = new ImageWidget;

      icon->setContentsMargins(10, 10, 10, 10);
      icon->setUrl(ImageURL::local(m_tmpFile.filesystemFileName()));

      return icon;
    }

    return new QWidget();
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
      toast->setToast("Failed to remove entry", ToastPriority::Danger);
    }
  }

public:
  RemoveSelectionAction(const QString &id)
      : AbstractAction("Remove entry", ImageURL::builtin("trash")), _id(id) {
    setStyle(AbstractAction::Danger);
  }
};

class PinClipboardAction : public AbstractAction {
  QString _id;
  bool _value;

  void execute() override {
    // To implement
  }

public:
  QString entryId() const { return _id; }

  PinClipboardAction(const QString &id, bool value)
      : AbstractAction(value ? "Pin" : "Unpin", ImageURL::builtin("pin")), _id(id), _value(value) {}
};

class ClipboardHistoryItem : public OmniList::AbstractVirtualItem, public ListView::Actionnable {
public:
  ClipboardHistoryEntry info;

  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx) const override {
    auto panel = std::make_unique<ActionPanelState>();
    auto wm = ctx->services->windowManager();
    auto pin = new PinClipboardAction(info.id, !info.pinnedAt);
    auto copyToClipboard = new CopyClipboardSelection(info.id);
    auto remove = new RemoveSelectionAction(info.id);
    auto mainSection = panel->createSection();

    remove->setStyle(AbstractAction::Danger);
    remove->setShortcut({.key = "X", .modifiers = {"ctrl"}});

    pin->setShortcut({.key = "P", .modifiers = {"shift", "ctrl"}});

    if (wm->canPaste()) {
      auto paste = new PasteClipboardSelection(info.id);

      paste->setShortcut({.key = "return"});
      paste->setPrimary(true);
      mainSection->addAction(paste);
      copyToClipboard->setShortcut({.key = "return", .modifiers = {"shift"}});
    } else {
      copyToClipboard->setPrimary(true);
      copyToClipboard->setShortcut({.key = "return"});
    }

    mainSection->addAction(copyToClipboard);

    auto toolsSection = panel->createSection();
    auto dangerSection = panel->createSection();

    toolsSection->addAction(pin);
    dangerSection->addAction(remove);

    return panel;
  }

  OmniListItemWidget *createWidget() const override {
    auto widget = new ClipboardHistoryItemWidget();

    widget->setEntry(info);

    return widget;
  }

  void refresh(QWidget *widget) const override {
    static_cast<ClipboardHistoryItemWidget *>(widget)->setEntry(info);
  }

  bool recyclable() const override { return true; }

  void recycle(QWidget *widget) const override {
    static_cast<ClipboardHistoryItemWidget *>(widget)->setEntry(info);
  }

  bool hasUniformHeight() const override { return true; }

  const QString &name() const { return info.textPreview; }

  QWidget *generateDetail() const override {
    auto detail = new ClipboardHistoryDetail();

    detail->setEntry(info);

    return detail;
  }

  QString generateId() const override { return info.id; }

public:
  ClipboardHistoryItem(const ClipboardHistoryEntry &info) : info(info) {}
};

class ClipboardStatusToobar : public QWidget {
  Q_OBJECT

public:
  enum ClipboardStatus { Monitoring, Paused, Unavailable };

  TypographyWidget *m_text = new TypographyWidget;

  QWidget *m_right = new QWidget;
  TypographyWidget *m_rightText = new TypographyWidget;
  IconButton *m_rightIcon = new IconButton;
  ClipboardStatus m_status = ClipboardStatus::Unavailable;

  QString statusText(ClipboardStatus status) {
    switch (status) {
    case Monitoring:
      return "Pause clipboard";
    case Paused:
      return "Resume clipboard";
    default:
      return "Clipboard monitoring unavailable";
    }
  }

  ImageURL statusIcon(ClipboardStatus status) {
    switch (status) {
    case Monitoring:
      return ImageURL::builtin("pause-filled").setFill(SemanticColor::Orange);
    case Paused:
      return ImageURL::builtin("play-filled").setFill(SemanticColor::Green);
    default:
      return ImageURL::builtin("warning").setFill(SemanticColor::Red);
    }
  }

public:
  ClipboardStatus clipboardStatus() const { return m_status; }

  void setLeftText(const QString &text) { m_text->setText(text); }

  void setClipboardStatus(ClipboardStatus status) {
    m_rightText->setText(statusText(status));
    m_rightIcon->setUrl(statusIcon(status));
    m_status = status;
  }

  ClipboardStatusToobar(QWidget *parent = nullptr) : QWidget(parent) {
    auto rightLayout = new QHBoxLayout;

    rightLayout->addWidget(m_rightText);
    rightLayout->addWidget(m_rightIcon);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    m_rightIcon->setFixedSize(25, 25);
    m_rightIcon->setUrl(ImageURL::builtin("pause-filled"));
    m_rightIcon->setBackgroundColor(Qt::transparent);
    m_right->setLayout(rightLayout);
    m_rightText->setText("Pause clipboard");

    auto layout = new QHBoxLayout;

    layout->setContentsMargins(15, 8, 15, 8);
    m_text->setText("0 Items");

    layout->addWidget(m_text);
    layout->addWidget(m_right, 0, Qt::AlignRight | Qt::AlignVCenter);
    setLayout(layout);

    connect(m_rightIcon, &IconButton::clicked, this, &ClipboardStatusToobar::statusIconClicked);
  }

signals:
  void statusIconClicked();
};

static const std::vector<Preference::DropdownData::Option> filterSelectorOptions = {
    {"All", "all"},
    {"Text", "text"},
    {"Images", "image"},
    {"Links", "link"},
};

static const std::unordered_map<QString, ClipboardOfferKind> typeToOfferKind{
    {"image", ClipboardOfferKind::Image},
    {"link", ClipboardOfferKind::Link},
    {"text", ClipboardOfferKind::Text},
};

class ClipboardHistoryView : public SimpleView {
  OmniList *m_list = new OmniList();
  ClipboardStatusToobar *m_statusToolbar = new ClipboardStatusToobar;
  SplitDetailWidget *m_split = new SplitDetailWidget(this);
  EmptyViewWidget *m_emptyView = new EmptyViewWidget;
  QStackedWidget *m_content = new QStackedWidget(this);
  PreferenceDropdown *m_filterInput = new PreferenceDropdown(this);
  using Watcher = QFutureWatcher<PaginatedResponse<ClipboardHistoryEntry>>;
  Watcher m_watcher;
  std::optional<ClipboardOfferKind> m_kindFilter;

  void reloadCurrentSearch() { startSearch({.query = searchText(), .kind = m_kindFilter}); }

  void handleListFinished() {
    if (!m_watcher.isFinished()) return;

    generateList(m_watcher.result());
  }

  QWidget *searchBarAccessory() const override { return m_filterInput; }

  void generateList(const PaginatedResponse<ClipboardHistoryEntry> &result) {
    size_t i = 0;

    qDebug() << "results" << result.totalCount;

    if (result.data.empty()) {
      m_content->setCurrentWidget(m_emptyView);
    } else {
      m_content->setCurrentWidget(m_split);
    }

    m_statusToolbar->setLeftText(QString("%1 Items").arg(result.data.size()));

    m_list->updateModel([&]() {
      auto &pinnedSection = m_list->addSection();

      while (i < result.data.size() && result.data[i].pinnedAt) {
        auto &entry = result.data[i];
        auto candidate = std::make_unique<ClipboardHistoryItem>(entry);

        pinnedSection.addItem(std::move(candidate));
        ++i;
      }

      while (i < result.data.size()) {
        auto &entry = result.data[i];
        auto candidate = std::make_unique<ClipboardHistoryItem>(entry);

        pinnedSection.addItem(std::move(candidate));
        ++i;
      }
    });
  }

  void initialize() override {
    setSearchPlaceholderText("Browse clipboard history...");
    textChanged("");
  }

  void selectionChanged(const OmniList::AbstractVirtualItem *next,
                        const OmniList::AbstractVirtualItem *previous) const {
    if (!next) {
      m_split->setDetailVisibility(false);
      return;
    }

    auto entry = static_cast<const ClipboardHistoryItem *>(next);

    context()->navigation->setActions(entry->newActionPanel(context()));

    if (auto detail = entry->generateDetail()) {
      if (auto current = m_split->detailWidget()) { current->deleteLater(); }

      m_split->setDetailWidget(detail);
      m_split->setDetailVisibility(true);
    }
  }

  void textChanged(const QString &value) override { startSearch({.query = value, .kind = m_kindFilter}); }

  void clipboardSelectionInserted(const ClipboardHistoryEntry &entry) { reloadCurrentSearch(); }

  void handlePinChanged(int entryId, bool value) { reloadCurrentSearch(); }
  void handleRemoved(int entryId) { reloadCurrentSearch(); }

  void handleMonitoringChanged(bool monitor) {
    if (monitor) {
      m_statusToolbar->setClipboardStatus(ClipboardStatusToobar::ClipboardStatus::Monitoring);
      return;
    }

    m_statusToolbar->setClipboardStatus(ClipboardStatusToobar::ClipboardStatus::Paused);
  }

  void handleStatusClipboard() {
    auto manager = context()->services->rootItemManager();
    QString rootItemId = "extension.clipboard.history";
    auto preferences = manager->getItemPreferenceValues(rootItemId);

    if (m_statusToolbar->clipboardStatus() == ClipboardStatusToobar::Paused) {
      preferences["monitoring"] = true;
    } else {
      preferences["monitoring"] = false;
    }

    manager->setItemPreferenceValues(rootItemId, preferences);
  }

  bool inputFilter(QKeyEvent *event) override {
    if (event->modifiers().toInt() == 0) {
      switch (event->key()) {
      case Qt::Key_Up:
        return m_list->selectUp();
      case Qt::Key_Down:
        return m_list->selectDown();
      case Qt::Key_Return:
        m_list->activateCurrentSelection();
        return true;
      }
    }

    if (event->keyCombination() == QKeyCombination(Qt::ControlModifier, Qt::Key_P)) {
      m_filterInput->openSelector();
      return true;
    }

    return SimpleView::inputFilter(event);
  }

  void startSearch(const ClipboardListSettings &opts) {
    auto clipman = context()->services->clipman();

    if (m_watcher.isRunning()) { m_watcher.cancel(); }

    m_watcher.setFuture(clipman->listAll(1000, 0, opts));
  }

  void handleFilterChange(const SelectorInput::AbstractItem &item) {
    if (auto it = typeToOfferKind.find(item.id()); it != typeToOfferKind.end()) {
      m_kindFilter = it->second;
    } else {
      m_kindFilter.reset();
    }

    if (!searchText().isEmpty()) {
      clearSearchText();
    } else {
      reloadCurrentSearch();
    }
  }

public:
  ClipboardHistoryView() {
    auto clipman = ServiceRegistry::instance()->clipman();
    auto layout = new QVBoxLayout;

    if (!clipman->isServerRunning()) {
      m_statusToolbar->setClipboardStatus(ClipboardStatusToobar::ClipboardStatus::Unavailable);
    } else {
      handleMonitoringChanged(clipman->monitoring());
    }

    m_filterInput->setMinimumWidth(300);
    m_filterInput->setFocusPolicy(Qt::NoFocus);
    m_filterInput->setOptions(filterSelectorOptions);
    m_filterInput->setValue("all");

    m_content->addWidget(m_split);
    m_content->addWidget(m_emptyView);
    m_content->setCurrentWidget(m_split);

    m_emptyView->setTitle("No clipboard entries");
    m_emptyView->setDescription("No results matching your search. You can try to refine your search.");
    m_emptyView->setIcon(ImageURL::builtin("magnifying-glass"));

    m_split->setMainWidget(m_list);
    m_split->setDetailVisibility(false);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_statusToolbar);
    layout->addWidget(new HDivider);
    layout->addWidget(m_content, 1);
    setLayout(layout);

    connect(m_list, &OmniList::selectionChanged, this, &ClipboardHistoryView::selectionChanged);
    connect(m_list, &OmniList::itemActivated, this, [this]() { executePrimaryAction(); });
    connect(clipman, &ClipboardService::itemInserted, this,
            &ClipboardHistoryView::clipboardSelectionInserted);
    connect(clipman, &ClipboardService::monitoringChanged, this,
            &ClipboardHistoryView::handleMonitoringChanged);
    connect(m_statusToolbar, &ClipboardStatusToobar::statusIconClicked, this,
            &ClipboardHistoryView::handleStatusClipboard);
    connect(m_filterInput, &SelectorInput::selectionChanged, this, &ClipboardHistoryView::handleFilterChange);
    connect(&m_watcher, &Watcher::finished, this, &ClipboardHistoryView::handleListFinished);
  }
};
