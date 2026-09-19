#include "macos-window.hpp"

MacosWindow::MacosWindow(AXUIElementRef element, QString id, QString title, QString wmClass, int pid,
                         bool canClose, bool canFullScreen)
    : m_element(element), m_id(std::move(id)), m_title(std::move(title)), m_wmClass(std::move(wmClass)),
      m_pid(pid), m_canClose(canClose), m_canFullScreen(canFullScreen) {
  if (m_element) CFRetain(m_element);
}

MacosWindow::~MacosWindow() {
  if (m_element) CFRelease(m_element);
}

std::optional<AbstractWindowManager::WindowBounds> MacosWindow::bounds() const {
  CFTypeRef positionValue = nullptr;
  CFTypeRef sizeValue = nullptr;
  CGPoint position{};
  CGSize size{};

  const bool hasPosition =
      AXUIElementCopyAttributeValue(m_element, kAXPositionAttribute, &positionValue) == kAXErrorSuccess &&
      positionValue && CFGetTypeID(positionValue) == AXValueGetTypeID() &&
      AXValueGetValue(static_cast<AXValueRef>(positionValue), kAXValueTypeCGPoint, &position);
  const bool hasSize =
      AXUIElementCopyAttributeValue(m_element, kAXSizeAttribute, &sizeValue) == kAXErrorSuccess &&
      sizeValue && CFGetTypeID(sizeValue) == AXValueGetTypeID() &&
      AXValueGetValue(static_cast<AXValueRef>(sizeValue), kAXValueTypeCGSize, &size);

  if (positionValue) CFRelease(positionValue);
  if (sizeValue) CFRelease(sizeValue);
  if (!hasPosition || !hasSize) return std::nullopt;

  return AbstractWindowManager::WindowBounds{.x = static_cast<std::int32_t>(position.x),
                                             .y = static_cast<std::int32_t>(position.y),
                                             .width = static_cast<std::int32_t>(size.width),
                                             .height = static_cast<std::int32_t>(size.height)};
}

bool MacosWindow::fullScreen() const {
  CFTypeRef value = nullptr;
  if (AXUIElementCopyAttributeValue(m_element, CFSTR("AXFullScreen"), &value) != kAXErrorSuccess || !value) {
    return false;
  }
  const bool fullScreen =
      CFGetTypeID(value) == CFBooleanGetTypeID() && CFBooleanGetValue(static_cast<CFBooleanRef>(value));
  CFRelease(value);
  return fullScreen;
}
