#include "ui/quick/windows-chrome-attached.hpp"

#include <QOperatingSystemVersion>
#include <QQuickItem>
#include <QQuickWindow>
#include <QtMath>
#include <algorithm>
#include <qevent.h>

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <dwmapi.h>

namespace {

constexpr int DWM_ROUND_CORNER_RADIUS = 8;
constexpr UINT_PTR CHROME_SUBCLASS_ID = 1;
constexpr UINT_PTR TITLEBAR_SUBCLASS_ID = 2;
constexpr int TITLEBAR_EXTEND_SLACK_PX = 1;

HWND hwndFromWindow(QWindow *window) { return reinterpret_cast<HWND>(window->winId()); }

COLORREF colorrefFromQColor(const QColor &c) { return RGB(c.red(), c.green(), c.blue()); }

int frameThickness(HWND hwnd) {
  const UINT dpi = GetDpiForWindow(hwnd);
  return GetSystemMetricsForDpi(SM_CXSIZEFRAME, dpi) + GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
}

void showSystemMenu(HWND hwnd, int x, int y) {
  HMENU menu = GetSystemMenu(hwnd, FALSE);
  if (!menu) return;
  const UINT cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, x, y, 0, hwnd, nullptr);
  if (cmd) PostMessageW(hwnd, WM_SYSCOMMAND, cmd, 0);
}

