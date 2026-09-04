#include "windows-media-control.hpp"
#include <algorithm>
#include <mutex>
#include <thread>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <QDebug>
#include <QFileInfo>

using namespace winrt::Windows::Media::Control;
using Session = GlobalSystemMediaTransportControlsSession;
using SessionManager = GlobalSystemMediaTransportControlsSessionManager;

namespace {

PlaybackStatus toStatus(GlobalSystemMediaTransportControlsSessionPlaybackStatus status) {
  switch (status) {
  case GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing:
    return PlaybackStatus::Playing;
  case GlobalSystemMediaTransportControlsSessionPlaybackStatus::Paused:
    return PlaybackStatus::Paused;
  default:
    return PlaybackStatus::Stopped;
  }
}

QString identityFor(const QString &aumid) {
  if (auto bang = aumid.lastIndexOf('!'); bang != -1) return aumid.mid(bang + 1);
  if (aumid.endsWith(".exe", Qt::CaseInsensitive)) return QFileInfo(aumid).completeBaseName();
  return aumid;
}

QString fromHString(const winrt::hstring &s) { return QString::fromWCharArray(s.c_str(), s.size()); }

} // namespace

struct WindowsMediaControl::Impl {
  struct Entry {
    Session session;
    Session::PlaybackInfoChanged_revoker playbackRevoker;
    Session::MediaPropertiesChanged_revoker propertiesRevoker;
  };

  explicit Impl(WindowsMediaControl *owner) : owner(owner) {}

  void start() {
    initThread = std::thread([this] {
      winrt::init_apartment(winrt::apartment_type::multi_threaded);
      try {
        manager = SessionManager::RequestAsync().get();
      } catch (const winrt::hresult_error &e) {
        qWarning() << "media session manager unavailable, media control will not work:"
                   << fromHString(e.message());
        return;
      }
      sessionsRevoker = manager.SessionsChanged(winrt::auto_revoke, [this](auto &&, auto &&) { refresh(); });
      refresh();
    });
  }

  void stop() {
    if (initThread.joinable()) initThread.join();
    sessionsRevoker.revoke();
    std::scoped_lock lock(refreshMutex, cacheMutex);
    entries.clear();
  }

  void refresh() {
    std::lock_guard refreshLock(refreshMutex);
    std::vector<MediaPlayer> players;
    std::vector<Entry> fresh;

    for (const auto &session : manager.GetSessions()) {
      MediaPlayer player{.id = fromHString(session.SourceAppUserModelId())};
      player.identity = identityFor(player.id);
      player.appId = player.id;

      const auto info = session.GetPlaybackInfo();
      player.status = toStatus(info.PlaybackStatus());
      player.canGoNext = info.Controls().IsNextEnabled();
      player.canGoPrevious = info.Controls().IsPreviousEnabled();

      try {
        const auto props = session.TryGetMediaPropertiesAsync().get();
        player.title = fromHString(props.Title());
        player.artist = fromHString(props.Artist());
      } catch (const winrt::hresult_error &) {}

      auto onChange = [this](auto &&, auto &&) { refresh(); };
      fresh.push_back({.session = session,
                       .playbackRevoker = session.PlaybackInfoChanged(winrt::auto_revoke, onChange),
                       .propertiesRevoker = session.MediaPropertiesChanged(winrt::auto_revoke, onChange)});
      players.emplace_back(std::move(player));
    }

    {
      std::lock_guard cacheLock(cacheMutex);
      cache = std::move(players);
      entries = std::move(fresh);
    }

    QMetaObject::invokeMethod(owner, [owner = owner] { emit owner->playersChanged(); }, Qt::QueuedConnection);
  }

  std::optional<Session> find(const QString &playerId) const {
    std::lock_guard lock(cacheMutex);
    const auto it = std::ranges::find_if(
        entries, [&](const Entry &e) { return fromHString(e.session.SourceAppUserModelId()) == playerId; });
    if (it == entries.end()) return std::nullopt;
    return it->session;
  }

  WindowsMediaControl *owner;
  std::thread initThread;
  SessionManager manager{nullptr};
  SessionManager::SessionsChanged_revoker sessionsRevoker;
  std::mutex refreshMutex;
  mutable std::mutex cacheMutex;
  std::vector<MediaPlayer> cache;
  std::vector<Entry> entries;
};

WindowsMediaControl::WindowsMediaControl() : m_impl(std::make_unique<Impl>(this)) { m_impl->start(); }

WindowsMediaControl::~WindowsMediaControl() { m_impl->stop(); }

QString WindowsMediaControl::id() const { return "smtc"; }

std::vector<MediaPlayer> WindowsMediaControl::players() const {
  std::lock_guard lock(m_impl->cacheMutex);
  return m_impl->cache;
}

bool WindowsMediaControl::playPause(const QString &playerId) {
  auto session = m_impl->find(playerId);
  if (!session) return false;
  session->TryTogglePlayPauseAsync();
  setLastPlayerId(playerId);
  return true;
}

bool WindowsMediaControl::next(const QString &playerId) {
  auto session = m_impl->find(playerId);
  if (!session) return false;
  session->TrySkipNextAsync();
  setLastPlayerId(playerId);
  return true;
}

bool WindowsMediaControl::previous(const QString &playerId) {
  auto session = m_impl->find(playerId);
  if (!session) return false;
  session->TrySkipPreviousAsync();
  setLastPlayerId(playerId);
  return true;
}
