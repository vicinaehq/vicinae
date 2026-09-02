#pragma once
#include "services/tray-host/abstract-tray-host.hpp"

class DummyTrayHost : public AbstractTrayHost {
public:
  QString id() const override { return "dummy"; }
  bool isAvailable() const override { return false; }
  std::vector<TrayItem> items() const override { return {}; }
  void activate(const TrayItem &) override {}
  void secondaryActivate(const TrayItem &) override {}
  QFuture<std::vector<TrayMenuItem>> menu(const TrayItem &) override {
    return QtFuture::makeReadyValueFuture(std::vector<TrayMenuItem>{});
  }
  void triggerMenuItem(const TrayItem &, int) override {}
};
