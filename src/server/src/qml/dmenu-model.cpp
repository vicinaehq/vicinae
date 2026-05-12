#include "dmenu-model.hpp"
#include "clipboard-actions.hpp"
#include "fuzzy/fzf.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "template-engine/template-engine.hpp"
#include "ui/action-pannel/action.hpp"
#include "utils/utils.hpp"
#include <algorithm>
#include <filesystem>
#include <ranges>
#include <utility>

void DMenuSection::setRawEntries(std::vector<std::string_view> entries) {
  m_entries = std::move(entries);
  m_filtered.clear();
  for (auto e : m_entries) {
    m_filtered.push_back({e, 0});
  }
  notifyChanged();
}

void DMenuSection::setFilter(std::string_view query) {
  std::string const queryStr(query);
  m_currentSearchText = QString::fromUtf8(query.data(), query.size());

  m_filtered.clear();
  for (auto e : m_entries) {
    int const score = fzf::threadLocalMatcher().fuzzy_match_v2_score_query(e, queryStr);
    if (queryStr.empty() || score > 0) { m_filtered.push_back({e, score}); }
  }
  std::ranges::stable_sort(m_filtered, std::greater{});
}

QString DMenuSection::sectionName() const {
  if (m_noSection) return {};
  return expandSectionName(m_filtered.size());
}

QString DMenuSection::expandSectionName(size_t count) const {
  TemplateEngine engine;
  engine.setVar("count", QString::number(count));
  return engine.build(QString::fromUtf8(m_sectionTemplate.data(), m_sectionTemplate.size()));
}

std::string_view DMenuSection::entryAt(int i) const {
  if (i < 0 || std::cmp_greater_equal(i, m_filtered.size())) return {};
  return m_filtered[i].data;
}

QString DMenuSection::itemTitle(int i) const {
  auto entry = entryAt(i);
  if (entry.starts_with('/')) {
    return QString::fromStdString(getLastPathComponent(std::filesystem::path(entry)));
  }
  return QString::fromUtf8(entry.data(), entry.size());
}

QString DMenuSection::itemSubtitle(int i) const {
  if (!m_noQuickLook) return {};
  auto entry = entryAt(i);
  if (entry.starts_with('/')) {
    std::error_code ec;
    if (std::filesystem::exists(entry, ec)) {
      return QString::fromUtf8(std::filesystem::path(entry).parent_path().native());
    }
  }
  return {};
}

QString DMenuSection::itemIconSource(int i) const {
  auto entry = entryAt(i);
  if (entry.starts_with('/')) {
    std::error_code ec;
    if (std::filesystem::exists(entry, ec)) { return imageSourceFor(ImageURL::fileIcon(entry)); }
  }
  return {};
}

void DMenuSection::selectEntry(const QString &text) const {
  if (m_onEntryChosen) m_onEntryChosen(text);
  scope().closeWindow();
}

std::unique_ptr<ActionPanelState> DMenuSection::actionPanel(int i) const {
  auto entry = entryAt(i);
  if (entry.empty()) return nullptr;

  auto text = QString::fromUtf8(entry.data(), entry.size());
  auto panel = std::make_unique<ListActionPanelState>();
  auto *main = panel->createSection();

  main->addAction(new StaticAction("Select entry", ImageURL::builtin("save-document"),
                                   [this, text](ApplicationContext *) { selectEntry(text); }));

  main->addAction(new StaticAction("Pass search text", ImageURL::builtin("save-document"),
                                   [this](ApplicationContext *) { selectEntry(m_currentSearchText); }));

  auto *selectAndCopy = new StaticAction("Select and copy entry", ImageURL::builtin("copy-clipboard"),
                                         [this, text](ApplicationContext *ctx) {
                                           ctx->services->clipman()->copyText(text);
                                           selectEntry(text);
                                         });
  selectAndCopy->setShortcut(Keybind::CopyAction);
  main->addAction(selectAndCopy);

  return panel;
}

void DMenuSection::onSelected(int i) {
  auto entry = entryAt(i);
  if (!entry.empty()) {
    std::error_code ec;
    if (entry.starts_with('/') && std::filesystem::exists(entry, ec)) {
      if (m_onFileHighlighted) m_onFileHighlighted(entry);
    }
  }
}
