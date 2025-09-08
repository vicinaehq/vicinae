#include "hypr-workspace.hpp"

using namespace Hyprland;

QString Workspace::id() const { return QString::number(m_id); }

QString Workspace::name() const { return m_name; }

QString Workspace::monitor() const { return QString::number(m_monitorId); }

bool Workspace::hasFullScreen() const { return m_hasFullScreen; };

Workspace::Workspace(const QJsonObject &json) {
  m_id = json.value("id").toInt();
  m_name = json.value("name").toString();
  m_hasFullScreen = json.value("hasfullscreen").toBool();
  m_monitorId = json.value("monitorID").toInt();
}
