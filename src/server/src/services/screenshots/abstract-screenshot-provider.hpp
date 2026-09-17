#pragma once

#include <QObject>
#include "services/screenshots/screenshot.hpp"

class AbstractScreenshotProvider : public QObject {
  Q_OBJECT

signals:
  void refreshed(ScreenshotResult result);

public:
  using QObject::QObject;
  virtual void refresh() = 0;
};
