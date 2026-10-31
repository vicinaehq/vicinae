#pragma once
#include "builtins/vicinae/builtin-icons-model.hpp"
#include "ui/views/list-view-host.hpp"

class BuiltinIconsViewHost : public ListViewHost {
  Q_OBJECT

public:
  void initialize() override;
  void loadInitialData() override;

private:
  BuiltinIconsSection m_section;
};
