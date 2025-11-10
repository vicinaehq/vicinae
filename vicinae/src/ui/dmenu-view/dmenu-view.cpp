#include "ui/dmenu-view/dmenu-view.hpp"
#include "fuzzy/weighted-fuzzy-scorer.hpp"
#include "proto/daemon.pb.h"
#include "services/clipboard/clipboard-service.hpp"
#include "ui/file-detail/file-detail.hpp"
#include "ui/omni-list/omni-list.hpp"
#include <filesystem>
#include <qwidget.h>
#include <ranges>

namespace DMenu {

class Item : public AbstractDefaultListItem, public ListView::Actionnable {
public:
  Item(std::string_view text) : m_text(text) {
    if (text.starts_with('/')) {
      std::filesystem::path path(text);
      std::error_code ec;
      if (std::filesystem::exists(path, ec)) { m_isFile = true; };
    }
  }

  QString generateId() const override { return QString::fromUtf8(m_text.data(), m_text.size()); };

  ItemData data() const override {
    ItemData data;
    data.name = displayName();

    if (m_isFile) { data.iconUrl = ImageURL::mimeType(m_text); }

    return data;
  }

  std::string_view text() const { return m_text; }

  void setMetadata(bool value) { m_metadata = value; }
  void setQuickLook(bool value) { m_quickLook = value; }

  QWidget *generateDetail() const override {
    if (!m_isFile || !m_quickLook) return nullptr;
    auto detail = new FileDetail;
    detail->setPath(m_text, m_metadata);
    return detail;
  }

private:
  QString displayName() const {
    if (m_isFile && m_quickLook) { return std::filesystem::path(m_text).filename().c_str(); }
    return QString::fromUtf8(m_text.data(), m_text.size());
  }

  std::string_view m_text;
  bool m_isFile = false;
  bool m_quickLook = true;
  bool m_metadata = true;
};

View::View(Payload data) : m_data(data) {
  auto lines = m_data.raw | std::views::split(std::string_view{"\n"}) |
               std::views::transform([](auto &&s) { return std::string_view{s}; }) |
               std::views::filter([](std::string_view view) { return !view.empty(); });
  std::unordered_set<std::string_view> seen;

  seen.reserve(100);
  m_lines.reserve(100);
  for (const auto &line : lines) {
    if (seen.contains(line)) continue;
    m_lines.emplace_back(line);
    seen.insert(line);
  }

  m_scoredItems.reserve(m_lines.size());
}

void View::hideEvent(QHideEvent *event) {
  popSelf();
  QWidget::hideEvent(event);
}

QString View::expandSectionName(size_t count) const {
  if (m_data.noSection) return "";
  TemplateEngine engine;
  engine.setVar("count", QString::number(count));
  return engine.build(m_data.sectionTitle.value_or("Entries ({count})").c_str());
}

QString View::initialNavigationTitle() const { return m_data.navigationTitle.value_or("").c_str(); }

QString View::initialSearchPlaceholderText() const {
  return m_data.placeholder.value_or("Search entries...").c_str();
}

void View::beforePop() {
  if (!m_selected) { emit selected(""); }
}

void View::emptied() {
  auto panel = std::make_unique<ListActionPanelState>();
  auto main = panel->createSection();
  auto selectSearchText = new StaticAction("Pass search text", ImageURL::builtin("save-document"),
                                           [this]() { selectEntry(searchText()); });
  main->addAction(selectSearchText);
  setActions(std::move(panel));
}

void View::initialize() {
  if (auto query = m_data.query) {
    setSearchText(m_data.query.value_or("").c_str());
  } else {
    textChanged("");
  }
}

void View::textChanged(const QString &text) {
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
    auto name = expandSectionName(m_scoredItems.size());
    auto &section = m_list->addSection(name);

    for (const auto &[view, score] : m_scoredItems) {
      auto item = std::make_shared<Item>(view);
      item->setQuickLook(!m_data.noQuickLook);
      item->setMetadata(!m_data.noMetadata);
      section.addItem(item);
    }
  });
}

void View::itemSelected(const OmniList::AbstractVirtualItem *item) {
  std::string_view view = static_cast<const Item *>(item)->text();
  auto text = QString::fromUtf8(view.data(), view.size());
  auto panel = std::make_unique<ListActionPanelState>();
  auto main = panel->createSection();
  auto select = new StaticAction("Select entry", ImageURL::builtin("save-document"),
                                 [this, text]() { selectEntry(text); });

  main->addAction(select);

  auto selectSearchText = new StaticAction("Pass search text", ImageURL::builtin("save-document"),
                                           [this]() { selectEntry(searchText()); });
  main->addAction(selectSearchText);

  auto selectAndCopy = new StaticAction("Select and copy entry", ImageURL::builtin("copy-clipboard"),
                                        [this, text](ApplicationContext *ctx) {
                                          ctx->services->clipman()->copyText(text);
                                          selectEntry(text);
                                        });
  selectAndCopy->setShortcut(Keybind::CopyAction);

  main->addAction(selectAndCopy);
  setActions(std::move(panel));
}

void View::selectEntry(const QString &text) {
  m_selected = true;
  emit selected(text);
  context()->navigation->closeWindow();
}

Payload Payload::fromProto(const proto::ext::daemon::DmenuRequest &req) {
  auto nullable = [](const std::string &str) -> std::optional<std::string> {
    if (str.empty()) return std::nullopt;
    return str;
  };
  Payload payload;
  payload.raw = req.raw_content();
  payload.placeholder = nullable(req.placeholder());
  payload.sectionTitle = nullable(req.section_title());
  payload.navigationTitle = nullable(req.navigation_title());
  payload.query = nullable(req.query());
  payload.noSection = req.no_section();
  payload.noQuickLook = req.no_quick_look();
  payload.noMetadata = req.no_metadata();
  return payload;
}

proto::ext::daemon::DmenuRequest Payload::toProto() const {
  proto::ext::daemon::DmenuRequest req;
  req.set_raw_content(raw);
  req.set_query(query.value_or(""));
  req.set_placeholder(placeholder.value_or(""));
  req.set_navigation_title(navigationTitle.value_or(""));
  req.set_section_title(sectionTitle.value_or(""));
  req.set_no_section(noSection);
  req.set_no_quick_look(noQuickLook);
  req.set_no_metadata(noMetadata);
  return req;
}

}; // namespace DMenu
