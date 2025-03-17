#pragma once
#include "app.hpp"
#include "clipboard/clipboard-service.hpp"
#include "omni-icon.hpp"
#include "theme.hpp"
#include "ui/action_popover.hpp"
#include "ui/omni-list-view.hpp"
#include "ui/omni-list.hpp"
#include "ui/toast.hpp"
#include <memory>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qobject.h>
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

      auto icon = new OmniIcon;

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

class ActionGenerator : public QObject {
public:
  virtual QList<AbstractAction *> generateActions(const OmniList::AbstractVirtualItem *item) = 0;
};

class RemoveSelectionAction : public AbstractAction {
  int _id;

  void execute(AppWindow &app) override {
    if (app.clipboardService->removeSelection(_id)) {
      app.statusBar->setToast("Entry removed");
    } else {
      app.statusBar->setToast("Failed to remove entry", ToastPriority::Danger);
    }
  }

public:
  RemoveSelectionAction(int id) : AbstractAction("Remove entry", BuiltinOmniIconUrl("trash")), _id(id) {}
};

class PinClipboardAction : public AbstractAction {
  int _id;
  bool _value;

  void execute(AppWindow &app) override {
    if (!app.clipboardService->setPinned(_id, _value)) {
      app.statusBar->setToast("Failed to " + QString(_value ? "pin" : "unpin"), ToastPriority::Danger);
    } else {
      app.statusBar->setToast(_value ? "Pinned" : "Unpinned", ToastPriority::Success);
    }
  }

public:
  PinClipboardAction(int id, bool value)
      : AbstractAction(value ? "Pin" : "Unpin", BuiltinOmniIconUrl("pin")), _id(id), _value(value) {}
};

class ClipboardHistoryItem : public AbstractDefaultListItem, public OmniListView::IActionnable {
  std::function<void(int)> _pinCallback;
  std::function<void(int)> _removeCallback;

  ActionGenerator *_generator = nullptr;

public:
  ClipboardHistoryEntry info;
  QList<AbstractAction *> generateActions() const override { return _generator->generateActions(this); }

  void setActionGenerator(ActionGenerator *generator) { _generator = generator; }

  OmniIconUrl iconForMime(const QString &mime) const {
    if (info.mimeType.startsWith("text/")) { return BuiltinOmniIconUrl("text"); }
    if (info.mimeType.startsWith("image/")) { return BuiltinOmniIconUrl("image"); }
    return BuiltinOmniIconUrl("text");
  }

  const QString &name() const { return info.textPreview; }

  void setPinCallback(const std::function<void(int)> &cb) { _pinCallback = cb; }
  void setRemoveCallback(const std::function<void(int)> &cb) { _removeCallback = cb; }

  ItemData data() const override { return {.iconUrl = iconForMime(info.mimeType), .name = info.textPreview}; }

  QWidget *generateDetail() const override {
    auto detail = std::make_unique<ClipboardItemDetail>(info);

    return new OmniListView::SideDetailWidget(*detail.get());
  }

  QString id() const override { return QString::number(info.id); }

public:
  ClipboardHistoryItem(const ClipboardHistoryEntry &info) : info(info) {}
};

class ClipboardHistoryItemActionGenerator : public ActionGenerator {
  Q_OBJECT

  QList<AbstractAction *> generateActions(const OmniList::AbstractVirtualItem *item) override {
    auto current = static_cast<const ClipboardHistoryItem *>(item);
    auto pin = new PinClipboardAction(current->info.id, !current->info.pinnedAt);
    auto remove = new RemoveSelectionAction(current->info.id);
    auto copy = new CopyTextAction("Copy preview", current->info.textPreview);
    int id = current->info.id;

    connect(pin, &PinClipboardAction::didExecute, this, [id, this]() { emit pinChanged(id, true); });
    connect(remove, &PinClipboardAction::didExecute, this, [id, this] { emit removed(id); });

    return {copy, pin, remove};
  }

signals:
  void pinChanged(int id, bool value) const;
  void removed(int id) const;
};

class ClipboardHistoryCommand : public OmniListView {
  Service<ClipboardService> _clipboardService;
  ClipboardHistoryItemActionGenerator *_generator = new ClipboardHistoryItemActionGenerator;
  QString _query;

  void generateList(const QString &query) {
    _query = query;
    std::vector<std::unique_ptr<OmniList::AbstractVirtualItem>> newList;
    auto result = _clipboardService.listAll(100, 0, {.query = query});

    newList.push_back(std::make_unique<OmniList::VirtualSection>("Pinned"));
    size_t i = 0;

    while (i < result.data.size() && result.data[i].pinnedAt) {
      auto &entry = result.data[i];
      auto candidate = std::make_unique<ClipboardHistoryItem>(entry);

      candidate->setActionGenerator(_generator);
      newList.push_back(std::move(candidate));
      ++i;
    }

    newList.push_back(std::make_unique<OmniList::FixedCountSection>("History", result.totalCount - i));

    while (i < result.data.size()) {
      auto &entry = result.data[i];
      auto candidate = std::make_unique<ClipboardHistoryItem>(entry);

      candidate->setActionGenerator(_generator);
      newList.push_back(std::move(candidate));
      ++i;
    }

    list->updateFromList(newList, OmniList::SelectionPolicy::SelectFirst);
  }

  void onMount() override { setSearchPlaceholderText("Browse clipboard history..."); }

  void onSearchChanged(const QString &s) override { generateList(s); }

  void handlePinChanged(int id, bool value) {
    list->invalidateCache(QString::number(id));
    generateList(_query);
  }

  void handleItemRemoved(int id) { generateList(_query); }

public:
  ClipboardHistoryCommand(AppWindow &app)
      : OmniListView(app), _clipboardService(service<ClipboardService>()) {
    connect(_generator, &ClipboardHistoryItemActionGenerator::pinChanged, this,
            &ClipboardHistoryCommand::handlePinChanged);
    connect(_generator, &ClipboardHistoryItemActionGenerator::removed, this,
            &ClipboardHistoryCommand::handleItemRemoved);
  }
};
