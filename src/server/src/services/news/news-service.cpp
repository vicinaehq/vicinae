#include "news-service.hpp"
#include "actions/app/app-actions.hpp"
#include "builtin_icon.hpp"
#include "config/config.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "theme/colors.hpp"
#include "vicinae.hpp"
#include <algorithm>
#include <fstream>
#include <glaze/glaze.hpp>
#include <ranges>

struct NewsState {
  std::vector<std::string> dismissed;
};

namespace {
class AutoDismissProxy : public ProxyAction {
public:
  AutoDismissProxy(AbstractAction *action, std::string newsId)
      : ProxyAction(action), m_newsId(std::move(newsId)) {}

  void executeAfter(ApplicationContext *ctx) override { ctx->services->newsService()->dismiss(m_newsId); }

private:
  std::string m_newsId;
};
} // namespace

DismissNewsAction::DismissNewsAction(std::string newsId)
    : AbstractAction(QStringLiteral("Dismiss"), BuiltinIcon::Xmark), m_newsId(std::move(newsId)) {}

void DismissNewsAction::execute(ApplicationContext *ctx) { ctx->services->newsService()->dismiss(m_newsId); }

NewsService::NewsService(config::Manager &config)
    : m_config(config), m_stateFile(Omnicast::stateDir() / "news.json") {
  m_items = allItems();
  loadState();
}

void NewsService::dismiss(const std::string &id) {
  if (isDismissed(id)) return;
  m_dismissed.emplace_back(id);
  saveState();
  emit itemsChanged();
}

bool NewsService::isDismissed(const std::string &id) const {
  return std::ranges::find(m_dismissed, id) != m_dismissed.end();
}

std::vector<const NewsItem *> NewsService::activeItems() const {
  const auto &cfg = m_config.value();
  std::vector<const NewsItem *> result;
  for (const auto &item : m_items) {
    if (isDismissed(item.id)) continue;
    if (item.id == "telemetry-notice-v1" && !cfg.telemetry.systemInfo) continue;
    result.emplace_back(&item);
  }
  return result;
}

void NewsService::loadState() {
  std::string buf;
  NewsState state;

  if (const auto err = glz::read_file_json(state, m_stateFile.c_str(), buf)) {
    qWarning() << "Failed to parse news state:" << glz::format_error(err);
    return;
  }

  m_dismissed = std::move(state.dismissed);
}

void NewsService::saveState() const {
  NewsState state{.dismissed = m_dismissed};
  std::string buf;
  if (const auto err = glz::write_json(state, buf)) {
    qWarning() << "Failed to serialize news state:" << glz::format_error(err);
    return;
  }

  std::ofstream file(m_stateFile);
  if (!file.is_open()) {
    qWarning() << "Failed to open news state file for writing:" << m_stateFile;
    return;
  }
  file << buf;
}

std::vector<NewsItem> NewsService::allItems() {
  std::vector<NewsItem> items;

  items.push_back({
      .id = "telemetry-notice-v1",
      .title = QStringLiteral("Telemetry"),
      .subtitle = QStringLiteral("We now collect basic usage statistics on startup"),
      .icon = ImageURL{BuiltinIcon::Megaphone}.setBackgroundTint(SemanticColor::Yellow),
      .actionFactory =
          [](ApplicationContext *) {
            auto panel = std::make_unique<ActionPanelState>();
            auto *section = panel->createSection();

            auto *openDocs = new OpenInBrowserAction(QUrl(Omnicast::DOC_TELEMETRY_URL), "Learn more");
            auto *proxy = new AutoDismissProxy(openDocs, "telemetry-notice-v1");
            proxy->setPrimary(true);
            section->addAction(proxy);

            return panel;
          },
  });

  return items;
}
