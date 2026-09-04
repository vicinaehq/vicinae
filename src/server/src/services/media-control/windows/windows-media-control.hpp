#pragma once
#include <memory>
#include "services/media-control/abstract-media-control.hpp"

/**
 * Media control through the system media transport controls, the same sessions the Windows
 * media flyout shows. Players are identified by their AppUserModelId.
 */
class WindowsMediaControl : public AbstractMediaControl {
  Q_OBJECT

public:
  WindowsMediaControl();
  ~WindowsMediaControl() override;

  QString id() const override;

  std::vector<MediaPlayer> players() const override;

  bool playPause(const QString &playerId) override;
  bool next(const QString &playerId) override;
  bool previous(const QString &playerId) override;

private:
  struct Impl;
  std::unique_ptr<Impl> m_impl;
};
