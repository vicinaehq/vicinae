#pragma once
#include <QObject>
#include <QString>
#include <optional>
#include <vector>
#include <windows.h>
#include <shobjidl_core.h>
#include <wrl/client.h>
#include "utils/scoped-com.hpp"

class QWinEventNotifier;

namespace Win {

/**
 * Windows virtual desktops, using only stable mechanisms: the explorer registry state for
 * enumeration/names/active desktop, and the public IVirtualDesktopManager for per-window desktop
 * queries and moving our own windows. The undocumented IVirtualDesktopManagerInternal (whose COM
 * GUIDs break between Windows builds) is deliberately not used.
 */
class VirtualDesktops : public QObject {
  Q_OBJECT

signals:
  /** Desktops were created, removed, renamed, reordered, or the active desktop changed. */
  void changed();

public:
  struct Desktop {
    QString id; // GUID in registry format, e.g. "{B514E313-...}"
    QString name;
  };

  explicit VirtualDesktops(QObject *parent = nullptr);
  ~VirtualDesktops() override;

  bool available() const { return m_manager && !m_desktops.empty(); }
  const std::vector<Desktop> &desktops() const { return m_desktops; }
  QString activeDesktopId() const;

  std::optional<QString> windowDesktopId(HWND hwnd) const;
  bool isWindowOnCurrentDesktop(HWND hwnd) const;

  /** Only works for windows owned by the calling process. */
  bool moveOwnWindowToDesktop(HWND hwnd, const QString &desktopId) const;

private:
  void reload();
  void armWatcher();

  ScopedCom m_com;
  Microsoft::WRL::ComPtr<IVirtualDesktopManager> m_manager;
  std::vector<Desktop> m_desktops;
  HKEY m_watchKey = nullptr;
  HANDLE m_watchEvent = nullptr;
  QWinEventNotifier *m_notifier = nullptr;
};

} // namespace Win
