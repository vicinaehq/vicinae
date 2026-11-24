#pragma once

#include <QtWaylandClient/QWaylandClientExtension>
#include "internal/wayland/shm-buffer.hpp"
#include "qwayland-wlr-screencopy-unstable-v1.h"
#include "services/screen-capture/abstract-screen-capture.hpp"

class WlrScreencopyManager : public QWaylandClientExtensionTemplate<WlrScreencopyManager>,
                             public QtWayland::zwlr_screencopy_manager_v1 {
  Q_OBJECT

public:
  WlrScreencopyManager();
  ~WlrScreencopyManager() override;
};

class WlrScreencopyCapture : public AbstractScreenCapture {
public:
  using AbstractScreenCapture::AbstractScreenCapture;

  bool isSupported() const override;
  QFuture<Result> capture(const QScreen &screen) override;

private:
  Wayland::Shm m_shm;
  WlrScreencopyManager m_manager;
};
