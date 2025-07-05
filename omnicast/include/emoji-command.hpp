#pragma once
#include "base-view.hpp"
#include "clipboard-actions.hpp"
#include "icon-browser-command.hpp"
#include "service-registry.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "omni-icon.hpp"
#include "services/emoji-service/emoji.hpp"
#include "services/toast/toast-service.hpp"
#include "timer.hpp"
#include "ui/action-pannel/action-item.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/image/omnimg.hpp"
#include "ui/omni-grid.hpp"
#include "ui/omni-list.hpp"
#include "ui/toast.hpp"
#include <QtConcurrent/qtconcurrentrun.h>
#include <memory>
#include <qevent.h>
#include <qfuturewatcher.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qobjectdefs.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <ranges>
#include <unistd.h>

class TestLongToast : public AbstractAction {
public:
  void execute() override {
    auto toastService = ServiceRegistry::instance()->toastService();
    auto toast = new Toast("Doing the thing...", ToastPriority::Info);

    toastService->registerToast(toast);
    auto fut = QtConcurrent::run([toast]() {
      for (int i = 0; i != 100; ++i) {
        QThread::msleep(200);
        toast->setTitle(QString("Running (%1%)").arg(i));
      }
      toast->setTitle("Done!");
      toast->setPriority(ToastPriority::Success);
      toast->update();
      QThread::msleep(2000);
      toast->close();
      toast->deleteLater();
    });
  }

  TestLongToast() : AbstractAction("Test long toast", BuiltinOmniIconUrl("copy-clipboard")) {}
};

class PushSelf : public AbstractAction {
  void execute() override {
    auto ui = ServiceRegistry::instance()->UI();

    ui->pushView(new IconBrowserView);
  }

  QString title() const override { return "Push icons"; }

public:
  PushSelf() : AbstractAction("Push icons", BuiltinOmniIconUrl("plus")) {}
};

class PasteEmojiAction : public PasteToFocusedWindowAction {
  std::string_view m_emoji;

public:
  void execute() override {
    auto emojiService = ServiceRegistry::instance()->emojiService();
    PasteToFocusedWindowAction::execute();
    emojiService->registerVisit(m_emoji);
  }

  PasteEmojiAction(std::string_view emoji)
      : PasteToFocusedWindowAction(Clipboard::Text(QString::fromUtf8(emoji.data(), emoji.size()))),
        m_emoji(emoji) {}
};

class PinEmojiAction : public AbstractAction {
  std::string_view m_emoji;

public:
  void execute() override { ServiceRegistry::instance()->emojiService()->pin(m_emoji); }

  OmniIconUrl icon() const override { return BuiltinOmniIconUrl("pin"); }
  QString title() const override { return "Pin emoji"; };

  PinEmojiAction(std::string_view emoji) : m_emoji(emoji) {}
};

class UnpinEmojiAction : public AbstractAction {
  std::string_view m_emoji;

public:
  void execute() override { ServiceRegistry::instance()->emojiService()->unpin(m_emoji); }

  OmniIconUrl icon() const override { return BuiltinOmniIconUrl("unpin"); }
  QString title() const override { return "Unpin emoji"; };

  UnpinEmojiAction(std::string_view emoji) : m_emoji(emoji) {}
};

class EmojiGridItem : public OmniGrid::AbstractGridItem, public GridView::Actionnable {
public:
  const EmojiData &info;
  bool m_pinned = false;

  QString tooltip() const override { return QString::fromUtf8(info.name.data(), info.name.size()); }

  QWidget *centerWidget() const override {
    auto icon = new Omnimg::ImageWidget();
    OmniIconUrl url;

    url.setType(OmniIconType::Emoji);
    url.setName(QString::fromStdString(std::string(info.emoji)));
    icon->setUrl(url);
    icon->setContentsMargins(10, 10, 10, 10);

    return icon;
  }

  bool centerWidgetRecyclable() const override { return true; }

  void recycleCenterWidget(QWidget *widget) const override {
    auto icon = static_cast<Omnimg::ImageWidget *>(widget);
    OmniIconUrl url;

    url.setType(OmniIconType::Emoji);
    url.setName(QString::fromStdString(std::string(info.emoji)));
    icon->setUrl(url);
    icon->setContentsMargins(10, 10, 10, 10);
  }

