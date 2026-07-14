#include "windows-chrome-attached.hpp"

#include <QOperatingSystemVersion>
#include <QQuickItem>
#include <QQuickWindow>
#include <qevent.h>

#include <windows.h>
#include <commctrl.h>
#include <dwmapi.h>

namespace {

constexpr int DWM_ROUND_CORNER_RADIUS = 8;
constexpr UINT_PTR CHROME_SUBCLASS_ID = 1;

HWND hwndFromWindow(QWindow *window) { return reinterpret_cast<HWND>(window->winId()); }

COLORREF colorrefFromQColor(const QColor &c) { return RGB(c.red(), c.green(), c.blue()); }

LRESULT CALLBACK chromeSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR,
                                    DWORD_PTR) {
  switch (msg) {
  case WM_NCCALCSIZE:
    if (wParam == TRUE) return 0;
    break;
  case WM_WINDOWPOSCHANGING: {
    auto *wp = reinterpret_cast<WINDOWPOS *>(lParam); // NOLINT
    if (!(wp->flags & SWP_NOSIZE)) wp->flags |= SWP_NOCOPYBITS;
    break;
  }
  case WM_NCACTIVATE:
    return DefWindowProcW(hwnd, msg, wParam, -1);
  case WM_SHOWWINDOW:
    if (wParam == TRUE) DefWindowProcW(hwnd, WM_NCACTIVATE, TRUE, -1);
    break;
  case WM_NCHITTEST: {
    LRESULT hit = DefSubclassProc(hwnd, msg, wParam, lParam);
    return hit >= HTLEFT && hit <= HTBOTTOMRIGHT ? HTCLIENT : hit;
  }
  default:
    break;
  }
  return DefSubclassProc(hwnd, msg, wParam, lParam);
}

} // namespace

WindowsWindowAttached::WindowsWindowAttached(QObject *parent) : QObject(parent) {
  m_window = qobject_cast<QWindow *>(parent);
  if (m_window) {
    trackWindow(m_window);
    return;
  }
  m_item = qobject_cast<QQuickItem *>(parent);
  if (m_item) {
    connect(m_item, &QQuickItem::windowChanged, this, &WindowsWindowAttached::onWindowChanged);
    if (m_item->window()) onWindowChanged(m_item->window());
  }
}

bool WindowsWindowAttached::acrylicSupported() {
  static const bool supported = QOperatingSystemVersion::current() >= QOperatingSystemVersion::Windows11_22H2;
  return supported;
}

bool windowsAcrylicSupported() { return WindowsWindowAttached::acrylicSupported(); }

int WindowsWindowAttached::nativeCornerRadius() { return DWM_ROUND_CORNER_RADIUS; }

void WindowsWindowAttached::setEnabled(bool value) {
  if (m_enabled == value) return;
  m_enabled = value;
  emit enabledChanged();
  if (m_enabled) {
    apply();
  } else {
    revert();
  }
}

void WindowsWindowAttached::setBlurEnabled(bool value) {
  if (m_blurEnabled == value) return;
  m_blurEnabled = value;
  emit blurEnabledChanged();
  apply();
}

void WindowsWindowAttached::setAppearance(const QString &value) {
  if (m_appearance == value) return;
  m_appearance = value;
  emit appearanceChanged();
  apply();
}

void WindowsWindowAttached::setBorderColor(const QColor &value) {
  if (m_borderColor == value) return;
  m_borderColor = value;
  emit borderColorChanged();
  apply();
}

void WindowsWindowAttached::trackWindow(QWindow *window) {
  m_window = window;
  if (!m_window) return;
  m_window->installEventFilter(this);
}

void WindowsWindowAttached::onWindowChanged(QQuickWindow *window) {
  if (m_window) {
    m_window->removeEventFilter(this);
    m_window = nullptr;
    m_surfaceReady = false;
  }
  if (window) {
    trackWindow(window);
    apply();
  }
}

void WindowsWindowAttached::apply() {
  if (!m_window || !m_enabled) return;
  if (!m_surfaceReady) {
    if (!m_window->handle()) return;
    m_surfaceReady = true;
  }

  HWND hwnd = hwndFromWindow(m_window);
  if (!hwnd) return;

  LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
  if (exStyle & WS_EX_LAYERED) SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);

  LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
  if (!(style & WS_THICKFRAME)) SetWindowLongPtrW(hwnd, GWL_STYLE, style | WS_THICKFRAME);
  SetWindowSubclass(hwnd, chromeSubclassProc, CHROME_SUBCLASS_ID, 0);
  SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
               SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

  DWM_WINDOW_CORNER_PREFERENCE corner = DWMWCP_ROUND;
  DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));

  COLORREF border = m_borderColor.isValid() ? colorrefFromQColor(m_borderColor) : DWMWA_COLOR_DEFAULT;
  DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &border, sizeof(border));

  MARGINS margins{-1, -1, -1, -1};
  DwmExtendFrameIntoClientArea(hwnd, &margins);

  BOOL dark = m_appearance == QStringLiteral("dark");
  DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));

  DWM_SYSTEMBACKDROP_TYPE backdrop =
      m_blurEnabled && acrylicSupported() ? DWMSBT_TRANSIENTWINDOW : DWMSBT_NONE;
  DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop, sizeof(backdrop));
}

void WindowsWindowAttached::revert() {
  if (!m_window || !m_window->handle()) return;
  HWND hwnd = hwndFromWindow(m_window);
  if (!hwnd) return;

  RemoveWindowSubclass(hwnd, chromeSubclassProc, CHROME_SUBCLASS_ID);
  LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
  if (style & WS_THICKFRAME) SetWindowLongPtrW(hwnd, GWL_STYLE, style & ~WS_THICKFRAME);
  SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
               SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

  DWM_WINDOW_CORNER_PREFERENCE corner = DWMWCP_DEFAULT;
  DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));

  COLORREF border = DWMWA_COLOR_DEFAULT;
  DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &border, sizeof(border));

  MARGINS margins{0, 0, 0, 0};
  DwmExtendFrameIntoClientArea(hwnd, &margins);

  BOOL dark = FALSE;
  DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));

  DWM_SYSTEMBACKDROP_TYPE backdrop = DWMSBT_AUTO;
  DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop, sizeof(backdrop));
}

bool WindowsWindowAttached::eventFilter(QObject *obj, QEvent *event) {
  if (obj == m_window && event->type() == QEvent::PlatformSurface) {
    auto *se = static_cast<QPlatformSurfaceEvent *>(event); // NOLINT
    if (se->surfaceEventType() == QPlatformSurfaceEvent::SurfaceCreated) {
      m_surfaceReady = true;
      apply();
    } else if (se->surfaceEventType() == QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed) {
      m_surfaceReady = false;
    }
  }
  return QObject::eventFilter(obj, event);
}
