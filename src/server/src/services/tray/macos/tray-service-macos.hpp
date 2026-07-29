#pragma once
#include "services/tray/tray-service.hpp"
#include <memory>

class TrayServiceMacOS : public TrayService {
  Q_OBJECT

public:
  explicit TrayServiceMacOS(QObject *parent = nullptr);
  ~TrayServiceMacOS() override;

  void setVersion(const QString &version) override;
  void setCheckForUpdatesVisible(bool visible) override;
  void setAvailableUpdate(const QString &tag) override;
  void show() override;
  void hide() override;

  void emitToggle();
  void emitOpenSettings(const QString &tab);
  void emitCheckForUpdates();
  void emitQuit();

private:
  struct Impl;
  std::unique_ptr<Impl> m_impl;
};
