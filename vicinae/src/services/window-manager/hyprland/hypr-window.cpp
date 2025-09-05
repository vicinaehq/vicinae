#include "hyprland.hpp"

HyprlandWindow::HyprlandWindow(const QJsonObject &json) {
  m_id = json.value("address").toString();
  m_title = json.value("title").toString();
  m_wmClass = json.value("class").toString();
}
