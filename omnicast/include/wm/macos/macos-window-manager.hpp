#include "wm/window-manager.hpp"
#include <qguiapplication.h>
#include <qtcoreexports.h>

class MacOSWindowManager : public AbstractWindowManager {
  bool isActivatable() const override { return QGuiApplication::platformName() == "cocoa"; }

  void start() const override {}

  QString name() const override { return "MacOS"; }

  bool ping() const override { return false; }
};
