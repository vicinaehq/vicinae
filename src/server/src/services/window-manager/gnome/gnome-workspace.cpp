#include "gnome-workspace.hpp"

using namespace Gnome;

QString Workspace::id() const {
  return QString::number(m_id);
}

QString Workspace::name() const {
  return m_name;
}

QString Workspace::monitor() const {
  return m_monitorId;
}

bool Workspace::hasFullScreen() const {
  return m_hasFullScreen;
}

Workspace::Workspace(const QJsonObject &json) {
  m_id = json.value("index").toInt();
  m_name = json.value("name").toString();
  if (m_name.isEmpty()) {
    m_name = QString("Workspace %1").arg(m_id + 1);
  }
  m_monitorId = QString::number(json.value("monitor").toInt());
  m_hasFullScreen = json.value("hasfullscreen").toBool();
}
