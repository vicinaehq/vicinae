#pragma once
#include "base-view.hpp"
#include "clipboard-actions.hpp"
#include "settings/command-metadata-settings-detail.hpp"
#include "theme.hpp"
#include "ui/form/selector-input.hpp"
#include "ui/selectable-omni-list-widget.hpp"
#include "utils/layout.hpp"
#include "ui/empty-view/empty-view.hpp"
#include "common.hpp"
#include "extend/metadata-model.hpp"
#include "manage-quicklinks-command.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include "ui/icon-button.hpp"
#include "ui/image/omnimg.hpp"
#include "ui/omni-list.hpp"
#include "ui/split-detail/split-detail.hpp"
#include "ui/toast.hpp"
#include <libqalculate/Number.h>
#include <libqalculate/includes.h>
#include <memory>
#include <qboxlayout.h>
#include <qevent.h>
#include <qlabel.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qproperty.h>
#include <qstackedwidget.h>
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
  int m_id;

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
  PasteClipboardSelection(int id) : PasteToFocusedWindowAction(), m_id(id) {}
};

class CopyClipboardSelection : public AbstractAction {
  int m_id;

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
  CopyClipboardSelection(int id)
      : AbstractAction("Copy to clipboard", BuiltinOmniIconUrl("copy-clipboard")), m_id(id) {}
};

class ClipboardHistoryItemWidget : public SelectableOmniListWidget {
  TypographyWidget *m_title = new TypographyWidget;
  TypographyWidget *m_description = new TypographyWidget;
  Omnimg::ImageWidget *m_icon = new Omnimg::ImageWidget;
  Omnimg::ImageWidget *m_pinIcon = new Omnimg::ImageWidget;

  OmniIconUrl iconForMime(const ClipboardHistoryEntry &entry) const {
    if (entry.mimeType.startsWith("text/")) { return BuiltinOmniIconUrl("text"); }
    if (entry.mimeType.startsWith("image/")) { return BuiltinOmniIconUrl("image"); }
    return BuiltinOmniIconUrl("text");
  }

  void setupUI() {
    m_pinIcon->setUrl(BuiltinOmniIconUrl("pin").setFill(SemanticColor::Red));
    m_pinIcon->setFixedSize(16, 16);
    m_description->setColor(SemanticColor::TextSecondary);
    m_description->setSize(TextSize::TextSmaller);

    auto layout = HStack().margins(5).spacing(10).add(m_icon).add(
        VStack().add(m_title).add(HStack().add(m_pinIcon).add(m_description).spacing(5)));

    setLayout(layout.buildLayout());
  }

public:
  void setEntry(const ClipboardHistoryEntry &entry) {
    auto createdAt = QDateTime::fromSecsSinceEpoch(entry.createdAt);
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
  std::vector<MetadataItem> createEntryMetadata(const ClipboardHistoryEntry &entry) const {
    auto mime = MetadataLabel{
        .text = entry.mimeType,
        .title = "Mime",
    };
    auto id = MetadataLabel{
        .text = QString::number(entry.id),
        .title = "ID",
    };
    auto copiedAt = MetadataLabel{
        .text = QDateTime::fromSecsSinceEpoch(entry.createdAt).toString(),
        .title = "Copied at",
    };
    auto checksum = MetadataLabel{
        .text = entry.md5sum,
        .title = "Checksum (MD5)",
    };

    return {mime, id, copiedAt, checksum};
  }

  QWidget *createEntryWidget(const ClipboardHistoryEntry &entry) {

    if (entry.mimeType.startsWith("text/")) {
      auto container = new TextContainer;
      auto viewer = new TextFileViewer();

      container->setWidget(viewer);
      viewer->load(entry.filePath);

      return container;
    }

    if (entry.mimeType.startsWith("image/")) {
      auto icon = new Omnimg::ImageWidget;

      icon->setContentsMargins(10, 10, 10, 10);
      icon->setUrl(LocalOmniIconUrl(entry.filePath));

      return icon;
    }

    return new QWidget();
  }

public:
  void setEntry(const ClipboardHistoryEntry &entry) {
    if (auto previous = content()) { previous->deleteLater(); }

    auto widget = createEntryWidget(entry);
    // auto metadata = createEntryMetadata(entry);

    setContent(widget);
    // setMetadata({});
  }
};

class RemoveSelectionAction : public AbstractAction {
  int _id;

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
  RemoveSelectionAction(int id) : AbstractAction("Remove entry", BuiltinOmniIconUrl("trash")), _id(id) {
    setStyle(AbstractAction::Danger);
  }
};

class PinClipboardAction : public AbstractAction {
  int _id;
  bool _value;

  void execute() override {
    // To implement
  }

public:
  int entryId() const { return _id; }

  PinClipboardAction(int id, bool value)
      : AbstractAction(value ? "Pin" : "Unpin", BuiltinOmniIconUrl("pin")), _id(id), _value(value) {}
};

class ClipboardHistoryItem : public OmniList::AbstractVirtualItem, public ListView::Actionnable {
public:
  ClipboardHistoryEntry info;

