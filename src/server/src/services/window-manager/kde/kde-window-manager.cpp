#include <QCoreApplication>
#include <algorithm>
#include <qdbusconnection.h>
#include <qdbusconnectioninterface.h>
#include <qdbusinterface.h>
#include <qdbusmessage.h>
#include <qdbusservicewatcher.h>
#include <qdir.h>
#include <qlogging.h>
#include <qtemporaryfile.h>
#include <quuid.h>
#include <string_view>
#include "kde-window-manager.hpp"

namespace KDE {

static constexpr const char *KWIN_SERVICE = "org.kde.KWin";
static constexpr const char *SCRIPTING_PATH = "/Scripting";
static constexpr const char *SCRIPTING_IFACE = "org.kde.kwin.Scripting";
static constexpr const char *SCRIPT_IFACE = "org.kde.kwin.Script";
static constexpr const char *TRACKER_SERVICE = "org.vicinae.WindowTracker";
static constexpr const char *TRACKER_PATH = "/";
static constexpr const char *TRACKER_PLUGIN = "vicinae-window-tracker";

// Persistent tracker. Walks stackingOrder once at load to seed our cache,
// then forwards windowAdded / windowRemoved / windowActivated events for the
// rest of the KWin session. Captions can change after add(); we re-emit add()
// from captionChanged so the cache stays current.
static constexpr std::string_view TRACKER_JS = R"JS(
(function () {
  const SVC = "org.vicinae.WindowTracker";
  const P = "/";
  const IF = "org.vicinae.WindowTracker";

  function push(w) {
    if (!w || !w.normalWindow) return;
    callDBus(SVC, P, IF, "add",
      String(w.internalId),
      String(w.resourceClass || ""),
      String(w.resourceName || ""),
      String(w.caption || ""),
      w.pid | 0);
  }

  function hook(w) {
    if (!w || !w.normalWindow) return;
    push(w);
    if (w.captionChanged) {
      w.captionChanged.connect(function () { push(w); });
    }
  }

  try {
    const list = workspace.stackingOrder;
    for (let i = 0; i < list.length; ++i) hook(list[i]);

    const a = workspace.activeWindow;
    callDBus(SVC, P, IF, "activated", a ? String(a.internalId) : "");

    workspace.windowAdded.connect(hook);
    workspace.windowRemoved.connect(function (w) {
      if (w) callDBus(SVC, P, IF, "remove", String(w.internalId));
    });
    workspace.windowActivated.connect(function (w) {
      callDBus(SVC, P, IF, "activated", w ? String(w.internalId) : "");
    });
  } catch (e) {
    callDBus(SVC, P, IF, "error", String(e));
  }
})();
)JS";

// One-shot focus. %1 is replaced with the target UUID before loading.
static constexpr std::string_view FOCUS_JS_TEMPLATE = R"JS(
(function () {
  const target = "%1";
  const list = workspace.stackingOrder;
  for (let i = 0; i < list.length; ++i) {
    if (list[i] && String(list[i].internalId) === target) {
      workspace.activeWindow = list[i];
      return;
    }
  }
})();
)JS";

static QDBusInterface kwinScripting() {
  return QDBusInterface(KWIN_SERVICE, SCRIPTING_PATH, SCRIPTING_IFACE);
}

WindowTracker::WindowTracker(QObject *parent) : QObject(parent) {}

void WindowTracker::reset() {
  bool hadWindows = !m_windows.empty();
  bool hadFocus = !m_focused.isEmpty();
  m_windows.clear();
  m_focused.clear();
  if (hadWindows) emit windowsChanged();
  if (hadFocus) emit focusChanged();
}

void WindowTracker::add(const QString &id, const QString &resourceClass, const QString &resourceName,
                        const QString &caption, int pid) {
  WindowInfo info{
      .id = id, .resourceClass = resourceClass, .resourceName = resourceName, .caption = caption, .pid = pid};
  auto it = std::ranges::find_if(m_windows, [&](const auto &w) { return w.id == id; });
  if (it != m_windows.end()) {
    *it = std::move(info);
  } else {
    m_windows.emplace_back(std::move(info));
  }
  emit windowsChanged();
}

void WindowTracker::remove(const QString &id) {
  auto erased = std::erase_if(m_windows, [&](const auto &w) { return w.id == id; });
  if (erased > 0) emit windowsChanged();
}

void WindowTracker::activated(const QString &id) {
  if (m_focused == id) return;
  m_focused = id;
  emit focusChanged();
}

void WindowTracker::error(const QString &message) { qWarning() << "[kde] tracker script error:" << message; }

WindowManager::WindowManager() = default;

WindowManager::~WindowManager() {
  if (m_serviceOwned) unloadScriptByName(TRACKER_PLUGIN);
}

bool WindowManager::ping() const {
  return QDBusConnection::sessionBus().interface()->isServiceRegistered(KWIN_SERVICE);
}

