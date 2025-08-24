#include "gnome-window.hpp"
#include <QJsonValue>
#include <QLoggingCategory>

GnomeWindow::GnomeWindow(const QJsonObject &json) {
  // Required fields from List() response
  m_id = QString::number(json.value("id").toInteger());
  m_title = json.value("title").toString();
  m_wmClass = json.value("wm_class").toString();
  m_wmClassInstance = json.value("wm_class_instance").toString();

  // Optional fields
  auto pidValue = json.value("pid");
  if (!pidValue.isUndefined() && !pidValue.isNull()) { m_pid = pidValue.toInt(); }

  m_focused = json.value("focus").toBool(false);
  m_inCurrentWorkspace = json.value("in_current_workspace").toBool(false);

  // Workspace information
  auto workspaceValue = json.value("workspace");
  if (!workspaceValue.isUndefined() && !workspaceValue.isNull() && workspaceValue.toInt() >= 0) {
    m_workspace = workspaceValue.toInt();
  }

  // Log the window creation for debugging
  qDebug() << "Created GnomeWindow:" << m_id << m_title << m_wmClass;
}

void GnomeWindow::updateWithDetails(const QJsonObject &detailsJson) {
  // Update basic info in case it changed
  m_title = detailsJson.value("title").toString(m_title);
  m_focused = detailsJson.value("focus").toBool(m_focused);
  m_inCurrentWorkspace = detailsJson.value("in_current_workspace").toBool(m_inCurrentWorkspace);

  // Update workspace information
  auto workspaceValue = detailsJson.value("workspace");
  if (!workspaceValue.isUndefined() && !workspaceValue.isNull() && workspaceValue.toInt() >= 0) {
    m_workspace = workspaceValue.toInt();
  }

  // Window capabilities - only parse canClose since it's the only one we use
  auto canCloseValue = detailsJson.value("canclose");
  if (!canCloseValue.isUndefined()) { m_canClose = canCloseValue.toBool(); }

  qDebug() << "Updated GnomeWindow details for:" << m_id;
}

uint32_t GnomeWindow::numericId() const {
  bool ok;
  uint32_t numId = m_id.toUInt(&ok);
  if (!ok) {
    qWarning() << "Failed to convert window ID to numeric:" << m_id;
    return 0;
  }
  return numId;
}
