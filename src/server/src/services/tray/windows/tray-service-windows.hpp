#pragma once

#include <memory>

#include "services/tray/tray-service.hpp"

class TrayServiceWindows : public TrayService {
  Q_OBJECT

public:
  explicit TrayServiceWindows(QObject *parent = nullptr);
  ~TrayServiceWindows() override;

  void setVersion(const QString &version) override;
  void setCheckForUpdatesVisible(bool visible) override;
  void setAvailableUpdate(const QString &tag) override;
  void show() override;
  void hide() override;

private:
  struct Impl;
  std::unique_ptr<Impl> m_impl;
};
