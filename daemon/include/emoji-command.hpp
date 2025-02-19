#pragma once
#include "app-database.hpp"
#include "app.hpp"
#include "emoji-database.hpp"
#include "ui/grid-view.hpp"
#include "ui/virtual-grid.hpp"
#include <qlabel.h>
#include <qnamespace.h>

class EmojiGridItem : public AbstractGridItem {
public:
  const EmojiInfo &info;

  QString tooltip() const override { return info.description; }

  QWidget *centerWidget() const override {
    auto label = new QLabel(QString("<span style=\"font-size: 36px;\">%1</span>").arg(info.emoji));

    return label;
  }

  int key() const override { return qHash(info.emoji); }

  QList<AbstractAction *> createActions() const override {
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

  std::vector<EmojiGridItem *> emojiItems;

public:
  EmojiView(AppWindow &app) : GridView(app), appDb(service<AppDatabase>()) {
    widget = grid;
    grid->setColumns(8);

    connect(grid, &VirtualGridWidget::selectionChanged, this, [this](const AbstractGridMember &item) {
      auto &emoji = static_cast<const EmojiGridItem &>(item);

      qDebug() << "selected emoji" << emoji.info.emoji;
    });
  }

  ~EmojiView() {
    for (auto item : emojiItems) {
      item->deleteLater();
    }
  }

  void onMount() override {
    for (const auto &emoji : emojiDb.list()) {
      emojiItems.push_back(new EmojiGridItem(emoji));
    }
  }

  void onSearchChanged(const QString &s) override {
    grid->clear();

    if (s.isEmpty()) {
      QHash<QString, VirtualGridSection *> sectionMap;

      for (auto item : emojiItems) {
        auto section = sectionMap.value(item->info.category);

        if (!section) {
          section = grid->section(item->info.category);
          sectionMap.insert(item->info.category, section);
        }

        section->addItem(item);
      }
    } else {
      auto results = grid->section("Results");

      for (auto item : emojiItems) {
        if (QString(item->info.description).contains(s, Qt::CaseInsensitive)) { results->addItem(item); }
      }
    }

    grid->updateLayout();
    grid->setSelected(0);
  }
};
