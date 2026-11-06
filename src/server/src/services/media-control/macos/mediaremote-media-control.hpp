#pragma once
#include <optional>
#include <string>
#include <vector>
#include <QPointer>
#include <QString>
#include <QTimer>
#include "services/media-control/abstract-media-control.hpp"

namespace AI {
class JsonLineProcess;
}

/**
 * Now Playing through Apple's private MediaRemote framework. Since macOS 15.4 only entitled processes may
 * use it, so everything runs inside /usr/bin/perl, which is one, through the mediaremote-adapter shim
 * (https://github.com/ungive/mediaremote-adapter). One `stream` process stays alive to keep the state
 * current, commands go through short-lived ones. There is a single player: whatever owns Now Playing.
 */
class MediaRemoteMediaControl : public AbstractMediaControl {
  Q_OBJECT

public:
  MediaRemoteMediaControl();
  ~MediaRemoteMediaControl() override;

  QString id() const override { return "mediaremote"; }
  bool operational() const override { return m_operational; }

  std::vector<MediaPlayer> players() const override;

  bool playPause(const QString &playerId) override;
  bool next(const QString &playerId) override;
  bool previous(const QString &playerId) override;

private:
  struct Adapter {
    QString script;
    QString framework;
  };

  // MRCommand ids understood by `send`
  static constexpr int PLAY = 0;
  static constexpr int PAUSE = 1;
  static constexpr int NEXT = 4;
  static constexpr int PREVIOUS = 5;
  static constexpr int RESTART_DELAY_MS = 5000;
  static constexpr int MAX_RESTARTS = 5;

  static std::optional<Adapter> locateAdapter();

  void startStream();
  void handleLine(const std::string &line);
  void handleExit(const std::string &reason);
  bool send(const QString &playerId, int command);

  std::optional<Adapter> m_adapter;
  QPointer<AI::JsonLineProcess> m_stream;
  QTimer m_restart;
  int m_restarts = 0;
  bool m_operational = false;
  std::optional<MediaPlayer> m_nowPlaying;
};
