#pragma once
#include "app.hpp"
#include "calculator-history-command.hpp"
#include "clipboard/clipboard-service.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include "ui/alert.hpp"
#include "ui/declarative-omni-list-view.hpp"
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

class CopyClipboardSelection : public AbstractAction {
  int m_id;

  void execute(AppWindow &app) override {
    auto clipman = ServiceRegistry::instance()->clipman();
    auto wm = ServiceRegistry::instance()->windowManager();

    if (auto selection = clipman->retrieveSelectionById(m_id)) {
      // XXX - We may want to update the update time of this selection to make
      // it appear on top in subsequent searches.
      clipman->copySelection(*selection, {.concealed = true});
      app.statusBar->setToast("Selection copied");
      app.closeWindow(true);
      QTimer::singleShot(10, [wm]() { wm->pasteToFocusedWindow(); });
    } else {
      app.statusBar->setToast(QString("No selection with ID %1").arg(m_id));
    }
  }

public:
  CopyClipboardSelection(int id)
      : AbstractAction("Copy selection", BuiltinOmniIconUrl("clipboard")), m_id(id) {}
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

      icon->setUrl(LocalOmniIconUrl(entry.filePath).setMask(OmniPainter::RoundedRectangleMask));

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
  Q_OBJECT

  int _id;

  void execute(AppWindow &app) override {
    auto clipman = ServiceRegistry::instance()->clipman();

    if (clipman->removeSelection(_id)) {
      app.statusBar->setToast("Entry removed");
      emit removed(_id);
    } else {
      app.statusBar->setToast("Failed to remove entry", ToastPriority::Danger);
    }
  }

public:
  RemoveSelectionAction(int id) : AbstractAction("Remove entry", BuiltinOmniIconUrl("trash")), _id(id) {}

signals:
  void removed(int id) const;
};

class PinConfirm : public AlertWidget {
  Q_OBJECT

  AppWindow &_app;
  int _id;
  bool _value;

public:
  PinConfirm(AppWindow &app, int id, bool value) : _app(app), _id(id), _value(value) {}

  void confirm() const override {
    auto clipman = ServiceRegistry::instance()->clipman();

    if (!clipman->setPinned(_id, _value)) {
      _app.statusBar->setToast("Failed to " + QString(_value ? "pin" : "unpin"), ToastPriority::Danger);
    } else {
      _app.statusBar->setToast(_value ? "Pinned" : "Unpinned", ToastPriority::Success);
      emit pinChanged(_id, _value);
    }
  }

  void canceled() const override {}

signals:
  bool pinChanged(int pid, bool _value) const;
};

class PinClipboardAction : public AbstractAction {
  Q_OBJECT

  int _id;
  bool _value;

  void execute(AppWindow &app) override {
    auto confirm = new PinConfirm(app, _id, _value);

    connect(confirm, &PinConfirm::pinChanged, this, &PinClipboardAction::pinChanged);

    app.confirmAlert(confirm);
  }

public:
  int entryId() const { return _id; }

  PinClipboardAction(int id, bool value)
      : AbstractAction(value ? "Pin" : "Unpin", BuiltinOmniIconUrl("pin")), _id(id), _value(value) {}

signals:
  void pinChanged(int id, bool value) const;
};

class ClipboardHistoryItem : public AbstractDefaultListItem, public DeclarativeOmniListView::IActionnable {
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
    auto copy = new CopyClipboardSelection(current->info.id);

    connect(pin, &PinClipboardAction::pinChanged, this, &ClipboardHistoryItemActionGenerator::pinChanged);
    connect(remove, &RemoveSelectionAction::removed, this, &ClipboardHistoryItemActionGenerator::removed);

    return {copy, pin, remove};
  }

signals:
  void pinChanged(int entryId, bool value) const;
  void removed(int entryId) const;
};

class ClipboardHistoryCommand : public DeclarativeOmniListView {
  ClipboardHistoryItemActionGenerator *_generator = new ClipboardHistoryItemActionGenerator;

  ItemList generateList(const QString &query) override {
    auto clipman = ServiceRegistry::instance()->clipman();
    auto result = clipman->listAll(100, 0, {.query = query});
    ItemList newList;

    newList.reserve(result.data.size() + 2);
    newList.emplace_back(std::make_unique<OmniList::VirtualSection>("Pinned"));
    size_t i = 0;

    while (i < result.data.size() && result.data[i].pinnedAt) {
      auto &entry = result.data[i];
      auto candidate = std::make_unique<ClipboardHistoryItem>(entry);

      candidate->setActionGenerator(_generator);
      newList.emplace_back(std::move(candidate));
      ++i;
    }

    newList.emplace_back(std::make_unique<OmniList::VirtualSection>("History", false));

    while (i < result.data.size()) {
      auto &entry = result.data[i];
      auto candidate = std::make_unique<ClipboardHistoryItem>(entry);

      candidate->setActionGenerator(_generator);
      newList.emplace_back(std::move(candidate));
      ++i;
    }

    return newList;
  }

  void onMount() override { setSearchPlaceholderText("Browse clipboard history..."); }

  void clipboardSelectionInserted(const ClipboardHistoryEntry &entry) { reload(); }

  void handlePinChanged(int entryId, bool value) { reload(); }
  void handleRemoved(int entryId) { reload(); }

public:
  ClipboardHistoryCommand(AppWindow &app) : DeclarativeOmniListView(app) {
    auto clipman = ServiceRegistry::instance()->clipman();

    connect(_generator, &ClipboardHistoryItemActionGenerator::pinChanged, this,
            &ClipboardHistoryCommand::handlePinChanged);
    connect(_generator, &ClipboardHistoryItemActionGenerator::removed, this,
            &ClipboardHistoryCommand::handleRemoved);
    connect(clipman, &ClipboardService::itemInserted, this,
            &ClipboardHistoryCommand::clipboardSelectionInserted);
  }
};
