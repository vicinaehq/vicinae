#include "ui/dmenu-view/dmenu-view.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "ui/dmenu-view/dmenu-model.hpp"
#include "ui/file-detail/file-detail.hpp"
#include "ui/views/typed-list-view.hpp"
#include "lib/fzf.hpp"
#include <filesystem>
#include <qwidget.h>
#include <ranges>

namespace DMenu {
View::View(Payload data) : m_data(data), m_model(new DMenuModel(this)) {
  m_entries = std::views::split(m_data.raw, std::string_view("\n")) |
              std::views::transform([](auto &&s) { return std::string_view(s); }) |
              std::ranges::to<std::vector>();
}

QWidget *View::generateDetail(const std::string_view &text) const {
  if (m_data.noQuickLook) return nullptr;
  std::error_code ec;

  if (text.starts_with('/') && std::filesystem::exists(text, ec)) {
    auto detail = new FileDetail;
    detail->setPath(text, !m_data.noMetadata);
    return detail;
  }

  return nullptr;
}

void View::hideEvent(QHideEvent *event) {
  popSelf();
  QWidget::hideEvent(event);
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
  TypedListView::initialize();
  setModel(m_model);
  if (m_data.noFooter) setStatusBarVisiblity(false);

  if (auto query = m_data.query) {
    setSearchText(m_data.query.value_or("").c_str());
  } else {
    textChanged("");
  }
}

void View::textChanged(const QString &text) {
  setFilter(text.toStdString());
  m_list->selectFirst();
}

void View::setFilter(std::string_view query) {
  fzf::search(m_entries, m_filteredEntries, query);

  if (!m_data.noSection) { updateSectionName(m_data.sectionTitle.value_or("Entries ({count})")); }

  m_model->setEntries(m_filteredEntries);
}

std::string View::expandSection(std::string_view name, size_t count) {
  TemplateEngine engine;
  engine.setVar("count", QString::number(count));
  return engine.build(QString::fromUtf8(name.data(), name.size())).toStdString();
}

void View::updateSectionName(std::string_view name) {
  m_sectionName = expandSection(name, m_filteredEntries.size());
  m_model->setSectionName(m_sectionName);
}

void View::itemSelected(const std::string_view &view) {
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

}; // namespace DMenu
