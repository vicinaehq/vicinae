#pragma once
#include "app-database.hpp"
#include "app.hpp"
#include "emoji-database.hpp"
#include "grid-view.hpp"
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

  EmojiGridItem(const EmojiInfo &info) : info(info) {}
};

class EmojiView : public GridView {
  Service<AppDatabase> appDb;
  EmojiDatabase emojiDb;

public:
  EmojiView(AppWindow &app) : GridView(app), appDb(service<AppDatabase>()) {
    widget = grid;
    grid->setColumns(8);

    connect(grid, &VirtualGridWidget::selectionChanged, this, [this](const AbstractGridMember &item) {
      auto emoji = static_cast<const EmojiGridItem &>(item);

      qDebug() << "selected emoji" << emoji.info.emoji;
    });
  }

  void onSearchChanged(const QString &s) override {
    QList<EmojiGridItem *> items;

    for (const auto &emoji : emojiDb.list()) {
      if (QString(emoji.description).contains(s, Qt::CaseInsensitive)) { items << new EmojiGridItem(emoji); }
    }

    QList<VirtualGridSection> sections;
    QHash<QString, VirtualGridSection *> sectionMap;

    for (auto item : items) {
      auto section = sectionMap.value(item->info.category);

      if (!section) {
        sections << VirtualGridSection(item->info.category);
        section = &sections[sections.size() - 1];
        sectionMap.insert(item->info.category, section);
      }

      section->addItem(item);
    }

    grid->setSections(sections);
  }
};
