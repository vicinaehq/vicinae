#pragma once

#include <QObject>
#include "services/screenshots/abstract-screenshot-provider.hpp"

class ScreenshotService : public QObject {
  Q_OBJECT

signals:
  void refreshed();
  void loadingChanged();

public:
  explicit ScreenshotService(QObject *parent = nullptr);
  bool isAvailable() const { return m_provider != nullptr; }
  bool isLoading() const { return m_loading; }
  const std::vector<Screenshot> &items() const { return m_result.items; }
  const Screenshot *latestImage() const { return m_result.latestImage(); }
  const std::optional<QString> &error() const { return m_result.error; }
  void refresh();

private:
  AbstractScreenshotProvider *m_provider = nullptr;
  ScreenshotResult m_result;
  bool m_loading = false;
};
