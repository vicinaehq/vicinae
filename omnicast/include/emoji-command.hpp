#pragma once
#include "base-view.hpp"
#include "clipboard-actions.hpp"
#include "clipboard/clipboard-service.hpp"
#include "emoji-database.hpp"
#include "libtrie/trie.hpp"
#include "omni-icon.hpp"
#include "timer.hpp"
#include "ui/action-pannel/action-item.hpp"
#include "ui/image/omnimg.hpp"
#include "ui/omni-grid.hpp"
#include "ui/omni-list.hpp"
#include <QtConcurrent/qtconcurrentrun.h>
#include <memory>
#include <qevent.h>
#include <qfuturewatcher.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qwidget.h>

class EmojiGridItem : public OmniGrid::AbstractGridItem, public GridView::Actionnable {
public:
  const EmojiInfo &info;

  QString tooltip() const override { return info.description; }

  QWidget *centerWidget() const override {
    auto icon = new Omnimg::ImageWidget();
    OmniIconUrl url;

    url.setType(OmniIconType::Emoji);
    url.setName(info.emoji);
    icon->setUrl(url);
    icon->setContentsMargins(10, 10, 10, 10);

    return icon;
  }

  bool centerWidgetRecyclable() const override { return true; }

  void recycleCenterWidget(QWidget *widget) const override {
    auto icon = static_cast<Omnimg::ImageWidget *>(widget);
    OmniIconUrl url;

    url.setType(OmniIconType::Emoji);
    url.setName(info.emoji);
    icon->setUrl(url);
    icon->setContentsMargins(10, 10, 10, 10);
  }

  QString generateId() const override { return info.description; }

  QList<AbstractAction *> generateActions() const override {
    return {
        new PasteToFocusedWindowAction(Clipboard::Text(info.emoji)),
        new CopyToClipboardAction(Clipboard::Text(info.description), "Copy emoji description"),
    };
  }

  QString navigationTitle() const override { return info.description; }

  EmojiGridItem(const EmojiInfo &info) : info(info) {}
};

struct EmojiInfoHash {
  size_t operator()(const EmojiInfo *const &info) { return std::hash<const char *>{}(info->emoji); }
};

class EmojiView : public GridView {
  EmojiDatabase emojiDb;
  std::vector<std::unique_ptr<OmniList::AbstractVirtualItem>> newItems;
  using TrieType = Trie<const EmojiInfo *, EmojiInfoHash>;
  std::unique_ptr<TrieType> m_searchTrie;

public:
  std::unique_ptr<TrieType> buildTrie() const {
    Timer timer;
    auto trie = std::make_unique<TrieType>();

    for (auto &item : emojiDb.list()) {
      trie->indexLatinText(item.description, &item);

      for (int i = 0; item.aliases[i]; ++i) {
        trie->index(item.aliases[i], &item);
      }

      for (int i = 0; item.tags[i]; ++i) {
        trie->index(item.tags[i], &item);
      }

      trie->indexLatinText(item.category, &item);
    }
    timer.time("emoji trie index");

    return trie;
  }

  EmojiView() {
    auto watcher = new QFutureWatcher<std::unique_ptr<TrieType>>;
    auto future = QtConcurrent::run([this]() { return buildTrie(); });

    setSearchPlaceholderText("Search for emojis...");

    watcher->setFuture(future);
    connect(watcher, &QFutureWatcher<TrieType>::finished, this, [this, watcher]() {
      m_searchTrie.reset(watcher->future().takeResult().release());
      watcher->deleteLater();
      onSearchChanged(searchText());
    });
  }

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
        newItems.push_back(
            std::make_unique<OmniGrid::GridSection>(name, m_grid->columns(), m_grid->spacing()));

        for (auto item : sectionMap[name]) {
          newItems.push_back(std::make_unique<EmojiGridItem>(*item));
        }
      }
    } else if (m_searchTrie) {
      newItems.push_back(
          std::make_unique<OmniGrid::GridSection>("Results", m_grid->columns(), m_grid->spacing()));

      Timer timer;
      auto results = m_searchTrie->prefixSearch(s.toStdString());
      timer.time("emoji searched");

      for (const auto &info : results) {
        newItems.push_back(std::make_unique<EmojiGridItem>(*info));
      }
    }

    m_grid->updateFromList(newItems, OmniGrid::SelectFirst);
  }
};
