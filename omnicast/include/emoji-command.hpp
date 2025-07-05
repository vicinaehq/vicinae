#pragma once
#include "base-view.hpp"
#include "clipboard-actions.hpp"
#include "icon-browser-command.hpp"
#include "service-registry.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "omni-icon.hpp"
#include "services/emoji-service/emoji.hpp"
#include "services/toast/toast-service.hpp"
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

class EmojiGridItem : public OmniGrid::AbstractGridItem, public GridView::Actionnable {
public:
  const EmojiData &info;

  QString tooltip() const override { return ""; }

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

  QString generateId() const override { return QString::fromStdString(std::string(info.emoji)); }

  QList<AbstractAction *> generateActions() const override {
    auto emoji = QString::fromStdString(std::string(info.emoji));
    auto paste = new PasteToFocusedWindowAction(Clipboard::Text(emoji));

    paste->setPrimary(true);

    return {paste, new CopyToClipboardAction(Clipboard::Text(""), "Copy emoji description"),
            new TestLongToast(), new PushSelf};
  }

  QString navigationTitle() const override { return ""; }

  EmojiGridItem(const EmojiData &info) : info(info) {}
};

class EmojiView : public GridView {
public:
  void initialize() override {
    setSearchPlaceholderText("Search for emojis...");
    textChanged(searchText());
  }

  EmojiView() {}

  void textChanged(const QString &s) override {
    m_grid->beginResetModel();

    auto makeEmojiItem = [](auto &&item) -> std::unique_ptr<OmniList::AbstractVirtualItem> {
      return std::make_unique<EmojiGridItem>(*item);
    };

    if (s.isEmpty()) {
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

    auto emojiService = ServiceRegistry::instance()->emojiService();
    auto matches = emojiService->search(s.toStdString());

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
