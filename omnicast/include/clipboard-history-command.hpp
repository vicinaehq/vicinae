#pragma once
#include "app.hpp"
#include "clipboard-service.hpp"
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

    if (entry.mimeType == "text/plain") {
      auto container = new TextContainer;
      auto viewer = new TextFileViewer();

      container->setWidget(viewer);
      viewer->load(entry.filePath);

      return container;
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

class ClipboardHistoryItem : public AbstractDefaultListItem, public OmniListView::IActionnable {
  ClipboardHistoryEntry info;
  std::function<void(int)> _pinCallback;

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

public:
  QList<AbstractAction *> generateActions() const override {
    auto pin = new PinClipboardAction(info.id, !info.pinnedAt);

    pin->setExecutionCallback([this]() {
      if (_pinCallback) { _pinCallback(info.id); }
    });

    return {new CopyTextAction("Copy preview", info.textPreview), pin};
  }

  const QString &name() const { return info.textPreview; }

  void setPinCallback(const std::function<void(int)> &cb) { _pinCallback = cb; }

  ItemData data() const override { return {.iconUrl = BuiltinOmniIconUrl("text"), .name = info.textPreview}; }

  QWidget *generateDetail() const override {
    auto detail = std::make_unique<ClipboardItemDetail>(info);

    return new OmniListView::SideDetailWidget(*detail.get());
  }

  QString id() const override { return QString::number(info.id); }

public:
  ClipboardHistoryItem(const ClipboardHistoryEntry &info) : info(info) {}
};

class ClipboardHistoryCommand : public OmniListView {
  Service<ClipboardService> _clipboardService;

  void generateList(const QString &query) {
    std::vector<std::unique_ptr<OmniList::AbstractVirtualItem>> newList;
    int totalCount = 0;
    std::vector<ClipboardHistoryEntry> items;

    if (query.isEmpty()) {
      auto page = _clipboardService.listAll();

      totalCount = page.totalCount;
      items = page.data;
    } else {
      items = _clipboardService.collectedSearch(query);
      totalCount = items.size();
    }

    list->clearSelection();
    newList.push_back(std::make_unique<OmniList::VirtualSection>("Pinned"));
    size_t i = 0;

    while (i < items.size() && items[i].pinnedAt) {
      auto &entry = items[i];
      auto candidate = std::make_unique<ClipboardHistoryItem>(entry);

      candidate->setPinCallback([this, &entry, query](int id) {
        list->invalidateCache(QString::number(id));
        generateList(query);
      });
      newList.push_back(std::move(candidate));
      ++i;
    }

    newList.push_back(std::make_unique<OmniList::FixedCountSection>("History", totalCount - i));

    while (i < items.size()) {
      auto &entry = items[i];
      auto candidate = std::make_unique<ClipboardHistoryItem>(entry);

      candidate->setPinCallback([this, &entry, query](int id) {
        list->invalidateCache(QString::number(id));
        generateList(query);
      });

      newList.push_back(std::move(candidate));
      ++i;
    }

    list->updateFromList(newList, OmniList::SelectionPolicy::SelectFirst);
  }

  void onMount() override { setSearchPlaceholderText("Browse clipboard history..."); }

  void onSearchChanged(const QString &s) override { generateList(s); }

public:
  ClipboardHistoryCommand(AppWindow &app)
      : OmniListView(app), _clipboardService(service<ClipboardService>()) {}
};
