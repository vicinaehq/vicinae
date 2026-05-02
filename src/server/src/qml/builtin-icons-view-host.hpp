#pragma once
#include "builtin-icons-model.hpp"
#include "list-view-host.hpp"

class BuiltinIconsViewHost : public ListViewHost {
  Q_OBJECT

public:
  void initialize() override;
  void loadInitialData() override;

private:
  BuiltinIconsSection m_section;
};
