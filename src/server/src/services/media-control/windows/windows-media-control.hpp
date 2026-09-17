#pragma once
#include <memory>
#include "services/media-control/abstract-media-control.hpp"

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