  ActionPanelView *actionPanel() const override {
    auto panel = new ActionPanelStaticListView;
    auto pin = new PinClipboardAction(info.id, !info.pinnedAt);
    auto paste = new PasteClipboardSelection(info.id);
    auto copyToClipboard = new CopyClipboardSelection(info.id);
    auto remove = new RemoveSelectionAction(info.id);

    remove->setStyle(AbstractAction::Danger);
    remove->setShortcut({.key = "X", .modifiers = {"ctrl"}});

    paste->setPrimary(true);
    paste->setShortcut({.key = "return"});

    copyToClipboard->setShortcut({.key = "return", .modifiers = {"shift"}});

    pin->setShortcut({.key = "P", .modifiers = {"shift", "ctrl"}});

    panel->addAction(paste);
    panel->addAction(copyToClipboard);
    panel->addSection();
    panel->addAction(pin);
    panel->addSection();
    panel->addAction(remove);

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

  OmniIconUrl iconForMime(const QString &mime) const {
    if (info.mimeType.startsWith("text/")) { return BuiltinOmniIconUrl("text"); }
    if (info.mimeType.startsWith("image/")) { return BuiltinOmniIconUrl("image"); }
    return BuiltinOmniIconUrl("text");
  }

  const QString &name() const { return info.textPreview; }

  QWidget *generateDetail() const override {
    auto detail = new ClipboardHistoryDetail();

    detail->setEntry(info);

    return detail;
  }

  QString generateId() const override { return QString::number(info.id); }

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

  OmniIconUrl statusIcon(ClipboardStatus status) {
    switch (status) {
    case Monitoring:
      return BuiltinOmniIconUrl("pause-filled").setFill(SemanticColor::Orange);
    case Paused:
      return BuiltinOmniIconUrl("play-filled").setFill(SemanticColor::Green);
    default:
      return BuiltinOmniIconUrl("warning").setFill(SemanticColor::Red);
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
    m_rightIcon->setUrl(BuiltinOmniIconUrl("pause-filled"));
    m_right->setLayout(rightLayout);
    m_rightText->setText("Pause clipboard");

    auto layout = new QHBoxLayout;

    layout->setContentsMargins(15, 8, 15, 8);
    m_text->setText("323 Items");

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
    {"Files", "file"},
    {"Images", "image"},
    {"Links", "link"},
};

class ClipboardHistoryView : public SimpleView {
  OmniList *m_list = new OmniList();
  ClipboardStatusToobar *m_statusToolbar = new ClipboardStatusToobar;
  SplitDetailWidget *m_split = new SplitDetailWidget(this);
  EmptyViewWidget *m_emptyView = new EmptyViewWidget;
  QStackedWidget *m_content = new QStackedWidget(this);
  QTimer m_searchDebounce;
  PreferenceDropdown *m_filterInput = new PreferenceDropdown(this);

  void handleDebounce() {
    qDebug() << "search text" << searchText();
    generateList(searchText());
  }

  QWidget *searchBarAccessory() const override { return m_filterInput; }

  void generateList(const QString &query) {
    auto clipman = ServiceRegistry::instance()->clipman();
    auto result = clipman->listAll(1000, 0, {.query = query});
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
    generateList("");
  }

  void selectionChanged(const OmniList::AbstractVirtualItem *next,
                        const OmniList::AbstractVirtualItem *previous) const {
    if (!next) {
      m_split->setDetailVisibility(false);
      m_actionPannelV2->popToRoot();
      return;
    }

    auto entry = static_cast<const ClipboardHistoryItem *>(next);

    if (auto detail = entry->generateDetail()) {
      m_split->setDetailWidget(detail);
      m_split->setDetailVisibility(true);
    }
    if (auto panel = entry->actionPanel()) { m_actionPannelV2->setView(panel); }
  }

  void textChanged(const QString &value) override { m_searchDebounce.start(200); }

  void clipboardSelectionInserted(const ClipboardHistoryEntry &entry) {}

  void handlePinChanged(int entryId, bool value) { generateList(searchText()); }
  void handleRemoved(int entryId) { generateList(searchText()); }

  void handleMonitoringChanged(bool monitor) {
    if (monitor) {
      m_statusToolbar->setClipboardStatus(ClipboardStatusToobar::ClipboardStatus::Monitoring);
      return;
    }

    m_statusToolbar->setClipboardStatus(ClipboardStatusToobar::ClipboardStatus::Paused);
  }

  void handleStatusClipboard() {
    qCritical() << "status changed, click";
    auto manager = ServiceRegistry::instance()->rootItemManager();
    QString rootItemId = "extension.clipboard.clipboard-history";
    auto preferences = manager->getItemPreferenceValues(rootItemId);

    if (m_statusToolbar->clipboardStatus() == ClipboardStatusToobar::Paused) {
      preferences["monitoring"] = true;
    } else {
      preferences["monitoring"] = false;
    }

    manager->setItemPreferenceValues(rootItemId, preferences);
  }

  bool inputFilter(QKeyEvent *event) override {
    switch (event->key()) {
    case Qt::Key_Up:
      return m_list->selectUp();
    case Qt::Key_Down:
      return m_list->selectDown();
    case Qt::Key_Return:
      m_list->activateCurrentSelection();
    }

    if (event->keyCombination() == QKeyCombination(Qt::ControlModifier, Qt::Key_P)) {
      m_filterInput->openSelector();
      return true;
    }

    return SimpleView::inputFilter(event);
  }

  void handleFilterChange(const SelectorInput::AbstractItem &item) {
    qDebug() << "filter changed" << item.id();
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
    m_emptyView->setIcon(BuiltinOmniIconUrl("magnifying-glass"));

    m_searchDebounce.setSingleShot(true);

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
    connect(&m_searchDebounce, &QTimer::timeout, this, &ClipboardHistoryView::handleDebounce);
    connect(m_filterInput, &SelectorInput::selectionChanged, this, &ClipboardHistoryView::handleFilterChange);
  }
};