LRESULT CALLBACK chromeSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR) {
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

LRESULT CALLBACK titlebarSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR,
                                      DWORD_PTR ref) {
  auto *self = reinterpret_cast<WindowsWindowAttached *>(ref); // NOLINT
  LRESULT dwmResult = 0;
  if (DwmDefWindowProc(hwnd, msg, wParam, lParam, &dwmResult)) return dwmResult;
  switch (msg) {
  case WM_NCCALCSIZE:
    if (wParam == TRUE) {
      if (IsZoomed(hwnd)) {
        auto *params = reinterpret_cast<NCCALCSIZE_PARAMS *>(lParam); // NOLINT
        const int frame = frameThickness(hwnd);
        InflateRect(&params->rgrc[0], -frame, -frame);
      }
      return 0;
    }
    break;
  case WM_STYLECHANGING:
    if (wParam == GWL_STYLE)
      reinterpret_cast<STYLESTRUCT *>(lParam)->styleNew |= self->extendedStyle(); // NOLINT
    break;
  case WM_NCHITTEST: {
    POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
    ScreenToClient(hwnd, &pt);
    return self->hitTest(QPoint(pt.x, pt.y));
  }
  case WM_NCRBUTTONUP:
    if (wParam == HTCAPTION) {
      showSystemMenu(hwnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
      return 0;
    }
    break;
  case WM_NCACTIVATE:
    return DefWindowProcW(hwnd, msg, wParam, -1);
  case WM_SIZE:
    self->updateCaptionButtonsRect();
    break;
  case WM_DPICHANGED: {
    const LRESULT result = DefSubclassProc(hwnd, msg, wParam, lParam);
    self->extendTitlebar();
    return result;
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

WindowsWindowAttached::~WindowsWindowAttached() {
  if (m_subclassed && m_surfaceReady && m_window) {
    RemoveWindowSubclass(hwndFromWindow(m_window), titlebarSubclassProc, TITLEBAR_SUBCLASS_ID);
  }
}

bool WindowsWindowAttached::acrylicSupported() {
  static const bool supported = QOperatingSystemVersion::current() >= QOperatingSystemVersion::Windows11_22H2;
  return supported;
}

bool WindowsWindowAttached::extendedTitlebarSupported() {
  static const bool supported = QOperatingSystemVersion::current() >= QOperatingSystemVersion::Windows11;
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

void WindowsWindowAttached::setDecorated(bool value) {
  if (m_decorated == value) return;
  m_decorated = value;
  emit decoratedChanged();
  apply();
}

void WindowsWindowAttached::setTitlebarHeight(int value) {
  if (m_titlebarHeight == value) return;
  m_titlebarHeight = value;
  emit titlebarHeightChanged();
  apply();
}

QQmlListProperty<QQuickItem> WindowsWindowAttached::titlebarControls() {
  return QQmlListProperty<QQuickItem>(
      this, this,
      [](QQmlListProperty<QQuickItem> *list, QQuickItem *item) {
        auto *self = static_cast<WindowsWindowAttached *>(list->data);
        self->m_titlebarControls.reserve(self->m_titlebarControls.size() + 1);
        self->m_titlebarControls.emplace_back(item);
      },
      [](QQmlListProperty<QQuickItem> *list) -> qsizetype {
        return static_cast<WindowsWindowAttached *>(list->data)->m_titlebarControls.size();
      },
      [](QQmlListProperty<QQuickItem> *list, qsizetype index) -> QQuickItem * {
        return static_cast<WindowsWindowAttached *>(list->data)->m_titlebarControls[index];
      },
      [](QQmlListProperty<QQuickItem> *list) {
        static_cast<WindowsWindowAttached *>(list->data)->m_titlebarControls.clear();
      });
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

void WindowsWindowAttached::setCaptionColor(const QColor &value) {
  if (m_captionColor == value) return;
  m_captionColor = value;
  emit captionColorChanged();
  apply();
}

void WindowsWindowAttached::setCaptionTitleVisible(bool value) {
  if (m_captionTitleVisible == value) return;
  m_captionTitleVisible = value;
  emit captionTitleVisibleChanged();
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
    m_subclassed = false;
  }
  if (window) {
    trackWindow(window);
    apply();
  }
}

long WindowsWindowAttached::extendedStyle() const {
  if (!m_window) return 0;
  const auto flags = m_window->flags();
  long style = WS_CAPTION | WS_SYSMENU;
  if (m_window->minimumSize() != m_window->maximumSize()) style |= WS_THICKFRAME;
  if (flags & Qt::WindowMinimizeButtonHint) style |= WS_MINIMIZEBOX;
  if (flags & Qt::WindowMaximizeButtonHint) style |= WS_MAXIMIZEBOX;
  return style;
}

long WindowsWindowAttached::hitTest(const QPoint &clientPx) const {
  if (!m_window || !m_surfaceReady) return HTCLIENT;
  HWND hwnd = hwndFromWindow(m_window);
  RECT rc{};
  GetClientRect(hwnd, &rc);
  const LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
  if ((style & WS_THICKFRAME) && !IsZoomed(hwnd)) {
    const int frame = frameThickness(hwnd);
    const bool horizontal = m_window->minimumWidth() != m_window->maximumWidth();
    const bool vertical = m_window->minimumHeight() != m_window->maximumHeight();
    const int col = !horizontal ? 0 : clientPx.x() < frame ? -1 : clientPx.x() >= rc.right - frame ? 1 : 0;
    const int row = !vertical ? 0 : clientPx.y() < frame ? -1 : clientPx.y() >= rc.bottom - frame ? 1 : 0;
    static constexpr long HITS[3][3] = {
        {HTTOPLEFT, HTTOP, HTTOPRIGHT}, {HTLEFT, HTCLIENT, HTRIGHT}, {HTBOTTOMLEFT, HTBOTTOM, HTBOTTOMRIGHT}};
    if (row || col) return HITS[row + 1][col + 1];
  }
  const qreal dpr = m_window->devicePixelRatio();
  if (clientPx.y() >= m_titlebarHeight * dpr) return HTCLIENT;
  const QPointF scene(clientPx.x() / dpr, clientPx.y() / dpr);
  const bool overControl = std::ranges::any_of(m_titlebarControls, [&](const auto &item) {
    return item && item->isVisible() && item->window() == m_window &&
           item->contains(item->mapFromScene(scene));
  });
  return overControl ? HTCLIENT : HTCAPTION;
}

void WindowsWindowAttached::updateCaptionButtonsRect() {
  QRectF rect;
  if (m_window && m_surfaceReady && extended()) {
    HWND hwnd = hwndFromWindow(m_window);
    RECT bounds{};
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CAPTION_BUTTON_BOUNDS, &bounds, sizeof(bounds)))) {
      RECT wr{};
      GetWindowRect(hwnd, &wr);
      POINT origin{wr.left + bounds.left, wr.top + bounds.top};
      ScreenToClient(hwnd, &origin);
      const qreal dpr = m_window->devicePixelRatio();
      rect = QRectF(origin.x / dpr, origin.y / dpr, (bounds.right - bounds.left) / dpr,
                    (bounds.bottom - bounds.top) / dpr);
    }
  }
  if (rect == m_captionButtonsRect) return;
  m_captionButtonsRect = rect;
  emit captionButtonsRectChanged();
}

void WindowsWindowAttached::extendTitlebar() {
  if (!m_window || !m_surfaceReady) return;
  HWND hwnd = hwndFromWindow(m_window);
  const int band = qCeil(m_titlebarHeight * m_window->devicePixelRatio()) + TITLEBAR_EXTEND_SLACK_PX;
  MARGINS margins{0, 0, band, 0};
  DwmExtendFrameIntoClientArea(hwnd, &margins);
  updateCaptionButtonsRect();
}

void WindowsWindowAttached::apply() {
  if (!m_window || !m_enabled) return;
  if (!m_decorated && !acrylicSupported()) return;
  if (!m_surfaceReady) {
    if (!m_window->handle()) return;
    m_surfaceReady = true;
  }

  HWND hwnd = hwndFromWindow(m_window);
  if (!hwnd) return;

  LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
  if (exStyle & WS_EX_LAYERED) SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);

  if (!m_decorated) {
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    if (!(style & WS_THICKFRAME)) SetWindowLongPtrW(hwnd, GWL_STYLE, style | WS_THICKFRAME);
    SetWindowSubclass(hwnd, chromeSubclassProc, CHROME_SUBCLASS_ID, 0);
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

    DWM_WINDOW_CORNER_PREFERENCE corner = DWMWCP_ROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));

    COLORREF border = m_borderColor.isValid() ? colorrefFromQColor(m_borderColor) : DWMWA_COLOR_DEFAULT;
    DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &border, sizeof(border));
  } else {
    if (extended()) {
      if (!m_subclassed) {
        SetWindowSubclass(hwnd, titlebarSubclassProc, TITLEBAR_SUBCLASS_ID,
                          reinterpret_cast<DWORD_PTR>(this));
        m_subclassed = true;
      }
      LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
      if ((style | extendedStyle()) != style) SetWindowLongPtrW(hwnd, GWL_STYLE, style | extendedStyle());
      SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                   SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

      DWM_WINDOW_CORNER_PREFERENCE corner = DWMWCP_ROUND;
      DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));
    }

    COLORREF caption = m_captionColor.isValid() ? colorrefFromQColor(m_captionColor) : DWMWA_COLOR_DEFAULT;
    DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR, &caption, sizeof(caption));

    COLORREF text = m_captionTitleVisible || !m_captionColor.isValid() ? DWMWA_COLOR_DEFAULT : caption;
    DwmSetWindowAttribute(hwnd, DWMWA_TEXT_COLOR, &text, sizeof(text));

    exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    LONG_PTR iconless =
        m_captionTitleVisible ? exStyle & ~WS_EX_DLGMODALFRAME : exStyle | WS_EX_DLGMODALFRAME;
    if (iconless != exStyle) {
      SetWindowLongPtrW(hwnd, GWL_EXSTYLE, iconless);
      SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                   SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }
  }

  DWM_SYSTEMBACKDROP_TYPE backdrop = DWMSBT_NONE;
  if (m_blurEnabled && acrylicSupported())
    backdrop = m_decorated ? DWMSBT_MAINWINDOW : DWMSBT_TRANSIENTWINDOW;

  BOOL dark = m_appearance == QStringLiteral("dark");
  DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));

  DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop, sizeof(backdrop));

  if (!m_decorated || backdrop != DWMSBT_NONE) {
    MARGINS margins{-1, -1, -1, -1};
    DwmExtendFrameIntoClientArea(hwnd, &margins);
  } else if (extended()) {
    extendTitlebar();
  } else {
    MARGINS margins{0, 0, 0, 0};
    DwmExtendFrameIntoClientArea(hwnd, &margins);
  }
  updateCaptionButtonsRect();
}

