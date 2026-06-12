#include "macos-window.hpp"

MacosWindow::MacosWindow(AXUIElementRef element, QString id, QString title, QString wmClass, int pid,
                         std::optional<AbstractWindowManager::WindowBounds> bounds, bool canClose)
    : m_element(element), m_id(std::move(id)), m_title(std::move(title)), m_wmClass(std::move(wmClass)),
      m_pid(pid), m_bounds(std::move(bounds)), m_canClose(canClose) {
  if (m_element) CFRetain(m_element);
}

MacosWindow::~MacosWindow() {
  if (m_element) CFRelease(m_element);
}
