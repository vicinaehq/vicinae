#include <qapplication.h>
#include <qdbusargument.h>
#include <qguiapplication_platform.h>
#include <qlogging.h>
#include <QWindow>
#include <ranges>
#include <QWidget>
#include "utils/qt-wayland-utils.hpp"
#include "kde-window-manager.hpp"
#include "services/window-manager/abstract-window-manager.hpp"
#include <wayland-client-protocol.h>

namespace KDE {

QDBusInterface WindowManager::getKRunnerInterface() {
  return QDBusInterface("org.kde.KWin", "/WindowsRunner", "org.kde.krunner1");
}

WindowManager::WindowList WindowManager::listWindowsSync() const {
  auto iface = getKRunnerInterface();

  if (!iface.isValid()) {
    qWarning() << "krunner interface is not valid";
    return {};
  }

  auto res = iface.call("Match", "");

  if (auto msg = res.errorMessage(); !msg.isEmpty()) {
    qWarning() << "listWindowsSync: failed to find matching windows" << msg;
    return {};
  }

  auto args = res.arguments();

  if (args.empty()) {
    qWarning() << "listWindowsSync: empty list of arguments";
    return {};
  }

  KDE::KRunnerWindowList lst;

  args.front().value<QDBusArgument>() >> lst;

  // we get rid of duplicates, as for some reason some windows seem to be duplicated
  std::ranges::sort(lst.windows, [](const auto &w1, const auto &w2) { return w1.id < w2.id; });
  const auto [first, last] =
      std::ranges::unique(lst.windows, [](const auto &w1, const auto &w2) { return w1.id == w2.id; });
  lst.windows.erase(first, last);

  return lst.windows | std::views::filter([](auto &&w) { return !w.title.isEmpty(); }) |
         std::views::transform(
             [](auto &&w) -> AbstractWindowManager::WindowPtr { return std::make_shared<Window>(w); }) |
         std::ranges::to<std::vector>();
}

void WindowManager::focusWindowSync(const AbstractWindow &window) const {
  QDBusInterface iface = getKRunnerInterface();
  iface.call("Run", window.id(), "");
}

void WindowManager::start() {
  qWarning() << "KDE window management is currently experimental. App to window matching may not work for "
                "some applications.";
}

}; // namespace KDE

const QDBusArgument &operator>>(const QDBusArgument &arg, KDE::KRunnerWindowList &lst) {
  arg.beginArray();
  while (!arg.atEnd()) {
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
