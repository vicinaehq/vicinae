#pragma once
#include "services/status-notifier/abstract-tray-service.hpp"

class DummyTrayService : public AbstractTrayService {
public:
  QString id() const override { return "dummy"; }
  bool isAvailable() const override { return false; }
  std::vector<TrayItem> items() const override { return {}; }
  void activate(const TrayItem &, int, int) override {}
  void secondaryActivate(const TrayItem &, int, int) override {}
  void contextMenu(const TrayItem &, int, int) override {}
  void scroll(const TrayItem &, int, bool) override {}
  QFuture<std::vector<TrayMenuItem>> menu(const TrayItem &) override {
    return QtFuture::makeReadyValueFuture(std::vector<TrayMenuItem>{});
  }
  QFuture<std::vector<TrayEntry>> snapshot() override {
    return QtFuture::makeReadyValueFuture(std::vector<TrayEntry>{});
  }
  void triggerMenuItem(const TrayItem &, int) override {}
};
