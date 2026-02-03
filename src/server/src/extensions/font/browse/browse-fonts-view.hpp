#pragma once
#include "clipboard-actions.hpp"
#include "extensions/font/browse/font-list-model.hpp"
#include "layout.hpp"
#include "navigation-controller.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/markdown/markdown-renderer.hpp"
#include "ui/views/typed-list-view.hpp"
#include "config/config.hpp"

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

public:
  FontShowcaseWidget(QWidget *parent = nullptr) : QWidget(parent) { VStack().add(m_markdown).imbue(this); }

  void setFont(const QFont &font) {
    m_markdown->setFont(font);
    m_markdown->setMarkdown(loremIpsum);
  }

private:
  MarkdownRenderer *m_markdown = new MarkdownRenderer();
};

class SetAppFont : public AbstractAction {
  QFont m_font;

  void execute(ApplicationContext *ctx) override {
    ctx->services->config()->mergeWithUser(
        {.font = config::Partial<config::FontConfig>{.normal = {.family = m_font.family().toStdString()}}});
  }

public:
  SetAppFont(const QFont &font)
      : AbstractAction("Set as vicinae font", ImageURL::builtin("text")), m_font(font) {}
};

class BrowseFontsView : public TypedListView<FontListModel> {
public:
  void textChanged(const QString &text) override {
    m_model->setFilter(text);
    m_list->selectFirst();
  }

  void initialize() override {
    TypedListView::initialize();
    setModel(m_model);
    setSearchPlaceholderText("Browse fonts to preview...");
    textChanged(searchText());
  }

private:
  QWidget *generateDetail(const ItemType &item) const override {
    QFont family(qStringFromStdView(item));
    auto widget = new FontShowcaseWidget;
    widget->setFont(family);
    return widget;
  }

  std::unique_ptr<ActionPanelState> createActionPanel(const ItemType &item) const override {
    QString text = qStringFromStdView(item);
    auto panel = std::make_unique<ListActionPanelState>();
    auto section = panel->createSection();
    QFont family(text);
    auto copyFamily = new CopyToClipboardAction(Clipboard::Text(text), "Copy font family");
    auto setFont = new SetAppFont(family);

    copyFamily->setPrimary(true);
    section->addAction(copyFamily);
    section->addAction(setFont);

    return panel;
  }

  FontListModel *m_model = new FontListModel;
};