void WindowsWindowAttached::revert() {
  if (!m_window || !m_window->handle()) return;
  if (!m_decorated && !acrylicSupported()) return;
  HWND hwnd = hwndFromWindow(m_window);
  if (!hwnd) return;

  if (!m_decorated) {
    RemoveWindowSubclass(hwnd, chromeSubclassProc, CHROME_SUBCLASS_ID);
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    if (style & WS_THICKFRAME) SetWindowLongPtrW(hwnd, GWL_STYLE, style & ~WS_THICKFRAME);
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

    DWM_WINDOW_CORNER_PREFERENCE corner = DWMWCP_DEFAULT;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));

    COLORREF border = DWMWA_COLOR_DEFAULT;
    DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &border, sizeof(border));
  } else {
    if (m_subclassed) {
      RemoveWindowSubclass(hwnd, titlebarSubclassProc, TITLEBAR_SUBCLASS_ID);
      m_subclassed = false;
      LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
      SetWindowLongPtrW(hwnd, GWL_STYLE, style & ~extendedStyle());
      SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                   SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
      DWM_WINDOW_CORNER_PREFERENCE corner = DWMWCP_DEFAULT;
      DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));
    }
    COLORREF caption = DWMWA_COLOR_DEFAULT;
    DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR, &caption, sizeof(caption));
    DwmSetWindowAttribute(hwnd, DWMWA_TEXT_COLOR, &caption, sizeof(caption));
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_DLGMODALFRAME) {
      SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_DLGMODALFRAME);
      SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                   SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }
  }

  MARGINS margins{0, 0, 0, 0};
  DwmExtendFrameIntoClientArea(hwnd, &margins);

  BOOL dark = FALSE;
  DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));

  DWM_SYSTEMBACKDROP_TYPE backdrop = DWMSBT_AUTO;
  DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop, sizeof(backdrop));
  updateCaptionButtonsRect();
}

bool WindowsWindowAttached::eventFilter(QObject *obj, QEvent *event) {
  if (obj == m_window && event->type() == QEvent::PlatformSurface) {
    auto *se = static_cast<QPlatformSurfaceEvent *>(event); // NOLINT
    if (se->surfaceEventType() == QPlatformSurfaceEvent::SurfaceCreated) {
      m_surfaceReady = true;
      apply();
    } else if (se->surfaceEventType() == QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed) {
      m_surfaceReady = false;
      m_subclassed = false;
    }
  }
  return QObject::eventFilter(obj, event);
}
