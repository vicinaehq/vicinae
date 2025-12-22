#pragma once
#include "environment.hpp"
#include "services/window-manager/abstract-window-manager.hpp"
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

  QString id() const override { return m_data.id; }
  QString title() const override { return m_data.title; }
  QString wmClass() const override {
    // we don't have direct window class access, so we try to extract what is the most likely
    // to match with it

    for (const char *sep : {" - ", " — "}) {
      if (m_data.title.contains(sep)) { return m_data.title.split(sep).last().trimmed(); }
    }

    return m_data.title;
  }

private:
  KRunnerWindowData m_data;
};

// only for KDE Wayland, X11 uses its own generic window manager implementation
class WindowManager : public AbstractWindowManager {
public:
  QString id() const override { return "kde"; }
  QString displayName() const override { return "KDE"; }

  WindowList listWindowsSync() const override;
  void focusWindowSync(const AbstractWindow &window) const override;

  // I don't know if there is a reliable way to do it on kde, it doesn't have the
  // virtual keyboard protocol
  bool supportsPaste() const override { return false; }

  bool isActivatable() const override { return Environment::isWaylandPlasmaDesktop(); }

  bool ping() const override { return true; }

  void start() override;

private:
  static QDBusInterface getKRunnerInterface();
};
} // namespace KDE

const QDBusArgument &operator>>(const QDBusArgument &argument, KDE::KRunnerWindowList &window);
const QDBusArgument &operator>>(const QDBusArgument &argument, KDE::KRunnerWindowData &window);
