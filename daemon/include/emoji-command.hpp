#pragma once
#include "app-database.hpp"
#include "app.hpp"
#include "emoji-database.hpp"
#include "ui/grid-view.hpp"
#include "ui/omni-grid.hpp"
#include <memory>
#include <qlabel.h>
#include <qnamespace.h>
#include <qwidget.h>

class EmojiViewer : public QWidget {
  QString _emoji;

  void paintEvent(QPaintEvent *event) override {
    QPainter painter(this);
    QFont font = painter.font();
    font.setPointSize(height() * 0.8);
    painter.setFont(font);
    painter.drawText(rect(), Qt::AlignCenter, _emoji);
  }

  QSize sizeHint() const override { return {32, 32}; }

public:
  void setEmoji(const QString &emoji) {
    _emoji = emoji;
    update();
  }

  EmojiViewer(const QString &emoji) : _emoji(emoji) {}
};

class EmojiGridItem : public OmniGrid::AbstractGridItem {
  QString generateMarkup() const {
    return QString("<span style=\"font-size: 36px;\">%1</span>").arg(info.emoji);
  }

public:
  const EmojiInfo &info;

  QString tooltip() const override { return info.description; }

  QWidget *centerWidget() const override { return new EmojiViewer(info.emoji); }

  bool centerWidgetRecyclable() const override { return true; }

  void recycleCenterWidget(QWidget *widget) const override {
    auto label = static_cast<EmojiViewer *>(widget);

    label->setEmoji(info.emoji);
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
    auto start = std::chrono::high_resolution_clock::now();
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

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1e6;
    qDebug() << "emojiSearch()" << "=" << duration << "ms";

    grid->updateFromList(newItems, OmniGrid::SelectFirst);
  }
};
