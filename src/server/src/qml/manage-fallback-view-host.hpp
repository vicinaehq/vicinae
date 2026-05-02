#pragma once
#include "manage-fallback-model.hpp"
#include "list-view-host.hpp"

class ManageFallbackViewHost : public ListViewHost {
  Q_OBJECT

public:
  void initialize() override;
  void loadInitialData() override;

private:
  void reload();

  EnabledFallbackSection m_enabledSection;
  AvailableFallbackSection m_availableSection;
};
