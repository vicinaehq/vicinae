#include "hyprland.hpp"

HyprlandWindow::HyprlandWindow(const QJsonObject &json) {
  m_id = json.value("address").toString();
  m_title = json.value("title").toString();
  m_wmClass = json.value("class").toString();
  m_workspaceId = json.value("workspace").toObject().value("id").toInt();

  auto at = json.value("at").toArray();
  auto size = json.value("size").toArray();

  m_bounds.x = at.at(0).toInt();
  m_bounds.y = at.at(1).toInt();
  m_bounds.width = size.at(0).toInt();
  m_bounds.height = size.at(1).toInt();
}
