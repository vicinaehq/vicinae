#include "dmenu-model.hpp"
#include "common/enumerate.hpp"
#include "fuzzy/fuzzy-searchable.hpp"
#include "service-registry.hpp"
#include "template-engine/template-engine.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "ui/action-pannel/action.hpp"
#include "utils/utils.hpp"
#include <algorithm>
#include <filesystem>
#include <ranges>
#include <utility>

void DMenuSection::setRawEntries(std::vector<std::string_view> entries) {
  m_entries = std::move(entries);
  m_filtered.clear();
  for (auto [idx, e] : vicinae::enumerate(m_entries)) {
    m_filtered.push_back({{e, idx}, 0});
  }
  notifyChanged();
}

void DMenuSection::setFilter(std::string_view query) {
  std::string const queryStr(query);
  m_currentSearchText = QString::fromUtf8(query.data(), query.size());

  m_filtered.clear();
  fuzzy::Query const fuzzyQuery{queryStr};
  for (auto [idx, e] : vicinae::enumerate(m_entries)) {
    auto const m = fuzzy::scoreWeighted({{e, 1.0}}, fuzzyQuery);
    if (queryStr.empty() || m.accepted()) { m_filtered.push_back({{e, idx}, m.score}); }
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

DMenuSection::IndexedData DMenuSection::entryAt(int i) const {
  if (i < 0 || std::cmp_greater_equal(i, m_filtered.size())) return {};
  return m_filtered[i].data;
}

QString DMenuSection::itemTitle(int i) const {
  auto entry = entryAt(i).first;
  if (entry.starts_with('/')) {
    return QString::fromStdString(getLastPathComponent(std::filesystem::path(entry)));
  }
  return QString::fromUtf8(entry.data(), entry.size());
}

QString DMenuSection::itemSubtitle(int i) const {
  if (!m_noQuickLook) return {};
  auto entry = entryAt(i).first;
  if (entry.starts_with('/')) {
    std::error_code ec;
    if (std::filesystem::exists(entry, ec)) {
      return QString::fromStdString(std::filesystem::path(entry).parent_path().string());
    }
  }
  return {};
}

std::optional<ImageURL> DMenuSection::itemIcon(int i) const {
  auto entry = entryAt(i).first;
  if (entry.starts_with('/')) {
    std::error_code ec;
    if (std::filesystem::exists(entry, ec)) { return ImageURL::fileIcon(entry); }
  }
  return std::nullopt;
}

void DMenuSection::selectEntry(const QString &text) const {
  if (m_onEntryChosen) m_onEntryChosen(text);
  scope().closeWindow();
}

std::unique_ptr<ActionPanelState> DMenuSection::actionPanel(int i) const {
  auto [entry, idx] = entryAt(i);
  if (entry.empty()) return nullptr;

  auto text = QString::fromUtf8(entry.data(), entry.size());
  auto panel = std::make_unique<ListActionPanelState>();
  auto *main = panel->createSection();

  using Format = ipc_gen::DMenuOutputFormat;

  auto selectEntryLabel = m_outputFormat == Format::Data ? tr("Select entry") : tr("Select entry (index)");

  main->addAction(new StaticAction(selectEntryLabel, ImageURL::builtin(BuiltinIcon::SaveDocument),
                                   [this, text, idx](ApplicationContext *) {
                                     switch (m_outputFormat) {
                                     case Format::Data:
                                       selectEntry(text);
                                       break;
                                     case Format::Index:
                                       selectEntry(QString::number(idx));
                                       break;
                                     }
                                   }));

  main->addAction(new StaticAction(tr("Pass search text"), ImageURL::builtin(BuiltinIcon::SaveDocument),
                                   [this](ApplicationContext *) { selectEntry(m_currentSearchText); }));

  auto *selectAndCopy =
      new StaticAction(tr("Select and copy entry"), ImageURL::builtin(BuiltinIcon::CopyClipboard),
                       [this, text](ApplicationContext *ctx) {
                         ctx->services->clipman()->copyText(text);
                         selectEntry(text);
                       });
  selectAndCopy->setShortcut(Keybind::CopyAction);
  main->addAction(selectAndCopy);

  return panel;
}

void DMenuSection::onSelected(int i) {
  auto entry = entryAt(i).first;
  if (!entry.empty()) {
    std::error_code ec;
    if (entry.starts_with('/') && std::filesystem::exists(entry, ec)) {
      if (m_onFileHighlighted) m_onFileHighlighted(entry);
    }
  }
}
