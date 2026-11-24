#pragma once

#include <QtWaylandClient/QWaylandClientExtension>
#include "internal/wayland/shm-buffer.hpp"
#include "qwayland-ext-image-capture-source-v1.h"
#include "qwayland-ext-image-copy-capture-v1.h"
#include "services/screen-capture/abstract-screen-capture.hpp"

class ExtOutputImageCaptureSourceManager
    : public QWaylandClientExtensionTemplate<ExtOutputImageCaptureSourceManager>,
      public QtWayland::ext_output_image_capture_source_manager_v1 {
  Q_OBJECT

public:
  ExtOutputImageCaptureSourceManager();
  ~ExtOutputImageCaptureSourceManager() override;
};

class ExtImageCopyCaptureManager : public QWaylandClientExtensionTemplate<ExtImageCopyCaptureManager>,
                                   public QtWayland::ext_image_copy_capture_manager_v1 {
  Q_OBJECT

public:
  ExtImageCopyCaptureManager();
  ~ExtImageCopyCaptureManager() override;
};

class ExtImageCopyCapture : public AbstractScreenCapture {
public:
  using AbstractScreenCapture::AbstractScreenCapture;

  bool isSupported() const override;
  QFuture<Result> capture(const QScreen &screen) override;

private:
  Wayland::Shm m_shm;
  ExtOutputImageCaptureSourceManager m_sources;
  ExtImageCopyCaptureManager m_manager;
};
