#include "dmenu-model.hpp"
#include "clipboard-actions.hpp"
#include "lib/fzf.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "template-engine/template-engine.hpp"
#include "ui/action-pannel/action.hpp"
#include <algorithm>
#include <filesystem>
#include <ranges>

void DMenuModel::setRawEntries(std::vector<std::string_view> entries) {
  m_entries = std::move(entries);
  m_filtered.clear();
  for (auto e : m_entries) {
    m_filtered.push_back({e, 0});
  }
  rebuildSections();
}

void DMenuModel::setFilter(const QString &text) {
  m_currentSearchText = text;
  std::string query = text.toStdString();

  m_filtered.clear();
  for (auto e : m_entries) {
    int score = fzf::defaultMatcher.fuzzy_match_v2_score_query(e, query);
    if (query.empty() || score > 0) {
      m_filtered.push_back({e, score});
    }
  }
  std::ranges::stable_sort(m_filtered, std::greater{});

  rebuildSections();
}

void DMenuModel::rebuildSections() {
  std::vector<SectionInfo> secs;
  if (!m_noSection) {
    secs.push_back({expandSectionName(m_filtered.size()), static_cast<int>(m_filtered.size())});
  } else {
    secs.push_back({{}, static_cast<int>(m_filtered.size())});
  }
  setSections(secs);
}

QString DMenuModel::expandSectionName(size_t count) const {
  TemplateEngine engine;
  engine.setVar("count", QString::number(count));
  return engine.build(QString::fromUtf8(m_sectionTemplate.data(), m_sectionTemplate.size()));
}

std::string_view DMenuModel::entryAt(int section, int item) const {
  if (item < 0 || item >= static_cast<int>(m_filtered.size())) return {};
  return m_filtered[item].data;
}

QString DMenuModel::itemTitle(int section, int item) const {
  auto entry = entryAt(section, item);
  if (entry.starts_with('/')) {
    auto pos = entry.rfind('/');
    if (pos != std::string_view::npos && pos + 1 < entry.size()) {
      return QString::fromUtf8(entry.substr(pos + 1));
    }
  }
  return QString::fromUtf8(entry.data(), entry.size());
}

QString DMenuModel::itemIconSource(int section, int item) const {
  auto entry = entryAt(section, item);
  if (entry.starts_with('/')) {
    std::error_code ec;
    if (std::filesystem::exists(entry, ec)) {
      return imageSourceFor(ImageURL::fileIcon(entry));
    }
  }
  return {};
}

void DMenuModel::selectEntry(const QString &text) const {
  emit const_cast<DMenuModel *>(this)->entryChosen(text);
  scope().closeWindow();
}

std::unique_ptr<ActionPanelState> DMenuModel::createActionPanel(int section, int item) const {
  auto entry = entryAt(section, item);
  if (entry.empty()) return nullptr;

  auto text = QString::fromUtf8(entry.data(), entry.size());
  auto panel = std::make_unique<ListActionPanelState>();
  auto *main = panel->createSection();

  main->addAction(
      new StaticAction("Select entry", ImageURL::builtin("save-document"),
                       [this, text](ApplicationContext *) { selectEntry(text); }));

  main->addAction(
      new StaticAction("Pass search text", ImageURL::builtin("save-document"),
                       [this](ApplicationContext *) { selectEntry(m_currentSearchText); }));

  auto *selectAndCopy = new StaticAction(
      "Select and copy entry", ImageURL::builtin("copy-clipboard"),
      [this, text](ApplicationContext *ctx) {
        ctx->services->clipman()->copyText(text);
        selectEntry(text);
      });
  selectAndCopy->setShortcut(Keybind::CopyAction);
  main->addAction(selectAndCopy);

  return panel;
}

void DMenuModel::onItemSelected(int section, int item) {
  auto entry = entryAt(section, item);
  if (!entry.empty()) {
    std::error_code ec;
    if (entry.starts_with('/') && std::filesystem::exists(entry, ec)) {
      emit fileHighlighted(entry);
    }
  }
}

void DMenuModel::onSelectionCleared() {
  auto panel = std::make_unique<ListActionPanelState>();
  auto *main = panel->createSection();
  main->addAction(
      new StaticAction("Pass search text", ImageURL::builtin("save-document"),
                       [this](ApplicationContext *) { selectEntry(m_currentSearchText); }));
  scope().setActions(std::move(panel));
}
