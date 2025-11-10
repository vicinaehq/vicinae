#include "ui/dmenu-view/dmenu-view.hpp"
#include "fuzzy/weighted-fuzzy-scorer.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "ui/omni-list/omni-list.hpp"
#include <ranges>

class Item : public AbstractDefaultListItem {
public:
  Item(std::string_view text) : m_text(text) {}

  QString generateId() const override { return QUuid::createUuid().toString(); };
  ItemData data() const override {
    return {
        .name = QString::fromUtf8(m_text.data(), m_text.size()),
    };
  }
  std::string_view text() const { return m_text; }

private:
  std::string_view m_text;
};

DMenuListView::DMenuListView(DmenuPayload data) : m_data(data) {
  m_lines = m_data.raw | std::views::split(std::string_view{"\n"}) |
            std::views::transform([](auto &&s) { return std::string_view{s}; }) |
            std::views::filter([](std::string_view view) { return !view.empty(); }) |
            std::ranges::to<std::vector>();
  m_scoredItems.reserve(m_lines.size());
}

void DMenuListView::hideEvent(QHideEvent *event) {
  popSelf();
  QWidget::hideEvent(event);
}

QString DMenuListView::initialNavigationTitle() const { return m_data.navigationTitle.c_str(); }

QString DMenuListView::initialSearchPlaceholderText() const {
  return m_data.placeholder.empty() ? "Search entries..." : m_data.placeholder.c_str();
}

void DMenuListView::beforePop() {
  if (!m_selected) { emit selected(""); }
}

void DMenuListView::initialize() { textChanged(""); }

void DMenuListView::textChanged(const QString &text) {
  std::string pattern = text.toStdString();
  auto score = [&](std::string_view view) {
    fuzzy::WeightedScorer scorer;
    scorer.add(std::string(view));
    return scorer.score(pattern);
  };

  auto filtered = m_lines | std::views::transform([&](auto &&s) { return std::pair{s, score(s)}; }) |
                  std::views::filter([&](auto &&p) { return pattern.empty() || p.second > 0; });

  m_scoredItems.clear();
  std::ranges::copy(filtered, std::back_inserter(m_scoredItems));
  std::ranges::stable_sort(m_scoredItems, [](auto &&a, auto &&b) { return a.second > b.second; });

  m_list->updateModel([&]() {
    auto &section = m_list->addSection();
    for (const auto &[view, score] : m_scoredItems) {
      section.addItem(std::make_shared<Item>(view));
    }
  });
}

void DMenuListView::itemSelected(const OmniList::AbstractVirtualItem *item) {
  std::string_view view = static_cast<const Item *>(item)->text();
  auto text = QString::fromUtf8(view.data(), view.size());
  auto panel = std::make_unique<ListActionPanelState>();
  auto main = panel->createSection();
  auto select = new StaticAction("Select entry", ImageURL::builtin("save-document"),
                                 [this, text]() { selectEntry(text); });
  auto selectSearchText = new StaticAction("Pass search text", ImageURL::builtin("save-document"),
                                           [this, text]() { selectEntry(searchText()); });

  auto selectAndCopy = new StaticAction("Select and copy entry", ImageURL::builtin("copy-clipboard"),
                                        [this, text](ApplicationContext *ctx) {
                                          ctx->services->clipman()->copyText(text);
                                          selectEntry(text);
                                        });
  selectAndCopy->setShortcut(Keybind::CopyAction);

  main->addAction(select);
  main->addAction(selectSearchText);
  main->addAction(selectAndCopy);
  setActions(std::move(panel));
}

void DMenuListView::selectEntry(const QString &text) {
  m_selected = true;
  emit selected(text);
  context()->navigation->closeWindow();
}
