#pragma once
#include "environment.hpp"
#include "services/window-manager/abstract-window-manager.hpp"
#include "services/window-manager/window-manager.hpp"
#include <qdbusargument.h>
#include <qdbusinterface.h>

namespace KDE {
struct KRunnerWindowData {
  QString id;
  QString title;
  QString category;
  int i;    // not sure what this is
  double d; // not sure what this is
  QString s;
  QVariant v;
};

struct KRunnerWindowList {
  std::vector<KRunnerWindowData> windows;
};

class Window : public AbstractWindowManager::AbstractWindow {
public:
  Window(const KRunnerWindowData &data) : m_data(data) {}
  QString id() const override { return ""; }
  QString title() const override { return ""; }
  QString wmClass() const override { return title(); }

private:
  KRunnerWindowData m_data;
};

// only for KDE Wayland, X11 uses its own generic window manager implementation
class WindowManager : public AbstractWindowManager {
  QString id() const override { return "kde"; }
  QString displayName() const override { return "KDE"; }

  QDBusInterface getKRunnerInterface() const {
    return QDBusInterface("org.kde.KWin", "/WindowsRunner", "org.kde.krunner1");
  }

  WindowList listWindowsSync() const override;

  // I don't know if there is a reliable way to do it on kde, it doesn't have the
  // virtual keyboard protocol
  bool supportsPaste() const override { return false; }

  bool isActivatable() const override { return Environment::isWaylandPlasmaDesktop(); }

  bool ping() const override { return true; }

  void start() override {}
};
} // namespace KDE

const QDBusArgument &operator>>(const QDBusArgument &argument, KDE::KRunnerWindowList &window);
const QDBusArgument &operator>>(const QDBusArgument &argument, KDE::KRunnerWindowData &window);
