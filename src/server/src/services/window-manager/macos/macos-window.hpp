#pragma once
#include <ApplicationServices/ApplicationServices.h>
#include <QString>
#include <optional>
#include "services/window-manager/abstract-window-manager.hpp"

/**
 * A window owned by a regular macOS application, backed by an Accessibility (AX) element.
 *
 * The window keeps a retained reference to its AXUIElement so that focus and close operations can be
 * performed on the exact window even after the list was fetched. All other properties are snapshotted at
 * construction time since the window list is refetched frequently.
 */
class MacosWindow : public AbstractWindowManager::AbstractWindow {
public:
  MacosWindow(AXUIElementRef element, QString id, QString title, QString wmClass, int pid,
              std::optional<AbstractWindowManager::WindowBounds> bounds, bool canClose, bool fullScreen,
              bool canFullScreen);
  ~MacosWindow() override;

  MacosWindow(const MacosWindow &) = delete;
  MacosWindow &operator=(const MacosWindow &) = delete;

  QString id() const override { return m_id; }
  QString title() const override { return m_title; }
  QString wmClass() const override { return m_wmClass; }
  std::optional<int> pid() const override { return m_pid; }
  std::optional<AbstractWindowManager::WindowBounds> bounds() const override { return m_bounds; }
  bool canClose() const override { return m_canClose; }
  bool fullScreen() const override { return m_fullScreen; }
  bool canFullScreen() const override { return m_canFullScreen; }

  AXUIElementRef element() const { return m_element; }

private:
  AXUIElementRef m_element;
  QString m_id;
  QString m_title;
  QString m_wmClass;
  int m_pid;
  std::optional<AbstractWindowManager::WindowBounds> m_bounds;
  bool m_canClose;
  bool m_fullScreen;
  bool m_canFullScreen;
};
