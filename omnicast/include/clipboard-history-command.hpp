#pragma once
#include "app.hpp"
#include "calculator-history-command.hpp"
#include "clipboard-service.hpp"
#include "omni-icon.hpp"
#include "theme.hpp"
#include "ui/action_popover.hpp"
#include "ui/omni-list-view.hpp"
#include "ui/omni-list.hpp"
#include "ui/toast.hpp"
#include <memory>
#include <qlabel.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <sys/socket.h>

class ClipboardItemDetail : public OmniListView::MetadataDetailModel {
  ClipboardHistoryEntry entry;

  QWidget *createView() const override {
    auto widget = new QWidget();
    auto layout = new QHBoxLayout();

    layout->setAlignment(Qt::AlignTop);
    layout->addWidget(new QLabel(entry.textPreview));
    widget->setLayout(layout);

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

    void execute(AppWindow &app) override {
      if (!app.clipboardService->setPinned(_id, true)) {
        app.statusBar->setToast("Failed to pin", ToastPriority::Danger);
      }
    }

  public:
    PinClipboardAction(int id) : AbstractAction("Pin", BuiltinOmniIconUrl("pin")), _id(id) {}
  };

public:
  QList<AbstractAction *> generateActions() const override {
    auto pin = new PinClipboardAction(info.id);

    pin->setExecutionCallback([this]() {
      if (_pinCallback) { _pinCallback(info.id); }
    });

    return {new CopyTextAction("Copy preview", info.textPreview), pin};
  }

  const QString &name() const { return info.textPreview; }

  void setPinCallback(const std::function<void(int)> &cb) { _pinCallback = cb; }

  ItemData data() const override {
    return {.iconUrl = BuiltinOmniIconUrl(info.isPinned ? "pin" : "copy-clipboard"),
            .name = info.textPreview};
  }

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
    auto &themeService = ThemeService::instance();
    std::vector<std::unique_ptr<OmniList::AbstractVirtualItem>> newList;
    auto &current = themeService.theme();
    list->clearSelection();

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

    newList.push_back(std::make_unique<OmniList::VirtualSection>("Pinned"));
    size_t i = 0;

    while (i < items.size() && items[i].isPinned) {
      auto &entry = items[i];
      auto candidate = std::make_unique<ClipboardHistoryItem>(entry);

      newList.push_back(std::move(candidate));
      ++i;
    }

    qDebug() << "i" << totalCount - i;

    newList.push_back(std::make_unique<OmniList::FixedCountSection>("History", totalCount - i));

    while (i < items.size()) {
      auto &entry = items[i];

      auto candidate = std::make_unique<ClipboardHistoryItem>(entry);

      candidate->setPinCallback([this, &entry, query](int id) {
        qDebug() << "pinned";
        list->clearSelection();
        list->invalidateCache(QString::number(id));
        generateList(query);
      });

      newList.push_back(std::move(candidate));
      ++i;
    }

    list->updateFromList(newList, OmniList::SelectionPolicy::SelectFirst);
  }

  void onMount() override { setSearchPlaceholderText("Manage themes..."); }

  void onSearchChanged(const QString &s) override { generateList(s); }

public:
  ClipboardHistoryCommand(AppWindow &app)
      : OmniListView(app), _clipboardService(service<ClipboardService>()) {}
};
