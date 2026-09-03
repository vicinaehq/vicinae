#include "bar/bar-host.hpp"
#include <QDateTime>
#include <QGuiApplication>
#include <QLocale>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <ranges>
#include "builtin_icon.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "services/audio-control/audio-control-service.hpp"
#include "services/media-control/media-control-service.hpp"
#include "services/news/news-service.hpp"
#include "services/power-manager/power-manager.hpp"
#include "services/window-manager/window-manager.hpp"
#include "settings-controller/settings-controller.hpp"

namespace {

using namespace std::chrono_literals;

constexpr auto COALESCE_DELAY = 40ms;
constexpr auto AUDIO_POLL_INTERVAL = 10s;
constexpr std::size_t MAX_WORKSPACE_ICONS = 4;

} // namespace

BarHost::BarHost(ApplicationContext &ctx, QObject *parent)
    : QObject(parent), m_ctx(ctx), m_tray(ctx.services->trayHost(), this) {
  m_clockTimer.setSingleShot(true);
  connect(&m_clockTimer, &QTimer::timeout, this, &BarHost::tickClock);
  tickClock();

  auto *wm = ctx.services->windowManager();

  m_workspaceTimer.setSingleShot(true);
  m_workspaceTimer.setInterval(COALESCE_DELAY);
  connect(&m_workspaceTimer, &QTimer::timeout, this, &BarHost::refreshWorkspaces);
  connect(wm, &WindowManager::windowsChanged, &m_workspaceTimer, qOverload<>(&QTimer::start));

  m_focusTimer.setSingleShot(true);
  m_focusTimer.setInterval(COALESCE_DELAY);
  connect(&m_focusTimer, &QTimer::timeout, this, &BarHost::refreshFocusedWindow);
  connect(wm, &WindowManager::focusChanged, &m_focusTimer, qOverload<>(&QTimer::start));
  connect(wm, &WindowManager::windowsChanged, &m_focusTimer, qOverload<>(&QTimer::start));

  refreshWorkspaces();
  refreshFocusedWindow();

  auto *media = ctx.services->mediaControl()->provider();
  connect(media, &AbstractMediaControl::playersChanged, this, &BarHost::refreshMedia);
  refreshMedia();

  m_audioTimer.setInterval(AUDIO_POLL_INTERVAL);
  connect(&m_audioTimer, &QTimer::timeout, this, &BarHost::refreshAudio);
  m_audioTimer.start();
  refreshAudio();

  connect(ctx.services->newsService(), &NewsService::itemsChanged, this, &BarHost::newsChanged);

  buildPowerActions();
}

