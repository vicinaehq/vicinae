#pragma once
#include <memory>
#include <unordered_set>
#include "services/media-control/abstract-media-control.hpp"
#ifdef Q_OS_LINUX
#include "services/media-control/mpris/mpris-media-control.hpp"
#elif defined(Q_OS_WIN)
#include "services/media-control/windows/windows-media-control.hpp"
#else
#include "services/media-control/dummy-media-control.hpp"
#endif

class MediaControlService {
public:
  struct TransientPauseHandle {
    explicit TransientPauseHandle(MediaControlService &service) : m_service(service) {
      for (const auto &player : service.m_backend->players()) {
        if (player.status == PlaybackStatus::Playing) {
          service.m_backend->playPause(player.id);
          paused.insert(player.id);
        }
      }
    }

    ~TransientPauseHandle() { resume(); }

    // resume players immediately, do not wait for destructor to run
    void resume() {
      if (paused.empty()) return;
      for (const auto &player : m_service.m_backend->players()) {
        if (paused.contains(player.id) && player.status != PlaybackStatus::Playing) {
          m_service.m_backend->playPause(player.id);
        }
      }
      paused.clear();
    }

  private:
    std::unordered_set<QString> paused;
    MediaControlService &m_service;
  };

  AbstractMediaControl *provider() const { return m_backend.get(); }

  MediaControlService() {
#ifdef Q_OS_LINUX
    m_backend = std::make_unique<MprisMediaControl>();
#elifdef Q_OS_WIN
    m_backend = std::make_unique<WindowsMediaControl>();
#else
    m_backend = std::make_unique<DummyMediaControl>();
#endif
  }

  bool available() const { return m_backend->id() != "dummy"; }

  /// Pause all currently playing sources, and resume them when the handle goes out of scope.
  TransientPauseHandle transientPauseAll() { return TransientPauseHandle{*this}; }

private:
  std::unique_ptr<AbstractMediaControl> m_backend;
};
