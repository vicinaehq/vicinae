#include "shortcut-seeder.hpp"
#include "services/app-service/app-service.hpp"
#include "ui/image/url.hpp"
#include <qlogging.h>

struct ShortcutData {
  QString name;
  ImageURL icon;
  QString url;
};

// clang-format off
static const std::vector<ShortcutData> webShortcuts = {
	ShortcutData{
		.name = "Search Google",
		.icon = ImageURL::favicon("google.com"),
		.url = "https://google.com/search?q={query}",
	},
	ShortcutData{
		.name = "Search DuckDuckGo",
		.icon = ImageURL::favicon("duckduckgo.com"),
		.url = "https://duckduckgo.com/?q={query}",
	}
};
// clang-format on

ShortcutSeeder::ShortcutSeeder(ShortcutService &shortcut, const AppService &appDb)
    : m_service(shortcut), m_appDb(appDb) {}

void ShortcutSeeder::seed() {
  auto browser = m_appDb.webBrowser();

  if (!browser) {
    qWarning() << "Could not get default web browser, will not seed.";
    return;
  }

  for (const auto &shortcut : webShortcuts) {
    m_service.createShortcut(shortcut.name, shortcut.icon, shortcut.url, browser->id());
  }
}
