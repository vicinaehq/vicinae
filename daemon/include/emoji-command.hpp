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
  std::vector<QSharedPointer<EmojiGridItem>> emojiItems;

public:
  EmojiView(AppWindow &app) : GridView(app), appDb(service<AppDatabase>()) {
    connect(grid, &VirtualGridWidget::selectionChanged, this, [this](const AbstractGridMember &item) {
      auto &emoji = static_cast<const EmojiGridItem &>(item);

      qDebug() << "selected emoji" << emoji.info.emoji;
    });
  }

  void onMount() override {
    for (const auto &emoji : emojiDb.list()) {
      emojiItems.push_back(QSharedPointer<EmojiGridItem>(new EmojiGridItem(emoji), &QObject::deleteLater));
    }
  }

  void onSearchChanged(const QString &s) override {
    grid->clearContents();

    if (s.isEmpty()) {
      QHash<QString, VirtualGridSection *> sectionMap;

      for (auto item : emojiItems) {
        auto section = sectionMap.value(item->info.category);

        if (!section) {
          section = grid->section(item->info.category);
          section->setColumns(8);
          section->setSpacing(10);
          sectionMap.insert(item->info.category, section);
        }

        section->addItem(item);
      }
    } else {
      auto results = grid->section("Results");

      results->setColumns(8);
      results->setSpacing(10);

      for (auto item : emojiItems) {
        if (QString(item->info.description).contains(s, Qt::CaseInsensitive)) { results->addItem(item); }
      }
    }

    grid->calculateLayout();
    grid->selectFirst();
  }
};
