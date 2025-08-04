#pragma once
#include "common.hpp"
#include "ui/views/list-view.hpp"
#include "clipboard-actions.hpp"
#include "libtrie/trie.hpp"
#include "services/config/config-service.hpp"
#include "src/ui/image/url.hpp"
#include "service-registry.hpp"
#include "font-service.hpp"
#include "timer.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/markdown/markdown-renderer.hpp"
#include "ui/omni-list/omni-list.hpp"
#include <memory>
#include <qboxlayout.h>
#include <qfuture.h>
#include <qfuturewatcher.h>
#include <qnamespace.h>
#include <qvariant.h>
#include <qwidget.h>
#include <ranges>

static const QString loremIpsum = R"(
# Lorem Ipsum Font Showcase

## Standard Weight Text
Lorem ipsum dolor sit amet, consectetur adipiscing elit. Maecenas tempus, tellus eget condimentum rhoncus, sem quam semper libero, sit amet adipiscing sem neque sed ipsum. Nam quam nunc, blandit vel, luctus pulvinar, hendrerit id, lorem.

## Bold Text
**Lorem ipsum dolor sit amet, consectetur adipiscing elit. Maecenas tempus, tellus eget condimentum rhoncus, sem quam semper libero, sit amet adipiscing sem neque sed ipsum.**

## Italic Text
*Lorem ipsum dolor sit amet, consectetur adipiscing elit. Maecenas tempus, tellus eget condimentum rhoncus, sem quam semper libero, sit amet adipiscing sem neque sed ipsum.*

## Bold and Italic
***Lorem ipsum dolor sit amet, consectetur adipiscing elit. Maecenas tempus, tellus eget condimentum rhoncus.***

## Headings Showcase

# Heading 1
## Heading 2
### Heading 3
#### Heading 4
##### Heading 5
###### Heading 6

## Special Characters
Áéíóúñçäëïöüÿß — Test for extended character support and diacritics.

## Numbers & Symbols
0123456789 !@#$%^&*()_+-=[]{}|;':\",./<>?

## Monospace Text
`Lorem ipsum dolor sit amet, consectetur adipiscing elit. Perfect for code or technical content.`

## Pangrams (Tests All Letters)
The quick brown fox jumps over the lazy dog.
Pack my box with five dozen liquor jugs.

## Kerning & Tracking Test
AWAY AVATAR WAVY TAWNY

## Character Width Test
iiiiii mmmmmm

## Ligature Test
fi fl ffi ffl

## Sample Paragraph with Mixed Punctuation
Lorem ipsum dolor sit amet — consectetur adipiscing elit; Maecenas "tempus" tellus (eget) condimentum rhoncus? Sem quam semper libero! Sit amet adipiscing sem neque sed ipsum.

## Readability Test with Long Text
Nam eget dui. Etiam rhoncus. Maecenas tempus, tellus eget condimentum rhoncus, sem quam semper libero, sit amet adipiscing sem neque sed ipsum. Nam quam nunc, blandit vel, luctus pulvinar, hendrerit id, lorem. Maecenas nec odio et ante tincidunt tempus. Donec vitae sapien ut libero venenatis faucibus. Nullam quis ante. Etiam sit amet orci eget eros faucibus tincidunt. Duis leo. Sed fringilla mauris sit amet nibh. Donec sodales sagittis magna. Sed consequat, leo eget bibendum sodales, augue velit cursus nunc.

## Font Size Demonstration
<small>Small lorem ipsum dolor sit amet</small>

Normal lorem ipsum dolor sit amet

# Large lorem ipsum 

## Extra Small Text Test
<small>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Maecenas tempus, tellus eget condimentum rhoncus, sem quam semper libero, sit amet adipiscing sem neque sed ipsum.</small>

## Line Length Test (Good for Testing Readability)
Lorem ipsum dolor sit amet, consectetur adipiscing elit. Maecenas tempus, tellus eget condimentum rhoncus, sem quam semper libero, sit amet adipiscing sem neque sed ipsum. Nam quam nunc, blandit vel, luctus pulvinar, hendrerit id, lorem. Maecenas nec odio et ante tincidunt tempus. Donec vitae sapien ut libero venenatis faucibus. Nullam quis ante. Etiam sit amet orci eget eros faucibus tincidunt.

---

