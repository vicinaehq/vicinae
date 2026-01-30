#pragma once
#include "services/window-manager/abstract-window-manager.hpp"
#include <QJsonObject>
#include <QString>
#include <optional>

/**
 * Represents a window in GNOME Shell through D-Bus interface.
 * Stores window information retrieved from the Vicinae GNOME extension.
 */
class GnomeWindow : public AbstractWindowManager::AbstractWindow {
private:
  QString m_id;
  QString m_title;
  QString m_wmClass;
  QString m_wmClassInstance;
  QString m_mappedWmClass; // Complex ID mapped from simple ID
  std::optional<int> m_pid;
  bool m_focused;
  bool m_inCurrentWorkspace;
  std::optional<int> m_workspace;
  std::optional<bool> m_canClose;

public:
  /**
   * Constructor for basic window info (from List() response)
   */
  explicit GnomeWindow(const QJsonObject &json);

  // Required AbstractWindow interface
  QString id() const override { return m_id; }
  QString title() const override { return m_title; }
  QString wmClass() const override { return m_mappedWmClass.isEmpty() ? m_wmClass : m_mappedWmClass; }
  std::optional<int> pid() const override { return m_pid; }

  // Extended AbstractWindow interface
  std::optional<QString> workspace() const override {
    if (!m_workspace) return std::nullopt;
    return QString::number(*m_workspace);
  }
  bool canClose() const override { return m_canClose.value_or(true); }

  // Additional GNOME-specific properties
  QString wmClassInstance() const { return m_wmClassInstance; }
  bool focused() const { return m_focused; }
  bool inCurrentWorkspace() const { return m_inCurrentWorkspace; }
  void setFocused(bool focused) { m_focused = focused; }

  /**
   * Update window with detailed information from Details() response
   */
  void updateWithDetails(const QJsonObject &detailsJson);

  /**
   * Set the mapped complex ID for this window
   */
  void setMappedWmClass(const QString &mappedId) { m_mappedWmClass = mappedId; }

  /**
   * Get the numeric window ID for D-Bus calls
   */
  uint32_t numericId() const;
};
