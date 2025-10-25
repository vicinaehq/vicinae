#include "ui/dmenu-view/dmenu-view.hpp"
#include "services/clipboard/clipboard-service.hpp"

class Item : public SearchableListView::Actionnable {
public:
  Item(const QString &text) : m_text(text) {}

  QString generateId() const override { return QUuid::createUuid().toString(); };
  std::vector<QString> searchStrings() const override { return {m_text}; }
  ItemData data() const override {
    return {
        .name = m_text,
    };
  }
  const QString &text() const { return m_text; }

private:
  QString m_text;
};

DMenuListView::DMenuListView(DmenuPayload data) : m_data(data) {}

void DMenuListView::hideEvent(QHideEvent *event) {
  popSelf();
  QWidget::hideEvent(event);
}

QString DMenuListView::initialNavigationTitle() const { return m_data.navigationTitle.c_str(); }

QString DMenuListView::initialSearchPlaceholderText() const {
  return m_data.placeholder.empty() ? "Search entries..." : m_data.placeholder.c_str();
}

QString DMenuListView::sectionName() const {
  if (m_data.noSection) return QString();
  return m_data.sectionTitle.empty() ? "Entries ({count})" : m_data.sectionTitle.c_str();
}

void DMenuListView::beforePop() {
  if (!m_selected) { emit selected(""); }
}

void DMenuListView::itemSelected(const OmniList::AbstractVirtualItem *item) {
  QString text = static_cast<const Item *>(item)->text();
  auto panel = std::make_unique<ListActionPanelState>();
  auto main = panel->createSection();
  auto select = new StaticAction("Select entry", ImageURL::builtin("save-document"),
                                 [this, text]() { selectEntry(text); });
  auto selectAndCopy = new StaticAction("Select and copy entry", ImageURL::builtin("copy-clipboard"),
                                        [this, text](ApplicationContext *ctx) {
                                          ctx->services->clipman()->copyText(text);
                                          selectEntry(text);
                                        });

  main->addAction(select);
  main->addAction(selectAndCopy);
  setActions(std::move(panel));
}

DMenuListView::Data DMenuListView::initData() const {
  Data data;
  auto source = QString::fromStdString(m_data.raw);
  auto lines = source.split('\n', Qt::SkipEmptyParts);
  auto trim = [](const QString &s) { return s.trimmed(); };
  auto filter = [](const QString &s) { return !s.isEmpty(); };

  data.reserve(lines.size());

  for (const auto &menu : lines | std::views::transform(trim) | std::views::filter(filter)) {
    data.emplace_back(std::make_shared<Item>(menu));
  }

  return data;
}

void DMenuListView::selectEntry(const QString &text) {
  m_selected = true;
  emit selected(text);
  context()->navigation->closeWindow();
}
