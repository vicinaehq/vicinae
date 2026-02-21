#include "manage-fallback-model.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action.hpp"

void ManageFallbackModel::applyFilter() {
  fuzzy::fuzzyFilter<RootItemPtr>(std::span<const RootItemPtr>(m_items), m_query, m_filtered);

  // Partition into enabled (fallback) and available, keeping fuzzy sort within each group
  std::vector<Scored<int>> enabled, available;
  for (const auto &entry : m_filtered) {
    if (isFallbackEnabled(m_items[entry.data]))
      enabled.push_back(entry);
    else
      available.push_back(entry);
  }

  // Stable sort enabled items by their position in the configured fallback order
  std::ranges::stable_sort(enabled, [&](const auto &a, const auto &b) {
    auto posA = std::distance(m_fallbacks.begin(), std::ranges::find(m_fallbacks, m_items[a.data]));
    auto posB = std::distance(m_fallbacks.begin(), std::ranges::find(m_fallbacks, m_items[b.data]));
    return posA < posB;
  });

  m_filtered.clear();
  m_filtered.insert(m_filtered.end(), enabled.begin(), enabled.end());
  m_filtered.insert(m_filtered.end(), available.begin(), available.end());

  std::vector<SectionInfo> sections;
  if (!enabled.empty())
    sections.push_back({.name = QStringLiteral("Enabled"), .count = static_cast<int>(enabled.size())});
  if (!available.empty())
    sections.push_back({.name = QStringLiteral("Available"), .count = static_cast<int>(available.size())});
  commitSections(sections);
}

bool ManageFallbackModel::isFallbackEnabled(const RootItemPtr &item) const {
  return std::ranges::find(m_fallbacks, item) != m_fallbacks.end();
}

QString ManageFallbackModel::displayTitle(const RootItemPtr &item) const {
  return item->displayName();
}

QString ManageFallbackModel::displaySubtitle(const RootItemPtr &item) const {
  return item->subtitle();
}

QString ManageFallbackModel::displayIconSource(const RootItemPtr &item) const {
  return imageSourceFor(item->iconUrl());
}

std::unique_ptr<ActionPanelState> ManageFallbackModel::buildActionPanel(const RootItemPtr &item) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto *section = panel->createSection();

  if (isFallbackEnabled(item)) {
    section->addAction(new StaticAction("Disable fallback", ImageURL::builtin("redo"),
                                        [id = item->uniqueId()](ApplicationContext *ctx) {
                                          ctx->services->rootItemManager()->disableFallback(id);
                                        }));
  } else {
    section->addAction(new StaticAction("Enable fallback", ImageURL::builtin("redo"),
                                        [id = item->uniqueId()](ApplicationContext *ctx) {
                                          ctx->services->rootItemManager()->enableFallback(id);
                                        }));
  }

  return panel;
}
