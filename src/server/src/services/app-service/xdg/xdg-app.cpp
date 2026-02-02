#include "xdg-app.hpp"
#include "services/app-service/xdg/xdg-app-action.hpp"

std::vector<std::shared_ptr<AbstractApplication>> XdgApplication::actions() const {
  std::vector<std::shared_ptr<AbstractApplication>> apps;
  apps.reserve(m_entry.actions().size());

  for (const auto &action : m_entry.actions()) {
    apps.emplace_back(std::make_shared<XdgApplicationAction>(m_entry, action));
  }

  return apps;
}
