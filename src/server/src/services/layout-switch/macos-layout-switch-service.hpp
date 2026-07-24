#pragma once
#include "services/layout-switch/abstract-layout-switch-service.hpp"

class MacosLayoutSwitchService : public AbstractLayoutSwitchService {
  Q_OBJECT

public:
  using AbstractLayoutSwitchService::AbstractLayoutSwitchService;

  bool isSupported() const override { return true; }
  std::vector<InputLayout> availableLayouts() const override;
  void activate(const QString &layoutId) override;
  void restore() override;

private:
  QString m_savedLayoutId;   // active before we switched
  QString m_appliedLayoutId; // the one we switched to
};
