#pragma once
#include "app.hpp"
#include "base-view.hpp"
#include "clipboard-actions.hpp"
#include "clipboard/clipboard-server.hpp"
#include "clipboard/clipboard-service.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include "ui/alert.hpp"
#include "ui/declarative-omni-list-view.hpp"
#include "ui/image/omnimg.hpp"
#include "ui/omni-list-view.hpp"
#include "ui/omni-list.hpp"
#include "ui/toast.hpp"
#include <memory>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qproperty.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <sys/socket.h>
#include "text-file-viewer.hpp"

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

  void execute() override {
    auto ui = ServiceRegistry::instance()->UI();
    auto clipman = ServiceRegistry::instance()->clipman();

    if (auto selection = clipman->retrieveSelectionById(m_id)) {
      loadClipboardData(*selection);
      PasteToFocusedWindowAction::execute();
      return;
    }

    ui->setToast(QString("No selection with ID %1").arg(m_id));
  }

public:
  PasteClipboardSelection(int id) : PasteToFocusedWindowAction(), m_id(id) {}
};

class CopyClipboardSelection : public AbstractAction {
  int m_id;

  void execute() override {
    auto clipman = ServiceRegistry::instance()->clipman();
    auto ui = ServiceRegistry::instance()->UI();

    if (auto selection = clipman->retrieveSelectionById(m_id)) {
      clipman->copySelection(*selection, {.concealed = true});
      ui->setToast("Selection copied to clipboard");
      ui->closeWindow();
      return;
    }

    ui->setToast("Failed to copy to clipboard", ToastPriority::Danger);
  }

public:
  CopyClipboardSelection(int id)
      : AbstractAction("Copy to clipboard", BuiltinOmniIconUrl("copy-clipboard")), m_id(id) {}
};

class ClipboardItemDetail : public OmniListView::MetadataDetailModel {
  ClipboardHistoryEntry entry;

  QWidget *createView() const override {
    auto widget = new QWidget();

    if (entry.mimeType.startsWith("text/plain")) {
      auto container = new TextContainer;
      auto viewer = new TextFileViewer();

      container->setWidget(viewer);
      viewer->load(entry.filePath);

      return container;
    }

    if (entry.mimeType.startsWith("image/")) {
      auto w = new LogWidget;
      auto l = new QVBoxLayout;
      w->setLayout(l);

      auto icon = new Omnimg::ImageWidget;

      // icon->setUrl(LocalOmniIconUrl(entry.filePath).setMask(OmniPainter::RoundedRectangleMask));
      icon->setUrl(LocalOmniIconUrl(entry.filePath));

      l->addWidget(icon, 1, Qt::AlignCenter);

      return w;
    }

    return widget;
  }

  MetadataModel createMetadata() const override {
    QList<MetadataItem> items;

    items << MetadataLabel{
        .text = entry.mimeType,
        .title = "Mime",
    };
    items << MetadataLabel{
        .text = QString::number(entry.id),
        .title = "ID",
    };
    items << MetadataLabel{
        .text = QDateTime::fromSecsSinceEpoch(entry.createdAt).toString(),
        .title = "Copied at",
    };
    items << MetadataLabel{
        .text = entry.md5sum,
        .title = "Checksum (MD5)",
    };

    return {.children = items};
  }

public:
  ClipboardItemDetail(const ClipboardHistoryEntry &entry) : entry(entry) {}
};

class RemoveSelectionAction : public AbstractAction {
  int _id;

  void execute() override {
    auto ui = ServiceRegistry::instance()->UI();
    auto clipman = ServiceRegistry::instance()->clipman();

    if (clipman->removeSelection(_id)) {
      ui->setToast("Entry removed");
    } else {
      ui->setToast("Failed to remove entry", ToastPriority::Danger);
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

class ClipboardHistoryItem : public AbstractDefaultListItem, public ListView::Actionnable {
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

  OmniIconUrl iconForMime(const QString &mime) const {
    if (info.mimeType.startsWith("text/")) { return BuiltinOmniIconUrl("text"); }
    if (info.mimeType.startsWith("image/")) { return BuiltinOmniIconUrl("image"); }
    return BuiltinOmniIconUrl("text");
  }

  const QString &name() const { return info.textPreview; }

  ItemData data() const override { return {.iconUrl = iconForMime(info.mimeType), .name = info.textPreview}; }

  QWidget *generateDetail() const override {
    auto detail = std::make_unique<ClipboardItemDetail>(info);

    return new OmniListView::SideDetailWidget(*detail.get());
  }

  QString generateId() const override { return QString::number(info.id); }

public:
  ClipboardHistoryItem(const ClipboardHistoryEntry &info) : info(info) {}
};

class ClipboardHistoryCommand : public ListView {
  void generateList(const QString &query) {
    auto clipman = ServiceRegistry::instance()->clipman();
    auto result = clipman->listAll(100, 0, {.query = query});
    size_t i = 0;

    m_list->beginResetModel();

    auto &pinnedSection = m_list->addSection("Pinned");

    while (i < result.data.size() && result.data[i].pinnedAt) {
      auto &entry = result.data[i];
      auto candidate = std::make_unique<ClipboardHistoryItem>(entry);

      pinnedSection.addItem(std::move(candidate));
      ++i;
    }

    auto &historySection = m_list->addSection("History");

    while (i < result.data.size()) {
      auto &entry = result.data[i];
      auto candidate = std::make_unique<ClipboardHistoryItem>(entry);

      historySection.addItem(std::move(candidate));
      ++i;
    }

    m_list->endResetModel(OmniList::SelectFirst);
  }

  void initialize() override { onSearchChanged(""); }

  void onSearchChanged(const QString &value) override { generateList(value); }

  void clipboardSelectionInserted(const ClipboardHistoryEntry &entry) {}

  void handlePinChanged(int entryId, bool value) { generateList(searchText()); }
  void handleRemoved(int entryId) { generateList(searchText()); }

public:
  ClipboardHistoryCommand() {
    auto clipman = ServiceRegistry::instance()->clipman();

    setSearchPlaceholderText("Browse clipboard history...");
    connect(clipman, &ClipboardService::itemInserted, this,
            &ClipboardHistoryCommand::clipboardSelectionInserted);
  }
};
