#include "manage-fallback-model.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action.hpp"

// --- EnabledFallbackSection ---

void EnabledFallbackSection::sortFiltered() {
  auto fallbackPos = [&](const auto &scored) {
    return std::distance(m_fallbacks.begin(), std::ranges::find(m_fallbacks, m_items[scored.data]));
  };

  std::ranges::stable_sort(m_filtered,
                           [&](const auto &a, const auto &b) { return fallbackPos(a) < fallbackPos(b); });
}

QString EnabledFallbackSection::displayTitle(const RootItemPtr &item) const { return item->title(); }

QString EnabledFallbackSection::displaySubtitle(const RootItemPtr &item) const { return item->subtitle(); }

QString EnabledFallbackSection::displayIconSource(const RootItemPtr &item) const {
  return imageSourceFor(item->iconUrl());
}

std::unique_ptr<ActionPanelState> EnabledFallbackSection::buildActionPanel(const RootItemPtr &item) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto *section = panel->createSection();

  section->addAction(new StaticAction("Disable fallback", ImageURL::builtin("redo"),
                                      [id = item->uniqueId()](ApplicationContext *ctx) {
                                        ctx->services->rootItemManager()->disableFallback(id);
                                      }));

  return panel;
}

// --- AvailableFallbackSection ---

QString AvailableFallbackSection::displayTitle(const RootItemPtr &item) const { return item->title(); }

QString AvailableFallbackSection::displaySubtitle(const RootItemPtr &item) const { return item->subtitle(); }

QString AvailableFallbackSection::displayIconSource(const RootItemPtr &item) const {
  return imageSourceFor(item->iconUrl());
}

std::unique_ptr<ActionPanelState> AvailableFallbackSection::buildActionPanel(const RootItemPtr &item) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto *section = panel->createSection();

  section->addAction(new StaticAction("Enable fallback", ImageURL::builtin("redo"),
                                      [id = item->uniqueId()](ApplicationContext *ctx) {
                                        ctx->services->rootItemManager()->enableFallback(id);
                                      }));

  return panel;
}