*This showcase includes various text formatting options to help evaluate font performance across different styles and contexts.*
)";

class FontShowcaseWidget : public QWidget {
  MarkdownRenderer *m_markdown = new MarkdownRenderer();

public:
  FontShowcaseWidget(QWidget *parent = nullptr) : QWidget(parent) {
    auto layout = new QVBoxLayout(this);

    layout->addWidget(m_markdown);
    setLayout(layout);
  }

  void setFont(const QFont &font) {
    m_markdown->setFont(font);
    m_markdown->setMarkdown(loremIpsum);
  }
};

class FontListItem : public AbstractDefaultListItem, public ListView::Actionnable {
  class SetAppFont : public AbstractAction {
    QFont m_font;

    void execute(ApplicationContext *ctx) override {
      ctx->services->config()->updateConfig(
          [&](ConfigService::Value &value) { value.font.normal = m_font.family(); });
    }

  public:
    SetAppFont(const QFont &font)
        : AbstractAction("Set as omnicast font", BuiltinOmniIconUrl("text")), m_font(font) {}
  };

  QString m_family;

public:
  QString generateId() const override { return m_family; }
  ItemData data() const override { return {.iconUrl = BuiltinOmniIconUrl("text"), .name = m_family}; }

  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx) const override {
    auto panel = std::make_unique<ActionPanelState>();
    auto section = panel->createSection();

    auto copyFamily = new CopyToClipboardAction(Clipboard::Text(m_family), "Copy font family");
    auto setFont = new SetAppFont(m_family);

    copyFamily->setPrimary(true);
    section->addAction(copyFamily);
    section->addAction(setFont);

    return panel;
  }

  QWidget *generateDetail() const override {
    auto widget = new FontShowcaseWidget;

    widget->setFont(m_family);

    return widget;
  }

  QString navigationTitle() const override { return m_family; }

  FontListItem(const QString &family) : m_family(family) {}
};

class BrowseFontsView : public ListView {
  std::unique_ptr<Trie<QString>> m_trie;

  QFuture<std::unique_ptr<Trie<QString>>> buildTrieAsync() {
    auto fontService = ServiceRegistry::instance()->fontService();

    return QtConcurrent::run(
        [&fontService]() { return std::make_unique<Trie<QString>>(fontService->buildFontSearchIndex()); });
  }

public:
  void renderEmptySearch() {
    m_list->beginResetModel();

    for (const auto &system : QFontDatabase::writingSystems()) {
      QString sname = QFontDatabase::writingSystemName(system);
      auto &section = m_list->addSection(QString("%1 Fonts").arg(sname));
      auto nonWide = [](const QString &name) { return !name.contains("wide", Qt::CaseInsensitive); };
      auto items =
          QFontDatabase::families(system) | std::views::filter(nonWide) |
          std::views::transform([](const QString &family) { return std::make_unique<FontListItem>(family); });

      for (auto item : items) {
        section.addItem(std::move(item));
      }
    }

    m_list->endResetModel(OmniList::SelectFirst);
  }

  void render(const QString &s) {
    QString query = s.trimmed();

    if (query.isEmpty()) return renderEmptySearch();
    if (!m_trie) return;

    m_list->beginResetModel();
    Timer timer;
    auto results = m_trie->prefixSearch(query.toStdString(), 500);
    auto nonWide = [](const QString &name) { return !name.contains("wide", Qt::CaseInsensitive); };
    timer.time("font search");

    auto &section = m_list->addSection("Fonts");
    auto items = results | std::views::filter(nonWide) | std::views::transform([](const QString &family) {
                   return std::make_unique<FontListItem>(family);
                 });

    for (auto item : items) {
      section.addItem(std::move(item));
    }
    m_list->endResetModel(OmniList::SelectFirst);
  }

  void textChanged(const QString &text) override { render(text); }

  void initialize() override { setSearchPlaceholderText("Browse fonts to preview..."); }

  BrowseFontsView() {
    auto watcher = QSharedPointer<QFutureWatcher<std::unique_ptr<Trie<QString>>>>::create();

    watcher->setFuture(buildTrieAsync());
    connect(watcher.get(), &QFutureWatcher<Trie<QString>>::finished, this, [this, watcher]() {
      m_trie = watcher->future().takeResult();
      render(searchText());
    });
  }
};
