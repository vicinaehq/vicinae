#include "font-browser-view-host.hpp"
#include "font-browser-model.hpp"
#include "clipboard-actions.hpp"
#include "config/config.hpp"
#include "view-utils.hpp"
#include "service-registry.hpp"
#include <QFontDatabase>

namespace {

class SetAppFont : public AbstractAction {
  QString m_family;

  void execute(ApplicationContext *ctx) override {
    ctx->services->config()->mergeWithUser(
        {.font = config::Partial<config::FontConfig>{.normal = {.family = m_family.toStdString()}}});
  }

public:
  SetAppFont(const QString &family)
      : AbstractAction("Set as vicinae font", ImageURL::builtin("text")), m_family(family) {}
};

const QString showcaseMarkdownContent = QStringLiteral(R"(
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

## Special Characters
Áéíóúñçäëïöüÿß — Test for extended character support and diacritics.

## Numbers & Symbols
0123456789 !@#$%^&*()_+-=[]{}|;':",./<>?

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

## Line Length Test (Good for Testing Readability)
Lorem ipsum dolor sit amet, consectetur adipiscing elit. Maecenas tempus, tellus eget condimentum rhoncus, sem quam semper libero, sit amet adipiscing sem neque sed ipsum. Nam quam nunc, blandit vel, luctus pulvinar, hendrerit id, lorem. Maecenas nec odio et ante tincidunt tempus. Donec vitae sapien ut libero venenatis faucibus. Nullam quis ante. Etiam sit amet orci eget eros faucibus tincidunt.

---

*This showcase includes various text formatting options to help evaluate font performance across different styles and contexts.*
)");

} // namespace

QString FontBrowserModel::displayTitle(const QString &item) const { return item; }

QString FontBrowserModel::displayIconSource(const QString &item) const {
  Q_UNUSED(item)
  return qml::imageSourceFor(ImageURL::builtin("text"));
}

std::unique_ptr<ActionPanelState> FontBrowserModel::buildActionPanel(const QString &item) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto section = panel->createSection();

  auto copyFamily = new CopyToClipboardAction(Clipboard::Text(item), "Copy font family");
  copyFamily->setPrimary(true);
  section->addAction(copyFamily);
  section->addAction(new SetAppFont(item));

  return panel;
}

QUrl FontBrowserViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/BrowseFontsView.qml"));
}

QVariantMap FontBrowserViewHost::qmlProperties() const {
  return {{QStringLiteral("host"), QVariant::fromValue(const_cast<FontBrowserViewHost *>(this))}};
}

void FontBrowserViewHost::initialize() {
  BaseView::initialize();

  m_model = new FontBrowserModel(this);
  m_model->setScope(ViewScope(context(), this));
  m_model->initialize();

  setSearchPlaceholderText("Browse fonts to preview...");

  connect(m_model, &FontBrowserModel::fontSelected, this, [this](const QString &family) {
    m_selectedFont = family;
    emit selectedFontChanged();
  });
}

void FontBrowserViewHost::loadInitialData() {
  auto families = QFontDatabase::families();
  std::vector<QString> items(families.begin(), families.end());
  m_model->setItems(std::move(items));
}

void FontBrowserViewHost::textChanged(const QString &text) { m_model->setFilter(text); }

void FontBrowserViewHost::onReactivated() { m_model->refreshActionPanel(); }

void FontBrowserViewHost::beforePop() { m_model->beforePop(); }

QObject *FontBrowserViewHost::listModel() const { return m_model; }

QString FontBrowserViewHost::showcaseMarkdown() const { return showcaseMarkdownContent; }
