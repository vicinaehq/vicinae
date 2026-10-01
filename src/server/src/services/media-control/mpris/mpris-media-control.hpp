#pragma once
#include <optional>
#include <vector>
#include <QDBusConnection>
#include <QVariantMap>
#include "services/media-control/abstract-media-control.hpp"

/**
 * Media control through MPRIS: https://specifications.freedesktop.org/mpris-spec/latest/
 * Players are discovered by looking for `org.mpris.MediaPlayer2.*` names on the session bus.
 */
class MprisMediaControl : public AbstractMediaControl {
  Q_OBJECT

public:
  MprisMediaControl();

  QString id() const override;

  std::vector<MediaPlayer> players() const override;

  bool playPause(const QString &playerId) override;
  bool next(const QString &playerId) override;
  bool previous(const QString &playerId) override;

private slots:
  void onNameOwnerChanged(const QString &service, const QString &oldOwner, const QString &newOwner);
  void onPropertiesChanged(const QString &interface, const QVariantMap &changed,
                           const QStringList &invalidated);

private:
  std::optional<QVariantMap> getAllProperties(const QString &service, const QString &interface) const;
  std::optional<MediaPlayer> queryPlayer(const QString &service) const;
  bool callPlayer(const QString &playerId, const QString &method);

  QDBusConnection m_bus;
  /// Well known MPRIS bus names, in the order they appeared.
  std::vector<QString> m_services;
};
