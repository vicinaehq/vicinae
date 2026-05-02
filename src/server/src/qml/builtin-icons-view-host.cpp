#include "builtin-icons-view-host.hpp"
#include "builtin-icons-model.hpp"

void BuiltinIconsViewHost::initialize() {
  BaseView::initialize();
  initModel();

  model()->addSource(&m_section);

  setSearchPlaceholderText("Search icons...");
}

void BuiltinIconsViewHost::loadInitialData() {
  auto &mapping = BuiltinIconService::mapping();
  std::vector<IconEntry> icons;
  icons.reserve(mapping.size());
  for (const auto &[icon, name] : mapping)
    icons.push_back({icon, name});
  m_section.setItems(std::move(icons));
}
