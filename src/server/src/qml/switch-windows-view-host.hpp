#pragma once
#include "list-view-host.hpp"
#include "switch-windows-model.hpp"
#include <chrono>

class SwitchWindowsViewHost : public ListViewHost {
  Q_OBJECT

public:
  void initialize() override;
  void loadInitialData() override;
  void textChanged(const QString &text) override;

private:
  void refreshWindows();

  SwitchWindowsSection m_section;
  std::chrono::time_point<std::chrono::steady_clock> m_lastFetch{};
};
