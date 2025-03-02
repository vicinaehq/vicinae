#pragma once
#include "app-database.hpp"
#include "app.hpp"
#include "emoji-database.hpp"
#include "ui/grid-view.hpp"
#include "ui/omni-grid.hpp"
#include "ui/virtual-grid.hpp"
#include <memory>
#include <qlabel.h>
#include <qnamespace.h>

class EmojiGridItem : public OmniGrid::AbstractGridItem {
public:
  const EmojiInfo &info;

  QString tooltip() const override { return info.description; }

  QWidget *centerWidget() const override {
    auto label = new QLabel(QString("<span style=\"font-size: 36px;\">%1</span>").arg(info.emoji));

    return label;
  }

  QString id() const override { return info.description; }

  QList<AbstractAction *> createActions() const {
    return {
        new CopyTextAction("Copy emoji", info.emoji),
        new CopyTextAction("Copy emoji name", info.description),
    };
  }

  EmojiGridItem(const EmojiInfo &info) : info(info) {}
};

class EmojiView : public GridView {
  Service<AppDatabase> appDb;
  EmojiDatabase emojiDb;
  OmniGrid *grid;
  std::vector<std::unique_ptr<OmniList::AbstractVirtualItem>> newItems;

public:
  EmojiView(AppWindow &app) : GridView(app), grid(new OmniGrid), appDb(service<AppDatabase>()) {
    widget = grid;
  }

  void onMount() override {
    grid->setColumns(8);
    grid->setSpacing(10);
    grid->setMargins(20, 10, 20, 10);
  }

  void onSearchChanged(const QString &s) override {
    newItems.clear();

    if (s.isEmpty()) {
      std::unordered_map<QString, std::vector<const EmojiInfo *>> sectionMap;

      for (auto &item : emojiDb.list()) {
        auto &list = sectionMap[item.category];

        list.push_back(&item);
      }

      for (auto &[name, items] : sectionMap) {
        newItems.push_back(std::make_unique<OmniGrid::GridSection>(name, grid->columns(), grid->spacing()));

        for (auto item : items) {
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
