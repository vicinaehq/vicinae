#pragma once
#include "services/media-control/abstract-media-control.hpp"

class DummyMediaControl : public AbstractMediaControl {
  Q_OBJECT

public:
  QString id() const override { return "dummy"; }

  std::vector<MediaPlayer> players() const override { return {}; }

  bool playPause(const QString &) override { return false; }
  bool next(const QString &) override { return false; }
  bool previous(const QString &) override { return false; }
};
