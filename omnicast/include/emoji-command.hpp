#pragma once
#include "app/app-database.hpp"
#include "app.hpp"
#include "emoji-database.hpp"
#include "ui/action-pannel/action-item.hpp"
#include "ui/omni-grid-view.hpp"
#include "ui/emoji-viewer.hpp"
#include "ui/omni-grid.hpp"
#include "ui/omni-list.hpp"
#include <memory>
#include <qevent.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qwidget.h>

class EmojiGridItem : public OmniGrid::AbstractGridItem, public OmniGridView::IActionnable {
public:
  const EmojiInfo &info;

  QString tooltip() const override { return info.description; }

  QWidget *centerWidget() const override {
    auto emoji = new EmojiViewer(info.emoji);

    emoji->setHeightScale(0.4);

    return emoji;
  }

  bool centerWidgetRecyclable() const override { return true; }

  void recycleCenterWidget(QWidget *widget) const override {
    auto label = static_cast<EmojiViewer *>(widget);

    label->setEmoji(info.emoji);
  }

  QString id() const override { return info.description; }

  QList<AbstractAction *> generateActions() const override {
    return {
        new CopyTextAction("Copy emoji", info.emoji),
        new CopyTextAction("Copy emoji name", info.description),
    };
  }

  std::vector<ActionItem> generateActionPannel() const override {
    std::vector<ActionItem> items;

    items.push_back(QString("Emoji - %1").arg(info.description));
    items.push_back(std::make_unique<CopyTextAction>("Copy emoji", info.emoji));
    items.push_back(std::make_unique<CopyTextAction>("Copy emoji name", info.description));

    return items;
  }

  QString navigationTitle() const override { return info.description; }

  EmojiGridItem(const EmojiInfo &info) : info(info) {}
};

class EmojiView : public OmniGridView {
  Service<AbstractAppDatabase> appDb;
  EmojiDatabase emojiDb;
  std::vector<std::unique_ptr<OmniList::AbstractVirtualItem>> newItems;

public:
  EmojiView(AppWindow &app) : OmniGridView(app), appDb(service<AbstractAppDatabase>()) {}

  void onSearchChanged(const QString &s) override {
    newItems.clear();

    if (s.isEmpty()) {
      std::unordered_map<QString, std::vector<const EmojiInfo *>> sectionMap;
      std::vector<QString> sectionNames;

      for (auto &item : emojiDb.list()) {
        if (auto it = sectionMap.find(item.category); it == sectionMap.end()) {
          sectionMap.insert({item.category, {}});
          sectionNames.push_back(item.category);
        }

        sectionMap[item.category].push_back(&item);
      }

      for (const auto &name : sectionNames) {
        newItems.push_back(std::make_unique<OmniGrid::GridSection>(name, grid->columns(), grid->spacing()));

        for (auto item : sectionMap[name]) {
          newItems.push_back(std::make_unique<EmojiGridItem>(*item));
        }
      }
    } else {
      newItems.push_back(
          std::make_unique<OmniGrid::GridSection>("Results", grid->columns(), grid->spacing()));

      for (auto &item : emojiDb.list()) {
        if (QString(item.description).contains(s, Qt::CaseInsensitive)) {
          newItems.push_back(std::make_unique<EmojiGridItem>(item));
        }
      }
    }

    grid->updateFromList(newItems, OmniGrid::SelectFirst);
  }
};
