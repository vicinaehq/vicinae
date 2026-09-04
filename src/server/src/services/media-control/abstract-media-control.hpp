#pragma once
#include <cstdint>
#include <optional>
#include <vector>
#include <QObject>
#include <QString>

enum class PlaybackStatus : std::uint8_t { Stopped, Paused, Playing };

struct MediaPlayer {
  /// Backend specific handle, e.g. the MPRIS bus name of the player.
  QString id;
  /// Name of the program owning the player, e.g. "Spotify".
  QString identity;
  /// Raw key of the owning app for `AppService::find`, e.g. a desktop entry or an AppUserModelId.
  QString appId;
  QString title;
  QString artist;
  PlaybackStatus status = PlaybackStatus::Stopped;
  bool canGoNext = false;
  bool canGoPrevious = false;
};

class AbstractMediaControl : public QObject {
  Q_OBJECT

signals:
  /**
   * A player appeared, went away, or changed what it was doing. Views listing
   * players should request an update.
   */
  void playersChanged() const;

public:
  virtual ~AbstractMediaControl() = default;
  virtual QString id() const = 0;

  virtual std::vector<MediaPlayer> players() const = 0;

  virtual bool playPause(const QString &playerId) = 0;
  virtual bool next(const QString &playerId) = 0;
  virtual bool previous(const QString &playerId) = 0;

  /**
   * Player to act on when the user did not name one: the last player we acted upon if
   * it is still around, otherwise the first playing one.
   */
  std::optional<MediaPlayer> defaultPlayer() const;

protected:
  void setLastPlayerId(const QString &id) { m_lastPlayerId = id; }

private:
  std::optional<QString> m_lastPlayerId;
};
