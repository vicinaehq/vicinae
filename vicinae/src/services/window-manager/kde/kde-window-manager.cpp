#include <qdbusargument.h>
#include <ranges>
#include "kde-window-manager.hpp"
#include "services/window-manager/abstract-window-manager.hpp"

namespace KDE {
WindowManager::WindowList WindowManager::listWindowsSync() const {
  auto iface = getKRunnerInterface();
  auto res = iface.call("Match", "");
  auto args = res.arguments();
  KDE::KRunnerWindowList lst;

  args.front().value<QDBusArgument>() >> lst;

  return lst.windows | std::views::transform([](auto &&w) -> AbstractWindowManager::WindowPtr {
           return std::make_shared<Window>(w);
         }) |
         std::ranges::to<std::vector>();
}
}; // namespace KDE

const QDBusArgument &operator>>(const QDBusArgument &arg, KDE::KRunnerWindowList &lst) {
  arg.beginArray();
  while (arg.atEnd()) {
    KDE::KRunnerWindowData win;
    arg >> win;
    lst.windows.emplace_back(win);
  }
  arg.endArray();
  return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, KDE::KRunnerWindowData &win) {
  arg.beginStructure();
  arg >> win.id >> win.title >> win.category >> win.i >> win.d;
  arg.beginStructure();
  arg >> win.s >> win.v;
  arg.endStructure();
  arg.endStructure();

  return arg;
}