void WindowManager::start() {
  m_tracker = std::make_unique<WindowTracker>(this);
  connect(m_tracker.get(), &WindowTracker::windowsChanged, this,
          [this]() { emit AbstractWindowManager::windowsChanged(); });

  auto bus = QDBusConnection::sessionBus();
  if (!bus.registerService(TRACKER_SERVICE)) {
    qWarning() << "[kde] could not register" << TRACKER_SERVICE
               << "- another vicinae instance may be running";
    return;
  }
  if (!bus.registerObject(TRACKER_PATH, m_tracker.get(), QDBusConnection::ExportScriptableSlots)) {
    qWarning() << "[kde] could not register tracker object";
    bus.unregisterService(TRACKER_SERVICE);
    return;
  }
  m_serviceOwned = true;

  m_watcher = std::make_unique<QDBusServiceWatcher>(
      KWIN_SERVICE, bus,
      QDBusServiceWatcher::WatchForRegistration | QDBusServiceWatcher::WatchForUnregistration, this);
  connect(m_watcher.get(), &QDBusServiceWatcher::serviceRegistered, this,
          [this](const QString &) { loadTrackerScript(); });
  connect(m_watcher.get(), &QDBusServiceWatcher::serviceUnregistered, this,
          [this](const QString &) { m_tracker->reset(); });

  // we can't do this in destructor because we need to make sure the event loop is still running when we
  // unload
  if (auto *app = QCoreApplication::instance()) {
    connect(app, &QCoreApplication::aboutToQuit, this, [this]() { unloadScriptByName(TRACKER_PLUGIN); });
  }

  loadTrackerScript();
}

bool WindowManager::loadTrackerScript() {
  unloadScriptByName(TRACKER_PLUGIN);
  m_tracker->reset();

  // QTemporaryFile auto-removes on destruction. KWin opens the path during run(), not loadScript(),
  // so we must keep the file alive until after the run reply arrives.
  QTemporaryFile tmp(QDir::tempPath() + "/vicinae-kwin-XXXXXX.js");
  if (!tmp.open()) {
    qWarning() << "[kde] could not create temp file for tracker script";
    return false;
  }
  tmp.write(TRACKER_JS.data(), static_cast<qint64>(TRACKER_JS.size()));
  tmp.flush();

  auto scripting = kwinScripting();
  auto loadReply = scripting.call("loadScript", tmp.fileName(), QString::fromLatin1(TRACKER_PLUGIN));

  if (loadReply.type() == QDBusMessage::ErrorMessage) {
    qWarning() << "[kde] loadScript failed:" << loadReply.errorMessage();
    return false;
  }
  int scriptId = loadReply.arguments().value(0).toInt();
  if (scriptId < 0) {
    qWarning() << "[kde] loadScript returned" << scriptId;
    return false;
  }

  QDBusInterface script(KWIN_SERVICE, QString("/Scripting/Script%1").arg(scriptId), SCRIPT_IFACE);
  auto runReply = script.call("run");
  if (runReply.type() == QDBusMessage::ErrorMessage) {
    qWarning() << "[kde] tracker script run failed:" << runReply.errorMessage();
    return false;
  }
  return true;
}

void WindowManager::unloadScriptByName(const QString &pluginName) const {
  kwinScripting().call("unloadScript", pluginName);
}

bool WindowManager::runOneShot(const QString &source, const QString &pluginName) const {
  QTemporaryFile tmp(QDir::tempPath() + "/vicinae-kwin-XXXXXX.js");
  if (!tmp.open()) return false;
  tmp.write(source.toUtf8());
  tmp.flush();

  auto scripting = kwinScripting();
  auto loadReply = scripting.call("loadScript", tmp.fileName(), pluginName);
  if (loadReply.type() == QDBusMessage::ErrorMessage) {
    qWarning() << "[kde] one-shot loadScript failed:" << loadReply.errorMessage();
    return false;
  }
  int scriptId = loadReply.arguments().value(0).toInt();
  if (scriptId < 0) {
    qWarning() << "[kde] one-shot loadScript returned" << scriptId;
    return false;
  }
  QDBusInterface script(KWIN_SERVICE, QString("/Scripting/Script%1").arg(scriptId), SCRIPT_IFACE);
  script.call("run");
  unloadScriptByName(pluginName);
  return true;
}

WindowManager::WindowList WindowManager::listWindowsSync() const {
  if (!m_tracker) return {};
  auto snap = m_tracker->snapshot();
  WindowList result;
  result.reserve(snap.size());
  for (auto &info : snap) {
    result.emplace_back(std::make_shared<Window>(std::move(info)));
  }
  return result;
}

WindowManager::WindowPtr WindowManager::getFocusedWindowSync() const {
  if (!m_tracker) return nullptr;
  const auto id = m_tracker->focusedId();
  if (id.isEmpty()) return nullptr;
  for (auto &info : m_tracker->snapshot()) {
    if (info.id == id) return std::make_shared<Window>(std::move(info));
  }
  return nullptr;
}

void WindowManager::focusWindowSync(const AbstractWindow &window) const {
  const QString source =
      QString::fromUtf8(FOCUS_JS_TEMPLATE.data(), static_cast<int>(FOCUS_JS_TEMPLATE.size()))
          .arg(window.id());
  const QString pluginName =
      QString("vicinae-focus-%1").arg(QUuid::createUuid().toString(QUuid::WithoutBraces));
  runOneShot(source, pluginName);
}

} // namespace KDE
