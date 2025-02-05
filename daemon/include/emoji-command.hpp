#pragma once
#include "app-database.hpp"
#include "app.hpp"
#include "emoji-database.hpp"
#include "grid-view.hpp"
#include "ui/virtual-grid.hpp"
#include <qlabel.h>
#include <qnamespace.h>

class EmojiGridItem : public AbstractGridItem {
  const EmojiInfo &info;

public:
  QString tooltip() const override { return info.description; }

  QWidget *widget() const override {
    auto label = new QLabel(QString("<span style=\"font-size: 48px;\">%1</span>").arg(info.emoji));

    return label;
  }

  EmojiGridItem(const EmojiInfo &info) : info(info) {}
};

class EmojiView : public GridView {
  Service<AppDatabase> appDb;
  EmojiDatabase emojiDb;

public:
  EmojiView(AppWindow &app) : GridView(app), appDb(service<AppDatabase>()) {
    widget = grid;
    grid->setColumns(8);
  }

  void onSearchChanged(const QString &s) override {
    QList<AbstractGridItem *> items;

    for (const auto &emoji : emojiDb.list()) {
      if (QString(emoji.description).contains(s, Qt::CaseInsensitive)) { items << new EmojiGridItem(emoji); }
    }

    grid->setItems(items);
  }
};