  QString generateId() const override { return QString::fromUtf8(info.emoji.data(), info.emoji.size()); }

  ActionPanelView *actionPanel() const override {
    auto panel = new ActionPanelStaticListView;
    auto paste = new PasteEmojiAction(info.emoji);
    auto copyName = new CopyToClipboardAction(
        Clipboard::Text(QString::fromUtf8(info.name.data(), info.name.size())), "Copy emoji name");
    auto copyGroup = new CopyToClipboardAction(
        Clipboard::Text(QString::fromUtf8(info.group.data(), info.group.size())), "Copy emoji group");

    paste->setPrimary(true);
    panel->addAction(paste);
    panel->addAction(copyName);
    panel->addAction(copyGroup);

    if (m_pinned) {
      panel->addAction(new UnpinEmojiAction(info.emoji));
    } else {
      panel->addAction(new PinEmojiAction(info.emoji));
    }

    return panel;
  }

  QString navigationTitle() const override { return ""; }

  EmojiGridItem(const EmojiData &info, bool pinned = false) : info(info), m_pinned(pinned) {}
};

class PinnedEmojiGridItem : public EmojiGridItem {
  QString generateId() const override { return QString("pinned.%1").arg(EmojiGridItem::generateId()); }
  using EmojiGridItem::EmojiGridItem;
};

class RecentlyUsedEmojiGridItem : public EmojiGridItem {
  QString generateId() const override { return QString("recent.%1").arg(EmojiGridItem::generateId()); }
  using EmojiGridItem::EmojiGridItem;
};

class EmojiView : public GridView {
public:
  void initialize() override {
    setSearchPlaceholderText("Search for emojis...");
    textChanged(searchText());
  }

  EmojiView() {}

  void textChanged(const QString &s) override {
    auto emojiService = ServiceRegistry::instance()->emojiService();

    m_grid->beginResetModel();

    auto makeEmojiItem = [](auto &&item) -> std::unique_ptr<OmniList::AbstractVirtualItem> {
      return std::make_unique<EmojiGridItem>(*item);
    };

    if (s.isEmpty()) {
      {

        Timer timer;
        auto visited = emojiService->getVisited();
        timer.time("Get visited");

        size_t i = 0;

        auto &pinnedSection = m_grid->addSection("Pinned");

        pinnedSection.setColumns(8);
        pinnedSection.setSpacing(10);

        while (i < visited.size() && visited[i].pinnedAt) {
          pinnedSection.addItem(std::make_unique<PinnedEmojiGridItem>(*visited[i].data, true));
          ++i;
        }

        auto &recentSection = m_grid->addSection("Recently used");

        recentSection.setColumns(8);
        recentSection.setSpacing(10);

        while (i < visited.size()) {
          recentSection.addItem(std::make_unique<RecentlyUsedEmojiGridItem>(*visited[i].data, false));
          ++i;
        }
      }

      std::unordered_map<std::string_view, std::vector<const EmojiData *>> sectionMap;
      std::vector<std::string_view> sectionNames;

      for (auto &item : StaticEmojiDatabase::orderedList()) {
        if (auto it = sectionMap.find(item.group); it == sectionMap.end()) {
          sectionMap.insert({item.group, {}});
          sectionNames.push_back(item.group);
        }

        sectionMap[item.group].push_back(&item);
      }

      for (const auto &name : sectionNames) {
        auto &section = m_grid->addSection(QString::fromStdString(std::string(name)));

        section.setColumns(8);
        section.setSpacing(10);

        auto items = sectionMap[name] | std::views::transform(makeEmojiItem) | std::ranges::to<std::vector>();

        section.addItems(std::move(items));
      }

      m_grid->endResetModel(OmniList::SelectFirst);
      return;
    }

    Timer timer;
    auto matches = emojiService->search(s.toStdString());
    timer.time("trie search");

    if (matches.empty()) {
      // TODO: show empty state
      m_grid->endResetModel(OmniList::SelectFirst);
      return;
    }

    auto &results = m_grid->addSection("Results");

    results.setColumns(8);
    results.setSpacing(10);

    auto items = matches | std::views::transform(makeEmojiItem) | std::ranges::to<std::vector>();

    results.addItems(std::move(items));

    m_grid->endResetModel(OmniList::SelectFirst);
  }
};