void BarHost::tickClock() {
  const auto now = QDateTime::currentDateTime();
  const auto locale = QLocale::system();

  m_time = locale.toString(now.time(), QLocale::ShortFormat);
  m_date = locale.toString(now.date(), QStringLiteral("ddd d MMM"));
  m_longDate = locale.toString(now.date(), QLocale::LongFormat);
  emit clockChanged();

  const auto msIntoMinute = now.time().second() * 1000 + now.time().msec();
  m_clockTimer.start(60'000 - msIntoMinute + 50);
}

void BarHost::refreshFocusedWindow() {
  auto *wm = m_ctx.services->windowManager();
  auto window = wm->getFocusedWindow();

  FocusedWindow next;
  if (window) {
    auto app = m_ctx.services->appDb()->find(window->wmClass());
    next.valid = true;
    next.title = window->title();
    next.app = app ? app->displayName() : window->wmClass();
    next.icon = ImageUrl(app ? app->iconUrl() : ImageURL::builtin(BuiltinIcon::AppWindow));
  }

  if (next.valid == m_focused.valid && next.title == m_focused.title && next.app == m_focused.app &&
      next.icon == m_focused.icon) {
    return;
  }

  m_focused = std::move(next);
  emit focusedWindowChanged();
}

void BarHost::refreshWorkspaces() {
  auto *wm = m_ctx.services->windowManager();
  auto *provider = wm->provider();
  auto *appDb = m_ctx.services->appDb();

  std::vector<BarWorkspace> next;

  if (provider->hasWorkspaces()) {
    auto active = provider->getActiveWorkspace();
    const auto &windows = wm->listWindows();
    auto workspaces = provider->listWorkspaces();
    next.reserve(workspaces.size());

    for (auto &workspace : workspaces) {
      BarWorkspace entry{.workspace = workspace};
      entry.active = active && active->id() == workspace->id();

      std::vector<QString> seen;
      for (const auto &win : windows) {
        if (win->workspace() != workspace->id()) continue;
        entry.windowCount++;
        if (entry.icons.size() >= MAX_WORKSPACE_ICONS || std::ranges::contains(seen, win->wmClass()))
          continue;
        seen.emplace_back(win->wmClass());
        auto app = appDb->find(win->wmClass());
        entry.icons.emplace_back(app ? app->iconUrl() : ImageURL::builtin(BuiltinIcon::AppWindow));
      }

      next.emplace_back(std::move(entry));
    }

    std::ranges::stable_sort(next, [](const BarWorkspace &a, const BarWorkspace &b) {
      bool aOk = false;
      bool bOk = false;
      const auto aId = a.workspace->id().toLongLong(&aOk);
      const auto bId = b.workspace->id().toLongLong(&bOk);
      if (aOk && bOk) return aId < bId;
      if (aOk != bOk) return aOk;
      return a.workspace->name().compare(b.workspace->name(), Qt::CaseInsensitive) < 0;
    });
  }

  m_workspaces = std::move(next);
  emit workspacesChanged();
}

void BarHost::refreshMedia() {
  auto *media = m_ctx.services->mediaControl()->provider();
  auto players = media->players();

  std::optional<MediaPlayer> next;
  auto playing = std::ranges::find_if(players, [](auto &&p) { return p.status == PlaybackStatus::Playing; });

  if (playing != players.end()) {
    next = *playing;
  } else if (auto fallback = media->defaultPlayer()) {
    next = *fallback;
  } else if (!players.empty()) {
    next = players.front();
  }

  if (next && next->status == PlaybackStatus::Stopped && next->title.isEmpty()) next.reset();

  m_media = std::move(next);
  emit mediaChanged();
}

bool BarHost::mediaPlaying() const { return m_media && m_media->status == PlaybackStatus::Playing; }

void BarHost::refreshAudio() {
  auto *audio = m_ctx.services->audioControl()->provider();
  const int volume = static_cast<int>(std::lround(audio->getVolume() * 100.0F));
  const bool muted = audio->isMuted();

  if (volume == m_volume && muted == m_muted) return;
  m_volume = volume;
  m_muted = muted;
  emit audioChanged();
}

void BarHost::adjustVolume(int delta) {
  auto *audio = m_ctx.services->audioControl()->provider();
  audio->adjustVolume(static_cast<float>(delta) / 100.0F);
  refreshAudio();
}

void BarHost::toggleMute() {
  m_ctx.services->audioControl()->provider()->toggleMute();
  refreshAudio();
}

void BarHost::mediaPlayPause() {
  if (m_media) m_ctx.services->mediaControl()->provider()->playPause(m_media->id);
}

void BarHost::mediaNext() {
  if (m_media) m_ctx.services->mediaControl()->provider()->next(m_media->id);
}

void BarHost::mediaPrevious() {
  if (m_media) m_ctx.services->mediaControl()->provider()->previous(m_media->id);
}

void BarHost::buildPowerActions() {
  auto *power = m_ctx.services->powerManager()->provider();
  auto add = [&](const QString &id, const QString &label, const QString &icon) {
    m_powerActions.push_back(QVariantMap{{"id", id}, {"label", label}, {"icon", icon}});
  };

  if (power->canLock()) add("lock", tr("Lock Screen"), "lock");
  if (power->canSuspend()) add("suspend", tr("Sleep"), "moon");
  if (power->canHibernate()) add("hibernate", tr("Hibernate"), "moon");
  if (power->canReboot()) add("reboot", tr("Restart"), "arrow-clockwise");
  if (power->canPowerOff()) add("power-off", tr("Shut Down"), "power");
  if (power->canLogOut()) add("logout", tr("Log Out"), "logout");
}

void BarHost::powerAction(const QString &id) {
  auto *power = m_ctx.services->powerManager()->provider();

  if (id == "lock") {
    power->lock();
  } else if (id == "suspend") {
    power->suspend();
  } else if (id == "hibernate") {
    power->hibernate();
  } else if (id == "reboot") {
    power->reboot();
  } else if (id == "power-off") {
    power->powerOff();
  } else if (id == "logout") {
    power->logout();
  }
}

bool BarHost::hasUnreadNews() const { return m_ctx.services->newsService()->hasUnreadNews(); }

void BarHost::toggleLauncher() { m_ctx.navigation->toggleWindow(); }

void BarHost::openSettings() { m_ctx.settings->openWindow(); }

void BarHost::focusWorkspace(const QString &id) {
  auto it = std::ranges::find_if(m_workspaces, [&](auto &&ws) { return ws.workspace->id() == id; });
  if (it == m_workspaces.end()) return;
  m_ctx.services->windowManager()->provider()->focusWorkspaceSync(*it->workspace);
}
