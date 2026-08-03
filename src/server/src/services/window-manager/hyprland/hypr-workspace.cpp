#include "hypr-workspace.hpp"
#include "hypr-ipc.hpp"

using namespace Hyprland;

QString Workspace::id() const { return QString::number(m_id); }

QString Workspace::name() const { return m_name; }

std::optional<QString> Workspace::monitor() const { return QString::number(m_monitorId); }

bool Workspace::hasFullScreen() const { return m_hasFullScreen; };

Workspace::Workspace(const ipc::Workspace &workspace) {
  m_id = workspace.id;
  m_name = QString::fromStdString(workspace.name);
  m_hasFullScreen = workspace.hasFullScreen;
  m_monitorId = workspace.monitorId;
}
